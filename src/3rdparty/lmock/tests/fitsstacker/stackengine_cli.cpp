#include "stackengine_cli.h"
#ifndef emit
#define emit
#endif
#include <fitsio.h>
#include <opencv2/imgproc.hpp>
// opencv2/calib3d.hpp no longer needed (findHomography replaced)
#include <algorithm>
#include <cmath>
#include <numeric>
#include <thread>
#include <future>
#include <set>
#include <map>
#include <random>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

// ─── Parallel helper (replaces cv::parallel_for_) ───────────────────────────
template<typename F>
static void parallelFor(int start, int end, F &&body)
{
    int n = std::max(1, (int)std::thread::hardware_concurrency());
    int total = end - start;
    if (total <= 0) return;
    if (n > total) n = total;
    std::vector<std::thread> threads(n);
    for (int t = 0; t < n; t++) {
        int lo = start + t * total / n;
        int hi = start + (t + 1) * total / n;
        threads[t] = std::thread([lo, hi, &body]() { body(lo, hi); });
    }
    for (auto &t : threads) t.join();
}

// ─── Channel split/merge (replaces cv::split / cv::merge) ──────────────────
static void splitChannels(const cv::Mat &src, std::vector<cv::Mat> &dst)
{
    int nc = src.channels();
    dst.resize(nc);
    for (int c = 0; c < nc; c++)
        dst[c] = cv::Mat(src.rows, src.cols, CV_MAKETYPE(src.depth(), 1));
    for (int y = 0; y < src.rows; y++) {
        const uchar *srow = src.ptr(y);
        int elemSize1 = (int)src.elemSize1();
        int step = nc * elemSize1;
        for (int c = 0; c < nc; c++) {
            uchar *drow = dst[c].ptr(y);
            const uchar *sp = srow + c * elemSize1;
            for (int x = 0; x < src.cols; x++) {
                std::memcpy(drow + x * elemSize1, sp + x * step, elemSize1);
            }
        }
    }
}

static void mergeChannels(const std::vector<cv::Mat> &src, cv::Mat &dst)
{
    int nc = (int)src.size();
    if (nc == 0) return;
    dst = cv::Mat(src[0].rows, src[0].cols, CV_MAKETYPE(src[0].depth(), nc));
    int elemSize1 = (int)src[0].elemSize1();
    int step = nc * elemSize1;
    for (int y = 0; y < dst.rows; y++) {
        uchar *drow = dst.ptr(y);
        for (int c = 0; c < nc; c++) {
            const uchar *srow = src[c].ptr(y);
            for (int x = 0; x < dst.cols; x++) {
                std::memcpy(drow + x * step + c * elemSize1, srow + x * elemSize1, elemSize1);
            }
        }
    }
}

// ─── SVD least-squares solver (replaces cv::solve with DECOMP_SVD) ─────────
// Golub-Reinsch SVD: decomposes A (m x n) into U*W*Vt, then solves Ax=b.
// Returns false if decomposition fails.
static bool solveSVD(const cv::Mat &A, const cv::Mat &b, cv::Mat &x)
{
    int m = A.rows, n = A.cols;
    // Copy A into U (will be overwritten)
    std::vector<double> U(m * n), W(n), rv1(n);
    std::vector<double> V(n * n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            U[i * n + j] = A.at<double>(i, j);

    // Householder bidiagonalization
    double g = 0, scale = 0, anorm = 0;
    for (int i = 0; i < n; i++) {
        int l = i + 1;
        rv1[i] = scale * g;
        g = scale = 0;
        if (i < m) {
            for (int k = i; k < m; k++) scale += std::abs(U[k*n+i]);
            if (scale != 0) {
                double s = 0;
                for (int k = i; k < m; k++) { U[k*n+i] /= scale; s += U[k*n+i]*U[k*n+i]; }
                double f = U[i*n+i];
                g = -std::copysign(std::sqrt(s), f);
                double h = f*g - s;
                U[i*n+i] = f - g;
                for (int j = l; j < n; j++) {
                    double s2 = 0;
                    for (int k = i; k < m; k++) s2 += U[k*n+i]*U[k*n+j];
                    double f2 = s2 / h;
                    for (int k = i; k < m; k++) U[k*n+j] += f2*U[k*n+i];
                }
                for (int k = i; k < m; k++) U[k*n+i] *= scale;
            }
        }
        W[i] = scale * g;
        g = scale = 0;
        if (i < m && i != n-1) {
            for (int k = l; k < n; k++) scale += std::abs(U[i*n+k]);
            if (scale != 0) {
                double s = 0;
                for (int k = l; k < n; k++) { U[i*n+k] /= scale; s += U[i*n+k]*U[i*n+k]; }
                double f = U[i*n+l];
                g = -std::copysign(std::sqrt(s), f);
                double h = f*g - s;
                U[i*n+l] = f - g;
                for (int k = l; k < n; k++) rv1[k] = U[i*n+k] / h;
                for (int j = l; j < m; j++) {
                    double s2 = 0;
                    for (int k = l; k < n; k++) s2 += U[j*n+k]*U[i*n+k];
                    for (int k = l; k < n; k++) U[j*n+k] += s2*rv1[k];
                }
                for (int k = l; k < n; k++) U[i*n+k] *= scale;
            }
        }
        anorm = std::max(anorm, std::abs(W[i]) + std::abs(rv1[i]));
    }

    // Accumulate right-hand transformations (V)
    for (int i = n-1; i >= 0; i--) {
        int l = i + 1;
        if (i < n-1) {
            if (g != 0) {
                for (int j = l; j < n; j++) V[j*n+i] = (U[i*n+j]/U[i*n+l]) / g;
                for (int j = l; j < n; j++) {
                    double s = 0;
                    for (int k = l; k < n; k++) s += U[i*n+k]*V[k*n+j];
                    for (int k = l; k < n; k++) V[k*n+j] += s*V[k*n+i];
                }
            }
            for (int j = l; j < n; j++) V[i*n+j] = V[j*n+i] = 0;
        }
        V[i*n+i] = 1;
        g = rv1[i];
    }

    // Accumulate left-hand transformations (U)
    for (int i = std::min(m,n)-1; i >= 0; i--) {
        int l = i + 1;
        g = W[i];
        for (int j = l; j < n; j++) U[i*n+j] = 0;
        if (g != 0) {
            g = 1.0 / g;
            for (int j = l; j < n; j++) {
                double s = 0;
                for (int k = l; k < m; k++) s += U[k*n+i]*U[k*n+j];
                double f = (s / U[i*n+i]) * g;
                for (int k = i; k < m; k++) U[k*n+j] += f*U[k*n+i];
            }
            for (int j = i; j < m; j++) U[j*n+i] *= g;
        } else {
            for (int j = i; j < m; j++) U[j*n+i] = 0;
        }
        U[i*n+i] += 1;
    }

    // Diagonalization (QR iteration)
    for (int k = n-1; k >= 0; k--) {
        for (int its = 0; its < 30; its++) {
            int l;
            bool flag = true;
            int nm = k;
            for (l = k; l >= 0; l--) {
                nm = l - 1;
                if (std::abs(rv1[l]) + anorm == anorm) { flag = false; break; }
                if (nm >= 0 && std::abs(W[nm]) + anorm == anorm) break;
            }
            if (flag) {
                double c = 0, s = 1;
                for (int i = l; i <= k; i++) {
                    double f = s * rv1[i];
                    rv1[i] *= c;
                    if (std::abs(f) + anorm == anorm) break;
                    g = W[i];
                    double h = std::hypot(f, g);
                    W[i] = h;
                    h = 1.0 / h;
                    c = g * h;
                    s = -f * h;
                    for (int j = 0; j < m; j++) {
                        double y = U[j*n+nm], z = U[j*n+i];
                        U[j*n+nm] = y*c + z*s;
                        U[j*n+i] = z*c - y*s;
                    }
                }
            }
            double z = W[k];
            if (l == k) {
                if (z < 0) { W[k] = -z; for (int j = 0; j < n; j++) V[j*n+k] = -V[j*n+k]; }
                break;
            }
            if (its == 29) return false;  // no convergence
            double xx = W[l], y = W[k-1];
            g = rv1[k-1]; double h = rv1[k];
            double f = ((y-z)*(y+z) + (g-h)*(g+h)) / (2*h*y);
            g = std::hypot(f, 1.0);
            f = ((xx-z)*(xx+z) + h*(y/(f + std::copysign(g, f)) - h)) / xx;
            double c = 1, s = 1;
            for (int j = l; j < k; j++) {
                int i = j + 1;
                g = rv1[i]; y = W[i];
                h = s*g; g = c*g;
                z = std::hypot(f, h);
                rv1[j] = z;
                c = f/z; s = h/z;
                f = xx*c + g*s; g = g*c - xx*s;
                h = y*s; y *= c;
                for (int jj = 0; jj < n; jj++) {
                    xx = V[jj*n+j]; z = V[jj*n+i];
                    V[jj*n+j] = xx*c + z*s;
                    V[jj*n+i] = z*c - xx*s;
                }
                z = std::hypot(f, h);
                W[j] = z;
                if (z != 0) { z = 1.0/z; c = f*z; s = h*z; }
                f = c*g + s*y; xx = c*y - s*g;
                for (int jj = 0; jj < m; jj++) {
                    y = U[jj*n+j]; z = U[jj*n+i];
                    U[jj*n+j] = y*c + z*s;
                    U[jj*n+i] = z*c - y*s;
                }
            }
            rv1[l] = 0; rv1[k] = f; W[k] = xx;
        }
    }

    // Backsubstitution: x = V * diag(1/W) * Ut * b
    double wmax = *std::max_element(W.begin(), W.end());
    double thresh = 1e-12 * wmax;
    x = cv::Mat(n, 1, CV_64F);
    std::vector<double> tmp(n);
    for (int j = 0; j < n; j++) {
        double s = 0;
        if (W[j] > thresh) {
            for (int i = 0; i < m; i++) s += U[i*n+j] * b.at<double>(i, 0);
            s /= W[j];
        }
        tmp[j] = s;
    }
    for (int j = 0; j < n; j++) {
        double s = 0;
        for (int jj = 0; jj < n; jj++) s += V[j*n+jj] * tmp[jj];
        x.at<double>(j, 0) = s;
    }
    return true;
}

StackEngine::StackEngine() {}

// ─── FITS I/O ───────────────────────────────────────────────────────────────

cv::Mat StackEngine::loadFits(const QString &path)
{
    fitsfile *fptr = nullptr;
    int status = 0;
    std::string spath = path.toStdString();
    fits_open_file(&fptr, spath.c_str(), READONLY, &status);
    if (status) { qWarning() << "FITS open error" << status << path; return {}; }

    int naxis = 0;
    fits_get_img_dim(fptr, &naxis, &status);
    long naxes[3] = {1, 1, 1};
    fits_get_img_size(fptr, 3, naxes, &status);
    int w = naxes[0], h = naxes[1], c = (naxis >= 3) ? naxes[2] : 1;

    long nelements = (long)w * h * c;
    std::vector<float> buf(nelements);
    long fpixel[3] = {1, 1, 1};
    fits_read_pix(fptr, TFLOAT, fpixel, nelements, nullptr, buf.data(), nullptr, &status);
    fits_close_file(fptr, &status);
    if (status) { qWarning() << "FITS read error" << status; return {}; }

    if (c == 1) {
        cv::Mat img(h, w, CV_32FC1, buf.data());
        return img.clone();
    }
    std::vector<cv::Mat> planes;
    for (int i = 0; i < std::min(c, 3); i++) {
        cv::Mat plane(h, w, CV_32FC1, buf.data() + (long)i * w * h);
        planes.push_back(plane.clone());
    }
    cv::Mat merged;
    mergeChannels(planes, merged);
    return merged;
}

bool StackEngine::saveFits(const QString &path, const cv::Mat &image)
{
    fitsfile *fptr = nullptr;
    int status = 0;
    std::string spath = "!" + path.toStdString();
    int c = image.channels();
    long naxes[3] = {image.cols, image.rows, c};
    int naxis = (c > 1) ? 3 : 2;

    fits_create_file(&fptr, spath.c_str(), &status);
    fits_create_img(fptr, FLOAT_IMG, naxis, naxes, &status);
    std::vector<cv::Mat> channels;
    splitChannels(image, channels);
    for (int i = 0; i < c; i++) {
        long fpixel[3] = {1, 1, i + 1};
        cv::Mat cont;
        if (!channels[i].isContinuous()) channels[i].copyTo(cont);
        else cont = channels[i];
        fits_write_pix(fptr, TFLOAT, fpixel, (long)image.cols * image.rows, cont.data, &status);
    }
    fits_close_file(fptr, &status);
    return status == 0;
}

// ─── Calibration (bias, dark, flat) ─────────────────────────────────────────

void StackEngine::loadCalibrationFrames(const StackParams &params)
{
    m_masterBias = cv::Mat();
    m_masterDark = cv::Mat();
    m_masterFlat = cv::Mat();

    if (!params.masterBiasPath.isEmpty()) {
        m_masterBias = loadFits(params.masterBiasPath);
        if (!m_masterBias.empty())
            qDebug() << "Loaded master bias:" << params.masterBiasPath
                     << m_masterBias.cols << "x" << m_masterBias.rows;
    }
    if (!params.masterDarkPath.isEmpty()) {
        m_masterDark = loadFits(params.masterDarkPath);
        if (!m_masterDark.empty()) {
            // If bias is also provided, subtract it from the dark so we don't
            // double-subtract the bias signal (raw darks include bias).
            if (!m_masterBias.empty() &&
                m_masterBias.rows == m_masterDark.rows &&
                m_masterBias.cols == m_masterDark.cols &&
                m_masterBias.channels() == m_masterDark.channels()) {
                m_masterDark -= m_masterBias;
                qDebug() << "Loaded master dark (bias-subtracted):" << params.masterDarkPath
                         << m_masterDark.cols << "x" << m_masterDark.rows;
            } else {
                qDebug() << "Loaded master dark:" << params.masterDarkPath
                         << m_masterDark.cols << "x" << m_masterDark.rows;
            }
        }
    }
    if (!params.masterFlatPath.isEmpty()) {
        m_masterFlat = loadFits(params.masterFlatPath);
        if (!m_masterFlat.empty()) {
            // Subtract bias from flat before normalizing (flats include bias)
            if (!m_masterBias.empty() &&
                m_masterBias.rows == m_masterFlat.rows &&
                m_masterBias.cols == m_masterFlat.cols &&
                m_masterBias.channels() == m_masterFlat.channels()) {
                m_masterFlat -= m_masterBias;
            }
            // Normalize flat so median ≈ 1.0
            int totalPix = m_masterFlat.rows * m_masterFlat.cols * m_masterFlat.channels();
            std::vector<float> vals(totalPix);
            int idx = 0;
            for (int y = 0; y < m_masterFlat.rows; y++) {
                const float *p = m_masterFlat.ptr<float>(y);
                int nc = m_masterFlat.channels();
                for (int x = 0; x < m_masterFlat.cols * nc; x++)
                    vals[idx++] = p[x];
            }
            std::nth_element(vals.begin(), vals.begin() + vals.size()/2, vals.end());
            float flatMedian = vals[vals.size()/2];
            if (flatMedian > 0)
                m_masterFlat /= flatMedian;
            qDebug() << "Loaded master flat:" << params.masterFlatPath
                     << m_masterFlat.cols << "x" << m_masterFlat.rows
                     << "(bias-subtracted, median normalized to 1.0)";
        }
    }
}

cv::Mat StackEngine::calibrateFrame(const cv::Mat &raw) const
{
    if (m_masterBias.empty() && m_masterDark.empty() && m_masterFlat.empty())
        return raw;  // no calibration needed

    cv::Mat result = raw.clone();

    // Subtract bias (offset)
    if (!m_masterBias.empty() &&
        m_masterBias.rows == result.rows && m_masterBias.cols == result.cols &&
        m_masterBias.channels() == result.channels()) {
        result -= m_masterBias;
    }

    // Subtract dark (thermal noise)
    if (!m_masterDark.empty() &&
        m_masterDark.rows == result.rows && m_masterDark.cols == result.cols &&
        m_masterDark.channels() == result.channels()) {
        result -= m_masterDark;
    }

    // Divide by flat (vignetting / pixel sensitivity)
    if (!m_masterFlat.empty() &&
        m_masterFlat.rows == result.rows && m_masterFlat.cols == result.cols &&
        m_masterFlat.channels() == result.channels()) {
        int rows = result.rows, cols = result.cols;
        int nc = result.channels();
        for (int y = 0; y < rows; y++) {
            float *rp = result.ptr<float>(y);
            const float *fp = m_masterFlat.ptr<float>(y);
            for (int x = 0; x < cols * nc; x++) {
                if (fp[x] > 0.01f)
                    rp[x] /= fp[x];
            }
        }
    }

    // Clamp to zero (subtraction can go negative)
    int rows = result.rows, cols = result.cols, nc = result.channels();
    for (int y = 0; y < rows; y++) {
        float *p = result.ptr<float>(y);
        for (int x = 0; x < cols * nc; x++)
            if (p[x] < 0) p[x] = 0;
    }

    return result;
}

void StackEngine::dumpGradients(const QString &outDir, const StackParams &params)
{
    QDir dir(outDir);
    if (!dir.exists()) dir.mkpath(".");

    int total = (int)m_frames.size();
    for (int i = 0; i < total; i++) {
        if (m_cancelled) return;
        const FrameInfo &fi = m_frames[i];
        if (!fi.enabled || !fi.aligned) continue;

        // Evaluate gradient surface at frame's original resolution
        int rows = fi.height, cols = fi.width;
        if (rows <= 0 || cols <= 0) {
            // Fall back to reference size
            rows = m_refImage.rows;
            cols = m_refImage.cols;
        }

        int nc = std::min(3, (int)(sizeof(fi.gradientCoeffs)/sizeof(fi.gradientCoeffs[0])));
        std::vector<cv::Mat> gradChans;
        bool hasGrad = false;
        for (int c = 0; c < nc; c++) {
            if (!fi.gradientCoeffs[c].empty()) {
                int deg = (int)std::round((-1 + std::sqrt(1 + 8.0 * fi.gradientCoeffs[c].size())) / 2.0);
                gradChans.push_back(evalGradient(rows, cols, fi.gradientCoeffs[c], deg));
                hasGrad = true;
            } else {
                gradChans.push_back(cv::Mat::zeros(rows, cols, CV_32FC1));
            }
        }
        if (!hasGrad) continue;

        cv::Mat gradient;
        mergeChannels(gradChans, gradient);

        QString fname = QFileInfo(fi.path).completeBaseName();
        QString outPath = dir.filePath(QString("grad_%1.fits").arg(fname));
        saveFits(outPath, gradient);

        emit progress(i + 1, total, QString("Dumping gradient %1/%2").arg(i + 1).arg(total));
    }
}

// ─── Helpers ────────────────────────────────────────────────────────────────

cv::Mat StackEngine::toGray(const cv::Mat &color)
{
    if (color.channels() == 1) return color;
    std::vector<cv::Mat> ch;
    splitChannels(color, ch);
    return 0.2126f * ch[0] + 0.7152f * ch[1] + 0.0722f * ch[2];
}

// ─── Solar Position ─────────────────────────────────────────────────────────

StackEngine::SolarPosition StackEngine::computeSolarPosition(const QString &dateObs,
                                                              double siteLat, double siteLong)
{
    // Parse DATE-OBS: "2026-03-17T20:00:12.451"
    int Y = dateObs.mid(0, 4).toInt();
    int M = dateObs.mid(5, 2).toInt();
    int D = dateObs.mid(8, 2).toInt();
    int h = dateObs.mid(11, 2).toInt();
    int m = dateObs.mid(14, 2).toInt();
    double s = dateObs.mid(17).toDouble();

    double UT = h + m / 60.0 + s / 3600.0;

    // Julian Date
    int a = (14 - M) / 12;
    int y = Y + 4800 - a;
    int mo = M + 12 * a - 3;
    double JD = D + (153 * mo + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045
                + (UT - 12.0) / 24.0;

    // Days from J2000.0
    double n = JD - 2451545.0;

    // Solar mean longitude and mean anomaly (degrees)
    double L = fmod(280.460 + 0.9856474 * n, 360.0);
    if (L < 0) L += 360.0;
    double g = fmod(357.528 + 0.9856003 * n, 360.0);
    if (g < 0) g += 360.0;
    double gRad = g * M_PI / 180.0;

    // Ecliptic longitude
    double lambda = L + 1.915 * sin(gRad) + 0.020 * sin(2.0 * gRad);
    double lambdaRad = lambda * M_PI / 180.0;

    // Obliquity of ecliptic
    double epsilon = 23.439 - 0.0000004 * n;
    double epsilonRad = epsilon * M_PI / 180.0;

    // Solar RA and Dec
    double sinLambda = sin(lambdaRad);
    double cosLambda = cos(lambdaRad);
    double RA = atan2(cos(epsilonRad) * sinLambda, cosLambda) * 180.0 / M_PI;
    if (RA < 0) RA += 360.0;
    double Dec = asin(sin(epsilonRad) * sinLambda) * 180.0 / M_PI;

    // Greenwich Mean Sidereal Time (degrees)
    double GMST = fmod(280.46061837 + 360.98564736629 * n, 360.0);
    if (GMST < 0) GMST += 360.0;

    // Local Sidereal Time
    double LST = GMST + siteLong;

    // Hour Angle
    double HA = LST - RA;

    double latRad = siteLat * M_PI / 180.0;
    double decRad = Dec * M_PI / 180.0;
    double haRad = HA * M_PI / 180.0;

    // Altitude
    double sinAlt = sin(latRad) * sin(decRad) + cos(latRad) * cos(decRad) * cos(haRad);
    double altitude = asin(std::clamp(sinAlt, -1.0, 1.0)) * 180.0 / M_PI;

    // Azimuth (from North through East)
    double cosAlt = cos(altitude * M_PI / 180.0);
    double azimuth = 0;
    if (cosAlt > 1e-10) {
        double sinAz = -cos(decRad) * sin(haRad) / cosAlt;
        double cosAz = (sin(decRad) - sin(latRad) * sinAlt) / (cos(latRad) * cosAlt);
        azimuth = atan2(sinAz, cosAz) * 180.0 / M_PI;
        if (azimuth < 0) azimuth += 360.0;
    }

    return {altitude, azimuth};
}

// ─── Separable Gaussian blur (replaces cv::GaussianBlur) ────────────────────
static void gaussianBlur(const cv::Mat &src, cv::Mat &dst, double sigma)
{
    int krad = (int)std::ceil(sigma * 3.0);
    int ksize = 2 * krad + 1;
    std::vector<float> kernel(ksize);
    double sum = 0;
    for (int i = 0; i < ksize; i++) {
        double d = i - krad;
        kernel[i] = (float)std::exp(-0.5 * d * d / (sigma * sigma));
        sum += kernel[i];
    }
    for (int i = 0; i < ksize; i++) kernel[i] /= (float)sum;

    int rows = src.rows, cols = src.cols;
    cv::Mat tmp(rows, cols, CV_32FC1);
    // Horizontal pass
    for (int y = 0; y < rows; y++) {
        const float *sp = src.ptr<float>(y);
        float *dp = tmp.ptr<float>(y);
        for (int x = 0; x < cols; x++) {
            float v = 0;
            for (int k = -krad; k <= krad; k++) {
                int xx = x + k;
                if (xx < 0) xx = 0; else if (xx >= cols) xx = cols - 1;
                v += sp[xx] * kernel[k + krad];
            }
            dp[x] = v;
        }
    }
    // Vertical pass
    dst = cv::Mat(rows, cols, CV_32FC1);
    for (int y = 0; y < rows; y++) {
        float *dp = dst.ptr<float>(y);
        for (int x = 0; x < cols; x++) {
            float v = 0;
            for (int k = -krad; k <= krad; k++) {
                int yy = y + k;
                if (yy < 0) yy = 0; else if (yy >= rows) yy = rows - 1;
                v += tmp.ptr<float>(yy)[x] * kernel[k + krad];
            }
            dp[x] = v;
        }
    }
}

// ─── Local max (replaces cv::dilate with rect kernel) ───────────────────────
static void localMax(const cv::Mat &src, cv::Mat &dst, int ksize)
{
    int rows = src.rows, cols = src.cols, r = ksize / 2;
    dst = cv::Mat(rows, cols, CV_32FC1);
    for (int y = 0; y < rows; y++) {
        float *dp = dst.ptr<float>(y);
        for (int x = 0; x < cols; x++) {
            float mx = -1e30f;
            for (int dy = -r; dy <= r; dy++) {
                int yy = y + dy;
                if (yy < 0 || yy >= rows) continue;
                const float *sp = src.ptr<float>(yy);
                for (int dx = -r; dx <= r; dx++) {
                    int xx = x + dx;
                    if (xx < 0 || xx >= cols) continue;
                    if (sp[xx] > mx) mx = sp[xx];
                }
            }
            dp[x] = mx;
        }
    }
}

// ─── Bilinear resize (replaces cv::resize with INTER_LINEAR) ────────────────
static void bilinearResize(const cv::Mat &src, cv::Mat &dst, int dstW, int dstH)
{
    dst = cv::Mat(dstH, dstW, src.type());
    int srcH = src.rows, srcW = src.cols;
    double sx = (double)srcW / dstW, sy = (double)srcH / dstH;
    int depth = src.depth();
    for (int y = 0; y < dstH; y++) {
        double fy = (y + 0.5) * sy - 0.5;
        int y0 = (int)std::floor(fy), y1 = y0 + 1;
        double wy = fy - y0;
        if (y0 < 0) y0 = 0; if (y1 >= srcH) y1 = srcH - 1;
        for (int x = 0; x < dstW; x++) {
            double fx = (x + 0.5) * sx - 0.5;
            int x0 = (int)std::floor(fx), x1 = x0 + 1;
            double wx = fx - x0;
            if (x0 < 0) x0 = 0; if (x1 >= srcW) x1 = srcW - 1;
            if (depth == CV_32F) {
                float v = (float)((1-wy)*((1-wx)*src.at<float>(y0,x0) + wx*src.at<float>(y0,x1))
                                 + wy*((1-wx)*src.at<float>(y1,x0) + wx*src.at<float>(y1,x1)));
                dst.at<float>(y,x) = v;
            } else { // CV_64F
                double v = (1-wy)*((1-wx)*src.at<double>(y0,x0) + wx*src.at<double>(y0,x1))
                         + wy*((1-wx)*src.at<double>(y1,x0) + wx*src.at<double>(y1,x1));
                dst.at<double>(y,x) = v;
            }
        }
    }
}

// ─── Star Detection (DoG) ───────────────────────────────────────────────────

std::vector<StarPos> StackEngine::detectStars(const cv::Mat &gray, int maxStars)
{
    cv::Mat blur1, blur2, dog;
    gaussianBlur(gray, blur1, 1.5);
    gaussianBlur(gray, blur2, 4.0);
    dog = blur1 - blur2;

    // Robust threshold: median + 5*MAD (subsampled)
    std::vector<float> vals;
    vals.reserve(dog.rows * dog.cols / 4);
    for (int y = 0; y < dog.rows; y += 2)
        for (int x = 0; x < dog.cols; x += 2)
            vals.push_back(dog.at<float>(y, x));
    std::nth_element(vals.begin(), vals.begin() + vals.size() / 2, vals.end());
    float median = vals[vals.size() / 2];

    std::vector<float> absdev(vals.size());
    for (size_t i = 0; i < vals.size(); i++)
        absdev[i] = std::abs(vals[i] - median);
    std::nth_element(absdev.begin(), absdev.begin() + absdev.size() / 2, absdev.end());
    float mad = absdev[absdev.size() / 2] * 1.4826f;
    float threshold = median + 5.0f * mad;

    // Local maxima
    cv::Mat dilated;
    localMax(dog, dilated, 5);

    std::vector<StarPos> stars;
    int border = 20;
    for (int y = border; y < dog.rows - border; y++) {
        const float *pd = dog.ptr<float>(y);
        const float *pdi = dilated.ptr<float>(y);
        for (int x = border; x < dog.cols - border; x++) {
            if (pd[x] > threshold && pd[x] == pdi[x]) {
                double sx = 0, sy = 0, sw = 0;
                for (int dy = -2; dy <= 2; dy++)
                    for (int dx = -2; dx <= 2; dx++) {
                        float w = dog.at<float>(y + dy, x + dx) - median;
                        if (w > 0) { sx += w * (x + dx); sy += w * (y + dy); sw += w; }
                    }
                if (sw > 0) {
                    StarPos s;
                    s.pos = cv::Point2f((float)(sx / sw), (float)(sy / sw));
                    s.brightness = pd[x];
                    s.fwhm = 0;
                    stars.push_back(s);
                }
            }
        }
    }

    std::sort(stars.begin(), stars.end(),
              [](const StarPos &a, const StarPos &b) { return a.brightness > b.brightness; });
    if ((int)stars.size() > maxStars) stars.resize(maxStars);

    for (auto &s : stars) s.fwhm = measureFWHM(gray, s.pos);
    return stars;
}

double StackEngine::measureFWHM(const cv::Mat &gray, cv::Point2f center)
{
    int cx = cvRound(center.x), cy = cvRound(center.y);
    int r = 8;
    if (cx < r || cy < r || cx >= gray.cols - r || cy >= gray.rows - r) return -1;

    std::vector<float> border;
    border.reserve(4 * (2 * r + 1));
    for (int dx = -r; dx <= r; dx++) {
        border.push_back(gray.at<float>(cy - r, cx + dx));
        border.push_back(gray.at<float>(cy + r, cx + dx));
    }
    for (int dy = -r + 1; dy < r; dy++) {
        border.push_back(gray.at<float>(cy + dy, cx - r));
        border.push_back(gray.at<float>(cy + dy, cx + r));
    }
    std::nth_element(border.begin(), border.begin() + border.size() / 2, border.end());
    float bg = border[border.size() / 2];

    float peak = gray.at<float>(cy, cx);
    float halfMax = bg + (peak - bg) * 0.5f;
    if (peak <= bg) return -1;

    auto findCrossing = [&](int dx, int dy) -> double {
        float prev = peak;
        for (int step = 1; step <= r; step++) {
            float val = gray.at<float>(cy + dy * step, cx + dx * step);
            if (val <= halfMax) {
                float frac = (prev - halfMax) / (prev - val);
                return (step - 1) + frac;
            }
            prev = val;
        }
        return -1;
    };

    int dirs[][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    double hm_sum = 0;
    int hm_count = 0;
    for (auto &d : dirs) {
        double dist = findCrossing(d[0], d[1]);
        if (dist > 0) { hm_sum += dist; hm_count++; }
    }
    if (hm_count < 2) return -1;
    return 2.0 * hm_sum / hm_count;
}

// ─── Star Detection (PCL StarDetector) ─────────────────────────────────────

#include <pcl/StarDetector.h>

std::vector<StarPos> StackEngine::detectStarsPCL(const cv::Mat &gray, int maxStars)
{
    int w = gray.cols, h = gray.rows;

    // Build a pcl::Image (single channel) from the cv::Mat float32
    pcl::Image pclImg;
    pclImg.AllocateData( w, h, 1, pcl::ColorSpace::Gray );
    for ( int y = 0; y < h; y++ )
    {
        const float* row = gray.ptr<float>( y );
        for ( int x = 0; x < w; x++ )
            pclImg( x, y, 0 ) = row[x];
    }

    pcl::StarDetector sd;
    sd.SetStructureLayers( 5 );
    sd.SetHotPixelFilterRadius( 1 );
    sd.SetSensitivity( 0.5F );
    sd.SetPeakResponse( 0.5F );
    sd.SetMaxDistortion( 0.6F );
    sd.EnablePSFFitting( false );
    sd.DisableClusteredSources();

    pcl::ImageVariant iv( &pclImg );
    pcl::StarDetector::star_list pclStars = sd.DetectStars( iv );
    iv.Free();

    // Convert to StarPos
    std::vector<StarPos> stars;
    stars.reserve( pclStars.Length() );
    for ( const auto& s : pclStars )
    {
        StarPos sp;
        sp.pos = cv::Point2f( float( s.pos.x ), float( s.pos.y ) );
        sp.brightness = s.flux;
        sp.fwhm = 0;
        stars.push_back( sp );
    }

    if ( (int)stars.size() > maxStars )
        stars.resize( maxStars );

    for ( auto& s : stars )
        s.fwhm = measureFWHM( gray, s.pos );

    return stars;
}

// ─── PSF Fitting (2D Elliptical Gaussian via Levenberg-Marquardt) ───────────

bool StackEngine::fitPSF(const cv::Mat &gray, cv::Point2f center,
                          float &A, float &x0, float &y0,
                          float &sigmaX, float &sigmaY, float &B,
                          float &residual)
{
    int cx = cvRound(center.x), cy = cvRound(center.y);
    int r = 8;
    if (cx < r || cy < r || cx >= gray.cols - r || cy >= gray.rows - r) return false;

    std::vector<float> border;
    border.reserve(4 * (2 * r + 1));
    for (int dx = -r; dx <= r; dx++) {
        border.push_back(gray.at<float>(cy - r, cx + dx));
        border.push_back(gray.at<float>(cy + r, cx + dx));
    }
    for (int dy = -r + 1; dy < r; dy++) {
        border.push_back(gray.at<float>(cy + dy, cx - r));
        border.push_back(gray.at<float>(cy + dy, cx + r));
    }
    std::nth_element(border.begin(), border.begin() + border.size() / 2, border.end());
    B = border[border.size() / 2];

    float peak = gray.at<float>(cy, cx);
    A = peak - B;
    if (A <= 0) return false;
    if (peak > 0.95f) return false;

    x0 = center.x; y0 = center.y;
    double Mxx = 0, Myy = 0, M00 = 0;
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++) {
            float w = gray.at<float>(cy + dy, cx + dx) - B;
            if (w > 0) { M00 += w; Mxx += w * dx * dx; Myy += w * dy * dy; }
        }
    if (M00 <= 0) return false;
    sigmaX = (float)std::sqrt(Mxx / M00);
    sigmaY = (float)std::sqrt(Myy / M00);
    if (sigmaX < 0.3f) sigmaX = 0.3f;
    if (sigmaY < 0.3f) sigmaY = 0.3f;

    float params[6] = {A, x0, y0, sigmaX, sigmaY, B};
    float lambda = 0.001f;

    struct Pixel { float x, y, val; };
    std::vector<Pixel> pixels;
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx * dx + dy * dy <= r * r)
                pixels.push_back({(float)(cx + dx), (float)(cy + dy),
                                  gray.at<float>(cy + dy, cx + dx)});
    int nPix = (int)pixels.size();
    if (nPix < 10) return false;

    for (int iter = 0; iter < 5; iter++) {
        float pA = params[0], px0 = params[1], py0 = params[2];
        float psx = params[3], psy = params[4], pB = params[5];
        float sx2 = psx * psx, sy2 = psy * psy;
        double JtJ[36] = {}, Jtr[6] = {};
        for (int i = 0; i < nPix; i++) {
            float dx = pixels[i].x - px0, dy = pixels[i].y - py0;
            float g = pA * std::exp(-0.5f * (dx * dx / sx2 + dy * dy / sy2));
            float ri = pixels[i].val - (g + pB);
            float J[6];
            J[0] = g / (pA + 1e-10f);
            J[1] = g * dx / sx2;
            J[2] = g * dy / sy2;
            J[3] = g * dx * dx / (psx * sx2);
            J[4] = g * dy * dy / (psy * sy2);
            J[5] = 1.0f;
            for (int a = 0; a < 6; a++) {
                Jtr[a] += J[a] * ri;
                for (int b = 0; b < 6; b++) JtJ[a * 6 + b] += J[a] * J[b];
            }
        }
        for (int a = 0; a < 6; a++) JtJ[a * 6 + a] *= (1.0 + lambda);
        cv::Mat H(6, 6, CV_64F, JtJ);
        cv::Mat gv(6, 1, CV_64F, Jtr);
        cv::Mat dp;
        if (!solveSVD(H, gv, dp)) break;
        for (int a = 0; a < 6; a++) params[a] += (float)dp.at<double>(a, 0);
        if (params[0] < 0) params[0] = 0;
        if (params[3] < 0.3f) params[3] = 0.3f;
        if (params[4] < 0.3f) params[4] = 0.3f;
        if (std::abs(params[1] - center.x) > r) params[1] = center.x;
        if (std::abs(params[2] - center.y) > r) params[2] = center.y;
    }

    A = params[0]; x0 = params[1]; y0 = params[2];
    sigmaX = params[3]; sigmaY = params[4]; B = params[5];

    double sumR2 = 0;
    float sx2 = sigmaX * sigmaX, sy2 = sigmaY * sigmaY;
    for (int i = 0; i < nPix; i++) {
        float dx = pixels[i].x - x0, dy = pixels[i].y - y0;
        float g = A * std::exp(-0.5f * (dx * dx / sx2 + dy * dy / sy2));
        float ri = pixels[i].val - (g + B);
        sumR2 += ri * ri;
    }
    residual = (float)std::sqrt(sumR2 / nPix);
    if (residual > 0.2f * A) return false;
    return true;
}

//─── Alignment: Geometric Triangle Matching ─────────────────────────────────

static inline float starDist(const StarPos &a, const StarPos &b)
{
    float dx = a.pos.x - b.pos.x, dy = a.pos.y - b.pos.y;
    return std::sqrt(dx * dx + dy * dy);
}

static constexpr double kTriBinSize = 0.01;

static uint64_t triHash(double r1, double r2)
{
    auto b1 = (uint32_t)(r1 / kTriBinSize);
    auto b2 = (uint32_t)(r2 / kTriBinSize);
    return ((uint64_t)b1 << 32) | b2;
}

// Build a triangle from 3 star indices.  Vertices are ordered so that
// v[0] is opposite the longest side, v[1] opposite the middle, v[2] opposite the shortest.
// r1 = middle_side / longest_side,  r2 = shortest_side / longest_side.
static bool makeTri(int a, int b, int c, const std::vector<StarPos> &stars,
                    int outV[3], double &r1, double &r2)
{
    double d[3] = {
        starDist(stars[b], stars[c]),   // opposite a
        starDist(stars[a], stars[c]),   // opposite b
        starDist(stars[a], stars[b])    // opposite c
    };
    int idx[3] = {a, b, c};

    // Sort sides descending, keep track of opposite vertex
    for (int i = 0; i < 2; i++)
        for (int j = i + 1; j < 3; j++)
            if (d[j] > d[i]) { std::swap(d[i], d[j]); std::swap(idx[i], idx[j]); }

    if (d[0] < 20) return false;        // too small — unreliable
    r1 = d[1] / d[0];
    r2 = d[2] / d[0];
    if (r2 < 0.05) return false;         // degenerate (nearly collinear)
    outV[0] = idx[0]; outV[1] = idx[1]; outV[2] = idx[2];
    return true;
}

void StackEngine::buildRefTriangleHash(int maxStars)
{
    m_refTriHash.clear();
    int n = std::min(maxStars, (int)m_refStars.size());
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            for (int k = j + 1; k < n; k++) {
                int v[3]; double r1, r2;
                if (!makeTri(i, j, k, m_refStars, v, r1, r2)) continue;
                TriRef t;
                t.v[0] = v[0]; t.v[1] = v[1]; t.v[2] = v[2];
                t.r1 = r1; t.r2 = r2;
                m_refTriHash[triHash(r1, r2)].push_back(t);
            }
    qDebug() << "Reference triangle hash:" << m_refTriHash.size()
             << "bins from" << n << "stars";
}

// Forward declarations
static cv::Mat computeHomographyDLT(const std::vector<cv::Point2f> &src,
                                     const std::vector<cv::Point2f> &dst,
                                     const std::vector<int> &indices);
static cv::Mat findHomographyRANSAC(const std::vector<cv::Point2f> &src,
                                     const std::vector<cv::Point2f> &dst,
                                     double thresh, cv::Mat &inlierMask,
                                     int maxIters = 2000);

StackEngine::AlignResult StackEngine::matchStars(const std::vector<StarPos> &frameStars)
{
    AlignResult res = {{}, 0};
    int nFrm = std::min(40, (int)frameStars.size());
    if (nFrm < 4 || m_refTriHash.empty()) return res;

    // Vote for correspondences from matching triangles
    std::map<std::pair<int,int>, int> votes;  // (frame_idx, ref_idx) → count

    const double tol = 0.02;  // ratio tolerance

    for (int i = 0; i < nFrm; i++) {
        for (int j = i + 1; j < nFrm; j++) {
            for (int k = j + 1; k < nFrm; k++) {
                int fv[3]; double fr1, fr2;
                if (!makeTri(i, j, k, frameStars, fv, fr1, fr2)) continue;

                // Search 3x3 hash neighborhood
                for (int dr1 = -1; dr1 <= 1; dr1++) {
                    for (int dr2 = -1; dr2 <= 1; dr2++) {
                        uint64_t key = triHash(fr1 + dr1 * kTriBinSize,
                                               fr2 + dr2 * kTriBinSize);
                        auto it = m_refTriHash.find(key);
                        if (it == m_refTriHash.end()) continue;

                        for (auto &rt : it->second) {
                            if (std::abs(fr1 - rt.r1) < tol && std::abs(fr2 - rt.r2) < tol) {
                                votes[{fv[0], rt.v[0]}]++;
                                votes[{fv[1], rt.v[1]}]++;
                                votes[{fv[2], rt.v[2]}]++;
                            }
                        }
                    }
                }
            }
        }
    }

    if (votes.empty()) return res;

    // Sort by vote count, take best unique correspondences
    std::vector<std::pair<int, std::pair<int,int>>> sorted;
    sorted.reserve(votes.size());
    for (auto &[corr, cnt] : votes)
        sorted.push_back({cnt, corr});
    std::sort(sorted.rbegin(), sorted.rend());

    std::vector<cv::Point2f> srcPts, dstPts;
    std::set<int> usedRef, usedFrm;
    for (auto &[cnt, corr] : sorted) {
        if (cnt < 2) break;
        auto [fi, ri] = corr;
        if (usedRef.count(ri) || usedFrm.count(fi)) continue;
        srcPts.push_back(frameStars[fi].pos);
        dstPts.push_back(m_refStars[ri].pos);
        usedRef.insert(ri);
        usedFrm.insert(fi);
    }

    if ((int)srcPts.size() < 4) return res;

    cv::Mat mask;
    cv::Mat H = findHomographyRANSAC(srcPts, dstPts, 3.0, mask);
    if (H.empty()) return res;

    int inlierCount = 0;
    for (int i = 0; i < mask.rows; i++) if (mask.at<uchar>(i)) inlierCount++;
    res.inliers = inlierCount;
    if (res.inliers < 4) return res;
    res.H = H;

    // Store inlier correspondences for TPS fitting
    for (int i = 0; i < (int)srcPts.size(); i++) {
        if (mask.at<uchar>(i)) {
            res.srcPts.push_back(srcPts[i]);
            res.dstPts.push_back(dstPts[i]);
        }
    }
    return res;
}

// ─── Homography estimation (replaces cv::findHomography with RANSAC) ────────
static cv::Mat computeHomographyDLT(const std::vector<cv::Point2f> &src,
                                     const std::vector<cv::Point2f> &dst,
                                     const std::vector<int> &indices)
{
    int N = (int)indices.size();
    double AtA[81] = {};
    for (int i = 0; i < N; i++) {
        double x = src[indices[i]].x, y = src[indices[i]].y;
        double u = dst[indices[i]].x, v = dst[indices[i]].y;
        double r0[9] = {x, y, 1, 0, 0, 0, -u*x, -u*y, -u};
        double r1[9] = {0, 0, 0, x, y, 1, -v*x, -v*y, -v};
        for (int j = 0; j < 9; j++)
            for (int k = 0; k < 9; k++)
                AtA[j*9+k] += r0[j]*r0[k] + r1[j]*r1[k];
    }
    // SVD of 9x9 to find null vector (column of V with smallest singular value)
    int n = 9;
    std::vector<double> Uv(n*n), Wv(n), rv1(n), Vv(n*n);
    for (int i = 0; i < n*n; i++) Uv[i] = AtA[i];
    double g2 = 0, sc = 0, anm = 0;
    for (int i = 0; i < n; i++) {
        int l = i+1; rv1[i] = sc*g2; g2 = sc = 0;
        { double s = 0; for (int k=i;k<n;k++) sc += std::abs(Uv[k*n+i]);
          if (sc != 0) { for (int k=i;k<n;k++) { Uv[k*n+i]/=sc; s+=Uv[k*n+i]*Uv[k*n+i]; }
            double f=Uv[i*n+i]; g2=-std::copysign(std::sqrt(s),f); double h2=f*g2-s; Uv[i*n+i]=f-g2;
            for (int j=l;j<n;j++) { double s2=0; for (int k=i;k<n;k++) s2+=Uv[k*n+i]*Uv[k*n+j]; double f2=s2/h2; for (int k=i;k<n;k++) Uv[k*n+j]+=f2*Uv[k*n+i]; }
            for (int k=i;k<n;k++) Uv[k*n+i]*=sc; } }
        Wv[i] = sc*g2; g2=sc=0;
        if (i<n-1) { for (int k=l;k<n;k++) sc+=std::abs(Uv[i*n+k]);
          if (sc != 0) { double s=0; for (int k=l;k<n;k++) { Uv[i*n+k]/=sc; s+=Uv[i*n+k]*Uv[i*n+k]; }
            double f=Uv[i*n+l]; g2=-std::copysign(std::sqrt(s),f); double h2=f*g2-s; Uv[i*n+l]=f-g2;
            for (int k=l;k<n;k++) rv1[k]=Uv[i*n+k]/h2;
            for (int j=l;j<n;j++) { double s2=0; for (int k=l;k<n;k++) s2+=Uv[j*n+k]*Uv[i*n+k]; for (int k=l;k<n;k++) Uv[j*n+k]+=s2*rv1[k]; }
            for (int k=l;k<n;k++) Uv[i*n+k]*=sc; } }
        anm = std::max(anm, std::abs(Wv[i])+std::abs(rv1[i]));
    }
    for (int i=n-1;i>=0;i--) { int l=i+1;
        if (i<n-1) { if (g2!=0) { for (int j=l;j<n;j++) Vv[j*n+i]=(Uv[i*n+j]/Uv[i*n+l])/g2;
            for (int j=l;j<n;j++) { double s=0; for (int k=l;k<n;k++) s+=Uv[i*n+k]*Vv[k*n+j]; for (int k=l;k<n;k++) Vv[k*n+j]+=s*Vv[k*n+i]; } }
          for (int j=l;j<n;j++) Vv[i*n+j]=Vv[j*n+i]=0; } Vv[i*n+i]=1; g2=rv1[i]; }
    for (int i=n-1;i>=0;i--) { int l=i+1; g2=Wv[i]; for (int j=l;j<n;j++) Uv[i*n+j]=0;
        if (g2!=0) { g2=1.0/g2; for (int j=l;j<n;j++) { double s=0; for (int k=l;k<n;k++) s+=Uv[k*n+i]*Uv[k*n+j]; double f=(s/Uv[i*n+i])*g2; for (int k=i;k<n;k++) Uv[k*n+j]+=f*Uv[k*n+i]; }
          for (int j=i;j<n;j++) Uv[j*n+i]*=g2; } else for (int j=i;j<n;j++) Uv[j*n+i]=0; Uv[i*n+i]+=1; }
    for (int k=n-1;k>=0;k--) { for (int its=0;its<30;its++) { int l; bool flag=true; int nm=k;
        for (l=k;l>=0;l--) { nm=l-1; if (std::abs(rv1[l])+anm==anm){flag=false;break;} if (nm>=0&&std::abs(Wv[nm])+anm==anm) break; }
        if (flag) { double c=0,s=1; for (int i=l;i<=k;i++) { double f=s*rv1[i]; rv1[i]*=c; if (std::abs(f)+anm==anm) break; g2=Wv[i]; double h2=std::hypot(f,g2); Wv[i]=h2; h2=1.0/h2; c=g2*h2; s=-f*h2;
            for (int j=0;j<n;j++) { double y=Uv[j*n+nm],z=Uv[j*n+i]; Uv[j*n+nm]=y*c+z*s; Uv[j*n+i]=z*c-y*s; } } }
        double z=Wv[k]; if (l==k) { if (z<0){Wv[k]=-z; for (int j=0;j<n;j++) Vv[j*n+k]=-Vv[j*n+k];} break; }
        if (its==29) break;
        double xx=Wv[l],y2=Wv[k-1]; g2=rv1[k-1]; double h2=rv1[k]; double f=((y2-z)*(y2+z)+(g2-h2)*(g2+h2))/(2*h2*y2); g2=std::hypot(f,1.0);
        f=((xx-z)*(xx+z)+h2*(y2/(f+std::copysign(g2,f))-h2))/xx; double c=1,s=1;
        for (int j=l;j<k;j++) { int i=j+1; g2=rv1[i]; y2=Wv[i]; h2=s*g2; g2=c*g2; z=std::hypot(f,h2); rv1[j]=z; c=f/z; s=h2/z; f=xx*c+g2*s; g2=g2*c-xx*s; h2=y2*s; y2*=c;
            for (int jj=0;jj<n;jj++) { xx=Vv[jj*n+j]; z=Vv[jj*n+i]; Vv[jj*n+j]=xx*c+z*s; Vv[jj*n+i]=z*c-xx*s; }
            z=std::hypot(f,h2); Wv[j]=z; if (z!=0){z=1.0/z; c=f*z; s=h2*z;} f=c*g2+s*y2; xx=c*y2-s*g2;
            for (int jj=0;jj<n;jj++) { y2=Uv[jj*n+j]; z=Uv[jj*n+i]; Uv[jj*n+j]=y2*c+z*s; Uv[jj*n+i]=z*c-y2*s; } }
        rv1[l]=0; rv1[k]=f; Wv[k]=xx; } }
    int minIdx = 0;
    for (int i = 1; i < n; i++) if (std::abs(Wv[i]) < std::abs(Wv[minIdx])) minIdx = i;
    cv::Mat H(3, 3, CV_64F);
    for (int i = 0; i < 9; i++) H.at<double>(i/3, i%3) = Vv[i*n+minIdx];
    double hv = H.at<double>(2,2);
    if (std::abs(hv) > 1e-12) H /= hv;
    return H;
}

static cv::Mat findHomographyRANSAC(const std::vector<cv::Point2f> &src,
                                     const std::vector<cv::Point2f> &dst,
                                     double thresh, cv::Mat &inlierMask,
                                     int maxIters)
{
    int N = (int)src.size();
    inlierMask = cv::Mat::zeros(N, 1, CV_8U);
    if (N < 4) return cv::Mat();

    double bestScore = 0;
    cv::Mat bestH;
    std::vector<uchar> bestInliers(N, 0);
    double thresh2 = thresh * thresh;

    std::mt19937 rng(42);
    for (int iter = 0; iter < maxIters; iter++) {
        std::vector<int> idx(4);
        for (int i = 0; i < 4; i++) {
            bool ok;
            do { ok = true; idx[i] = rng() % N;
                for (int j = 0; j < i; j++) if (idx[j] == idx[i]) { ok = false; break; }
            } while (!ok);
        }
        cv::Mat H = computeHomographyDLT(src, dst, idx);
        if (H.empty()) continue;

        double h00=H.at<double>(0,0), h01=H.at<double>(0,1), h02=H.at<double>(0,2);
        double h10=H.at<double>(1,0), h11=H.at<double>(1,1), h12=H.at<double>(1,2);
        double h20=H.at<double>(2,0), h21=H.at<double>(2,1), h22=H.at<double>(2,2);
        int nInliers = 0;
        std::vector<uchar> curInliers(N, 0);
        for (int i = 0; i < N; i++) {
            double x = src[i].x, y = src[i].y;
            double w = h20*x + h21*y + h22;
            if (std::abs(w) < 1e-12) continue;
            double px = (h00*x + h01*y + h02) / w;
            double py = (h10*x + h11*y + h12) / w;
            double dx = px - dst[i].x, dy = py - dst[i].y;
            if (dx*dx + dy*dy < thresh2) { curInliers[i] = 1; nInliers++; }
        }
        if (nInliers > bestScore) {
            bestScore = nInliers;
            bestH = H;
            bestInliers = curInliers;
            if (nInliers > N * 0.995) break;
        }
    }

    if (bestScore < 4) return cv::Mat();
    std::vector<int> inlierIdx;
    for (int i = 0; i < N; i++) if (bestInliers[i]) inlierIdx.push_back(i);
    bestH = computeHomographyDLT(src, dst, inlierIdx);
    for (int i = 0; i < N; i++) inlierMask.at<uchar>(i) = bestInliers[i];
    return bestH;
}

// ─── TPS Distortion Correction ─────────────────────────────────────��───────

static inline double tpsKernel(double r2)
{
    return (r2 > 0) ? r2 * std::log(r2) : 0;
}

FrameInfo::TPSCoeffs StackEngine::fitTPS(const std::vector<cv::Point2f> &srcPts,
                                          const std::vector<cv::Point2f> &dstPts)
{
    FrameInfo::TPSCoeffs result;
    int N = (int)srcPts.size();
    if (N < 6) return result;

    int M = N + 3;
    cv::Mat L = cv::Mat::zeros(M, M, CV_64F);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            if (i == j) { L.at<double>(i, j) = 1e-6; continue; }
            double dx = srcPts[i].x - srcPts[j].x;
            double dy = srcPts[i].y - srcPts[j].y;
            L.at<double>(i, j) = tpsKernel(dx * dx + dy * dy);
        }

    for (int i = 0; i < N; i++) {
        L.at<double>(i, N) = 1;   L.at<double>(N, i) = 1;
        L.at<double>(i, N+1) = srcPts[i].x; L.at<double>(N+1, i) = srcPts[i].x;
        L.at<double>(i, N+2) = srcPts[i].y; L.at<double>(N+2, i) = srcPts[i].y;
    }

    cv::Mat vx(M, 1, CV_64F, cv::Scalar(0));
    for (int i = 0; i < N; i++) vx.at<double>(i, 0) = dstPts[i].x;
    cv::Mat solX;
    if (!solveSVD(L, vx, solX)) return result;

    cv::Mat vy(M, 1, CV_64F, cv::Scalar(0));
    for (int i = 0; i < N; i++) vy.at<double>(i, 0) = dstPts[i].y;
    cv::Mat solY;
    if (!solveSVD(L, vy, solY)) return result;

    result.ctrlPts.assign(srcPts.begin(), srcPts.end());
    result.wx.resize(N); result.wy.resize(N);
    for (int i = 0; i < N; i++) { result.wx[i] = solX.at<double>(i,0); result.wy[i] = solY.at<double>(i,0); }
    result.ax[0] = solX.at<double>(N,0); result.ax[1] = solX.at<double>(N+1,0); result.ax[2] = solX.at<double>(N+2,0);
    result.ay[0] = solY.at<double>(N,0); result.ay[1] = solY.at<double>(N+1,0); result.ay[2] = solY.at<double>(N+2,0);
    result.valid = true;
    return result;
}

void StackEngine::generateTPSRemapMaps(const FrameInfo::TPSCoeffs &tps, int rows, int cols,
                                         cv::Mat &mapX, cv::Mat &mapY)
{
    int N = (int)tps.ctrlPts.size();
    int sRows = (rows + 3) / 4, sCols = (cols + 3) / 4;
    cv::Mat smX(sRows, sCols, CV_32FC1), smY(sRows, sCols, CV_32FC1);

    parallelFor(0, sRows, [&](int lo, int hi) {
        for (int sy = lo; sy < hi; sy++) {
            float *px = smX.ptr<float>(sy);
            float *py = smY.ptr<float>(sy);
            double y = sy * 4.0;
            for (int sx = 0; sx < sCols; sx++) {
                double x = sx * 4.0;
                double fx = tps.ax[0] + tps.ax[1] * x + tps.ax[2] * y;
                double fy = tps.ay[0] + tps.ay[1] * x + tps.ay[2] * y;
                for (int i = 0; i < N; i++) {
                    double dx = x - tps.ctrlPts[i].x;
                    double dy2 = y - tps.ctrlPts[i].y;
                    double r2 = dx * dx + dy2 * dy2;
                    fx += tps.wx[i] * tpsKernel(r2);
                    fy += tps.wy[i] * tpsKernel(r2);
                }
                px[sx] = (float)fx;
                py[sx] = (float)fy;
            }
        }
    });

    bilinearResize(smX, mapX, cols, rows);
    bilinearResize(smY, mapY, cols, rows);
}

// ─── Background Gradient ────────────────────────────────────────────────────

static int polyTerms(int degree) { return (degree + 1) * (degree + 2) / 2; }

static void polyRow(double xn, double yn, int degree, double *row)
{
    int col = 0;
    for (int p = 0; p <= degree; p++)
        for (int q = 0; q <= degree - p; q++)
            row[col++] = std::pow(xn, p) * std::pow(yn, q);
}

cv::Mat StackEngine::fitGradient(const cv::Mat &channel, const std::vector<StarPos> &stars,
                                  int degree, std::vector<double> &coeffs)
{
    if (degree < 1) {
        coeffs.clear();
        return cv::Mat::zeros(channel.size(), CV_32FC1);
    }

    int nterms = polyTerms(degree);
    int h = channel.rows, w = channel.cols;

    cv::Mat mask = cv::Mat::ones(h, w, CV_8U) * 255;
    for (auto &s : stars) {
        int r = std::max(10, (int)(s.fwhm * 3));
        cv::circle(mask, s.pos, r, cv::Scalar(0), -1);
    }

    int step = 32;
    int patchR = 3;  // 7x7 patch median
    std::vector<cv::Point2i> samples;
    std::vector<double> values;
    for (int y = step / 2; y < h; y += step)
        for (int x = step / 2; x < w; x += step)
            if (mask.at<uchar>(y, x) > 0) {
                // Patch median for robustness against hot pixels and faint stars
                std::vector<float> patch;
                for (int dy = -patchR; dy <= patchR; dy++)
                    for (int dx = -patchR; dx <= patchR; dx++) {
                        int py = y + dy, px = x + dx;
                        if (py >= 0 && py < h && px >= 0 && px < w && mask.at<uchar>(py, px) > 0)
                            patch.push_back(channel.at<float>(py, px));
                    }
                if (patch.size() < 5) continue;
                std::nth_element(patch.begin(), patch.begin() + patch.size() / 2, patch.end());
                samples.push_back({x, y});
                values.push_back(patch[patch.size() / 2]);
            }

    if ((int)samples.size() < nterms * 3) {
        if (degree > 1) return fitGradient(channel, stars, 1, coeffs);
        coeffs.assign(nterms, 0);
        return cv::Mat::zeros(channel.size(), CV_32FC1);
    }

    std::vector<bool> inlier(samples.size(), true);
    for (int iter = 0; iter < 2; iter++) {
        int n = 0;
        for (size_t i = 0; i < samples.size(); i++) if (inlier[i]) n++;

        cv::Mat A(n, nterms, CV_64F);
        cv::Mat b(n, 1, CV_64F);
        int row = 0;
        for (size_t i = 0; i < samples.size(); i++) {
            if (!inlier[i]) continue;
            polyRow((double)samples[i].x / w, (double)samples[i].y / h, degree, A.ptr<double>(row));
            b.at<double>(row, 0) = values[i];
            row++;
        }

        cv::Mat c;
        solveSVD(A, b, c);
        coeffs.resize(nterms);
        for (int i = 0; i < nterms; i++) coeffs[i] = c.at<double>(i, 0);

        if (iter < 1) {
            std::vector<double> residuals(samples.size());
            for (size_t i = 0; i < samples.size(); i++) {
                double rv[20];
                polyRow((double)samples[i].x / w, (double)samples[i].y / h, degree, rv);
                double pred = 0;
                for (int j = 0; j < nterms; j++) pred += coeffs[j] * rv[j];
                residuals[i] = std::abs(values[i] - pred);
            }
            auto sr = residuals;
            std::sort(sr.begin(), sr.end());
            double med = sr[sr.size() / 2];
            std::vector<double> ad(sr.size());
            for (size_t i = 0; i < sr.size(); i++) ad[i] = std::abs(sr[i] - med);
            std::sort(ad.begin(), ad.end());
            double sig = ad[ad.size() / 2] * 1.4826;
            for (size_t i = 0; i < samples.size(); i++)
                inlier[i] = residuals[i] < 3.0 * sig + 1e-6;
        }
    }
    return evalGradient(h, w, coeffs, degree);
}

cv::Mat StackEngine::fitGradient(const cv::Mat &channel, const std::vector<StarPos> &stars,
                                  int degree, std::vector<double> &coeffs,
                                  const cv::Mat &signalMask)
{
    if (degree < 1) {
        coeffs.clear();
        return cv::Mat::zeros(channel.size(), CV_32FC1);
    }

    int nterms = polyTerms(degree);
    int h = channel.rows, w = channel.cols;

    // Build mask: start with all valid, then mask out stars and signal regions
    cv::Mat mask = cv::Mat::ones(h, w, CV_8U) * 255;
    for (auto &s : stars) {
        int r = std::max(10, (int)(s.fwhm * 3));
        cv::circle(mask, s.pos, r, cv::Scalar(0), -1);
    }
    if (!signalMask.empty() && signalMask.size() == mask.size())
        cv::bitwise_and(mask, signalMask, mask);

    int step = 32;
    int patchR = 3;
    std::vector<cv::Point2i> samples;
    std::vector<double> values;
    for (int y = step / 2; y < h; y += step)
        for (int x = step / 2; x < w; x += step)
            if (mask.at<uchar>(y, x) > 0) {
                std::vector<float> patch;
                for (int dy = -patchR; dy <= patchR; dy++)
                    for (int dx = -patchR; dx <= patchR; dx++) {
                        int py = y + dy, px = x + dx;
                        if (py >= 0 && py < h && px >= 0 && px < w && mask.at<uchar>(py, px) > 0)
                            patch.push_back(channel.at<float>(py, px));
                    }
                if (patch.size() < 5) continue;
                std::nth_element(patch.begin(), patch.begin() + patch.size() / 2, patch.end());
                samples.push_back({x, y});
                values.push_back(patch[patch.size() / 2]);
            }

    if ((int)samples.size() < nterms * 3) {
        if (degree > 1) return fitGradient(channel, stars, 1, coeffs, signalMask);
        coeffs.assign(nterms, 0);
        return cv::Mat::zeros(channel.size(), CV_32FC1);
    }

    std::vector<bool> inlier(samples.size(), true);
    for (int iter = 0; iter < 2; iter++) {
        int n = 0;
        for (size_t i = 0; i < samples.size(); i++) if (inlier[i]) n++;

        cv::Mat A(n, nterms, CV_64F);
        cv::Mat b(n, 1, CV_64F);
        int row = 0;
        for (size_t i = 0; i < samples.size(); i++) {
            if (!inlier[i]) continue;
            polyRow((double)samples[i].x / w, (double)samples[i].y / h, degree, A.ptr<double>(row));
            b.at<double>(row, 0) = values[i];
            row++;
        }

        cv::Mat c;
        solveSVD(A, b, c);
        coeffs.resize(nterms);
        for (int i = 0; i < nterms; i++) coeffs[i] = c.at<double>(i, 0);

        if (iter < 1) {
            std::vector<double> residuals(samples.size());
            for (size_t i = 0; i < samples.size(); i++) {
                double rv[20];
                polyRow((double)samples[i].x / w, (double)samples[i].y / h, degree, rv);
                double pred = 0;
                for (int j = 0; j < nterms; j++) pred += coeffs[j] * rv[j];
                residuals[i] = std::abs(values[i] - pred);
            }
            auto sr = residuals;
            std::sort(sr.begin(), sr.end());
            double med = sr[sr.size() / 2];
            std::vector<double> ad(sr.size());
            for (size_t i = 0; i < sr.size(); i++) ad[i] = std::abs(sr[i] - med);
            std::sort(ad.begin(), ad.end());
            double sig = ad[ad.size() / 2] * 1.4826;
            for (size_t i = 0; i < samples.size(); i++)
                inlier[i] = residuals[i] < 3.0 * sig + 1e-6;
        }
    }
    return evalGradient(h, w, coeffs, degree);
}

cv::Mat StackEngine::evalGradient(int rows, int cols, const std::vector<double> &coeffs, int degree)
{
    int nterms = polyTerms(degree);
    if ((int)coeffs.size() < nterms) return cv::Mat::zeros(rows, cols, CV_32FC1);

    cv::Mat grad(rows, cols, CV_32FC1);
    grad.forEach<float>([&](float &v, const int *pos) {
        double yn = (double)pos[0] / rows, xn = (double)pos[1] / cols;
        double rv[20];
        polyRow(xn, yn, degree, rv);
        double val = 0;
        for (int j = 0; j < nterms; j++) val += coeffs[j] * rv[j];
        v = (float)val;
    });
    return grad;
}

// ─── Rayleigh Scattering Sky Model ──────────────────────────────────────────

// Rayleigh λ^-4 factors normalized to R=1.0 (R~620nm, G~530nm, B~470nm)
static constexpr double kRayleigh[3] = {1.0, 1.875, 3.025};

void StackEngine::fitRayleighGradient(const std::vector<cv::Mat> &channels,
                                       const std::vector<StarPos> &stars,
                                       int greyDegree, double sunDirX, double sunDirY,
                                       std::vector<double> coeffsOut[3])
{
    // Model: pixel(x,y,c) = A * k_c * sunGrad(x,y) + grey_poly(x,y)
    // sunGrad(x,y) = sunDirX*(xn-0.5) + sunDirY*(yn-0.5)
    // Fit all 3 channels simultaneously; grey poly is shared (achromatic)

    int nc = std::min((int)channels.size(), 3);
    int h = channels[0].rows, w = channels[0].cols;
    int nGreyTerms = (greyDegree >= 1) ? polyTerms(greyDegree) : polyTerms(1);
    int actualGreyDeg = std::max(1, greyDegree);
    int nParams = 1 + nGreyTerms;  // A + grey polynomial

    // Star mask
    cv::Mat mask = cv::Mat::ones(h, w, CV_8U) * 255;
    for (auto &s : stars) {
        int r = std::max(10, (int)(s.fwhm * 3));
        cv::circle(mask, s.pos, r, cv::Scalar(0), -1);
    }

    // Sample background pixels with patch median
    int step = 32;
    int patchR = 3;  // 7x7 patch
    std::vector<cv::Point2i> samples;
    std::vector<std::array<double, 3>> sampleVals;  // per-channel patch medians
    for (int y = step / 2; y < h; y += step)
        for (int x = step / 2; x < w; x += step)
            if (mask.at<uchar>(y, x) > 0) {
                std::array<double, 3> vals = {};
                bool ok = true;
                for (int c = 0; c < nc; c++) {
                    std::vector<float> patch;
                    for (int dy = -patchR; dy <= patchR; dy++)
                        for (int dx = -patchR; dx <= patchR; dx++) {
                            int py = y + dy, px = x + dx;
                            if (py >= 0 && py < h && px >= 0 && px < w && mask.at<uchar>(py, px) > 0)
                                patch.push_back(channels[c].at<float>(py, px));
                        }
                    if (patch.size() < 5) { ok = false; break; }
                    std::nth_element(patch.begin(), patch.begin() + patch.size() / 2, patch.end());
                    vals[c] = patch[patch.size() / 2];
                }
                if (!ok) continue;
                samples.push_back({x, y});
                sampleVals.push_back(vals);
            }

    if ((int)samples.size() < nParams * 3) {
        // Fallback to independent polynomial
        for (int c = 0; c < nc; c++)
            fitGradient(channels[c], stars, greyDegree, coeffsOut[c]);
        return;
    }

    // Iterative fitting with sigma-clipping
    int nSamples = samples.size();
    std::vector<bool> inlier(nSamples, true);

    cv::Mat solution;
    for (int iter = 0; iter < 2; iter++) {
        int nActive = 0;
        for (int i = 0; i < nSamples; i++) if (inlier[i]) nActive++;

        cv::Mat A(nActive * nc, nParams, CV_64F);
        cv::Mat b(nActive * nc, 1, CV_64F);
        int row = 0;
        for (int i = 0; i < nSamples; i++) {
            if (!inlier[i]) continue;
            double xn = (double)samples[i].x / w;
            double yn = (double)samples[i].y / h;
            double sunGrad = sunDirX * (xn - 0.5) + sunDirY * (yn - 0.5);
            double polyVals[20];
            polyRow(xn, yn, actualGreyDeg, polyVals);

            for (int c = 0; c < nc; c++) {
                A.at<double>(row, 0) = kRayleigh[c] * sunGrad;
                for (int j = 0; j < nGreyTerms; j++)
                    A.at<double>(row, 1 + j) = polyVals[j];
                b.at<double>(row, 0) = sampleVals[i][c];
                row++;
            }
        }

        solveSVD(A, b, solution);

        // Sigma-clip on first iteration
        if (iter < 1) {
            std::vector<double> residuals(nSamples * nc);
            for (int i = 0; i < nSamples; i++) {
                double xn = (double)samples[i].x / w;
                double yn = (double)samples[i].y / h;
                double sunGrad = sunDirX * (xn - 0.5) + sunDirY * (yn - 0.5);
                double polyVals[20];
                polyRow(xn, yn, actualGreyDeg, polyVals);

                double maxRes = 0;
                for (int c = 0; c < nc; c++) {
                    double pred = solution.at<double>(0, 0) * kRayleigh[c] * sunGrad;
                    for (int j = 0; j < nGreyTerms; j++)
                        pred += solution.at<double>(1 + j, 0) * polyVals[j];
                    double obs = sampleVals[i][c];
                    maxRes = std::max(maxRes, std::abs(obs - pred));
                }
                residuals[i] = maxRes;
            }
            auto sr = residuals;
            sr.resize(nSamples);
            std::sort(sr.begin(), sr.end());
            double med = sr[nSamples / 2];
            std::vector<double> ad(nSamples);
            for (int i = 0; i < nSamples; i++) ad[i] = std::abs(residuals[i] - med);
            std::sort(ad.begin(), ad.end());
            double sig = ad[nSamples / 2] * 1.4826;
            for (int i = 0; i < nSamples; i++)
                inlier[i] = residuals[i] < 3.0 * sig + 1e-6;
        }
    }

    // Expand Rayleigh + grey model into per-channel polynomial coefficients
    // so that the existing evalGradient/prepareFrame pipeline works unchanged.
    //
    // polyRow order for degree d:
    //   p=0: q=0..d  -> 1, yn, yn^2, ...
    //   p=1: q=0..d-1 -> xn, xn*yn, ...
    //   p=2: q=0..d-2 -> xn^2, ...
    // So: index 0 = constant, index 1 = yn, index (d+1) = xn (for degree >= 1)

    double rayleighA = solution.at<double>(0, 0);
    int xnIdx = actualGreyDeg + 1;  // index of xn term in polyRow

    for (int c = 0; c < nc; c++) {
        coeffsOut[c].resize(nGreyTerms);
        for (int j = 0; j < nGreyTerms; j++)
            coeffsOut[c][j] = solution.at<double>(1 + j, 0);

        // Add Rayleigh linear gradient: A * k_c * (sunDirX*(xn-0.5) + sunDirY*(yn-0.5))
        double Akc = rayleighA * kRayleigh[c];
        coeffsOut[c][0]     += Akc * (-0.5 * (sunDirX + sunDirY));  // constant
        coeffsOut[c][1]     += Akc * sunDirY;                        // yn term
        coeffsOut[c][xnIdx] += Akc * sunDirX;                        // xn term
    }

    qDebug() << "Rayleigh model: A=" << rayleighA
             << " sunDir=(" << sunDirX << "," << sunDirY << ")"
             << " R/G/B scale:" << rayleighA * kRayleigh[0]
             << rayleighA * kRayleigh[1] << rayleighA * kRayleigh[2];
}

// ─── Normalization ──────────────────────────────────────────────────────────

static double robustMedian(const cv::Mat &m)
{
    std::vector<float> v;
    v.reserve(m.rows * m.cols / 4);
    for (int y = 0; y < m.rows; y += 2) {
        const float *p = m.ptr<float>(y);
        for (int x = 0; x < m.cols; x += 2) {
            float val = p[x];
            if (std::isfinite(val)) v.push_back(val);
        }
    }
    if (v.empty()) return 0;
    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    return v[v.size() / 2];
}

static double robustMAD(const cv::Mat &m, double median)
{
    std::vector<float> dev;
    dev.reserve(m.rows * m.cols / 4);
    for (int y = 0; y < m.rows; y += 2) {
        const float *p = m.ptr<float>(y);
        for (int x = 0; x < m.cols; x += 2) {
            float val = p[x];
            if (std::isfinite(val)) dev.push_back(std::abs(val - (float)median));
        }
    }
    if (dev.empty()) return 1;
    std::nth_element(dev.begin(), dev.begin() + dev.size() / 2, dev.end());
    return dev[dev.size() / 2] * 1.4826;
}

void StackEngine::computeNormalization(const cv::Mat &refChan, const cv::Mat &frameChan,
                                        const cv::Mat &refGrad, const cv::Mat &frameGrad,
                                        double &scale, double &offset)
{
    cv::Mat refFlat = refChan - refGrad;
    cv::Mat frameFlat = frameChan - frameGrad;

    double refMed = robustMedian(refFlat);
    double refSig = robustMAD(refFlat, refMed);
    double frmMed = robustMedian(frameFlat);
    double frmSig = robustMAD(frameFlat, frmMed);

    scale = (frmSig > 1e-6) ? refSig / frmSig : 1.0;
    offset = refMed - scale * frmMed;
}

// Map FITS BAYERPAT string to OpenCV demosaic code, or -1 if not recognised.
static int bayerCode(const QString &pat)
{
    if (pat == "RGGB") return cv::COLOR_BayerRG2RGB;
    if (pat == "BGGR") return cv::COLOR_BayerBG2RGB;
    if (pat == "GRBG") return cv::COLOR_BayerGR2RGB;
    if (pat == "GBRG") return cv::COLOR_BayerGB2RGB;
    return -1;
}

// Demosaic a single-channel float32 Bayer frame to 3-channel float32 RGB.
static cv::Mat demosaicFloat(const cv::Mat &raw, int code)
{
    cv::Mat u16;
    raw.convertTo(u16, CV_16UC1, 65535.0);
    cv::Mat rgb16;
    cv::cvtColor(u16, rgb16, code);
    cv::Mat rgbf;
    rgb16.convertTo(rgbf, CV_32FC3, 1.0 / 65535.0);
    return rgbf;
}

// ─── Frame Preparation ─────────────────────────────────────────────────────

cv::Mat StackEngine::prepareFrame(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask)
{
    int h = m_refImage.rows, w = m_refImage.cols;

    // Demosaic Bayer frames before warping so interpolation doesn't mix colours
    cv::Mat input = raw;
    if (m_debayer && raw.channels() == 1 && !fi.bayerPattern.isEmpty()) {
        int code = bayerCode(fi.bayerPattern);
        if (code >= 0)
            input = demosaicFloat(raw, code);
    }

    cv::Mat warped;
    cv::Mat ones = cv::Mat::ones(input.size(), CV_8UC1);

    if (fi.tpsCoeffs.valid) {
        cv::Mat mapX, mapY;
        generateTPSRemapMaps(fi.tpsCoeffs, h, w, mapX, mapY);
        cv::remap(input, warped, mapX, mapY, cv::INTER_LINEAR,
                  cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        cv::remap(ones, mask, mapX, mapY, cv::INTER_NEAREST,
                  cv::BORDER_CONSTANT, cv::Scalar(0));
    } else {
        cv::warpPerspective(input, warped, fi.homography, cv::Size(w, h),
                            cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
        cv::warpPerspective(ones, mask, fi.homography, cv::Size(w, h),
                            cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));
    }

    std::vector<cv::Mat> channels;
    splitChannels(warped, channels);

    int nc = std::min((int)channels.size(), 3);
    for (int c = 0; c < nc; c++) {
        cv::Mat grad;
        if (!fi.gradientCoeffs[c].empty()) {
            int deg = (int)std::round((-1 + std::sqrt(1 + 8.0 * fi.gradientCoeffs[c].size())) / 2.0);
            cv::Mat origGrad = evalGradient(raw.rows, raw.cols, fi.gradientCoeffs[c], deg);
            if (fi.tpsCoeffs.valid) {
                cv::Mat mapX, mapY;
                generateTPSRemapMaps(fi.tpsCoeffs, h, w, mapX, mapY);
                cv::remap(origGrad, grad, mapX, mapY, cv::INTER_LINEAR,
                          cv::BORDER_CONSTANT, cv::Scalar(0));
            } else {
                cv::warpPerspective(origGrad, grad, fi.homography, cv::Size(w, h),
                                    cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
            }
        } else {
            grad = cv::Mat::zeros(h, w, CV_32FC1);
        }

        if (fi.localGridSize > 0 && !fi.localMedian[c].empty()) {
            int gs = fi.localGridSize;
            const FrameInfo &ref = m_frames[m_refIndex];
            cv::Mat scaleGrid(gs, gs, CV_64FC1), offsetGrid(gs, gs, CV_64FC1);
            for (int gy = 0; gy < gs; gy++)
                for (int gx = 0; gx < gs; gx++) {
                    int idx = gy * gs + gx;
                    double refSig = ref.localMAD[c][idx];
                    double frmSig = fi.localMAD[c][idx];
                    double s = (frmSig > 1e-6) ? refSig / frmSig : 1.0;
                    double refMed = ref.localMedian[c][idx];
                    double frmMed = fi.localMedian[c][idx];
                    scaleGrid.at<double>(gy, gx) = s;
                    offsetGrid.at<double>(gy, gx) = refMed - s * frmMed;
                }
            cv::Mat scaleFull, offsetFull;
            bilinearResize(scaleGrid, scaleFull, w, h);
            bilinearResize(offsetGrid, offsetFull, w, h);
            scaleFull.convertTo(scaleFull, CV_32FC1);
            offsetFull.convertTo(offsetFull, CV_32FC1);
            channels[c] = scaleFull.mul(channels[c] - grad) + offsetFull;
        } else {
            channels[c] = fi.scale[c] * (channels[c] - grad) + fi.offset[c];
        }
    }

    cv::Mat result;
    mergeChannels(channels, result);
    return result;
}

// ─── Analysis ───────────────────────────────────────────────────────────────

void StackEngine::preselectFrames(const QStringList &paths, const StackParams &params)
{
    m_cancelled = false;
    m_frames.clear();
    m_frames.resize(paths.size());
    int total = paths.size();

    emit progress(0, total, "Pre-scan: reading FITS headers...");
    int preFiltered = 0;
    for (int i = 0; i < total; i++) {
        if (m_cancelled) return;
        FrameInfo &fi = m_frames[i];
        fi.path = paths[i];
        fi.enabled = true;

        fitsfile *fptr = nullptr;
        int status = 0;
        fits_open_file(&fptr, fi.path.toStdString().c_str(), READONLY, &status);
        if (!status) {
            char dateobs[80] = {};
            fits_read_key(fptr, TSTRING, "DATE-OBS", dateobs, nullptr, &status);
            if (!status) fi.dateObs = dateobs;
            status = 0;
            fits_read_key(fptr, TDOUBLE, "EXPTIME", &fi.exptime, nullptr, &status);
            status = 0;
            double siteLat = 0, siteLong = 0;
            fits_read_key(fptr, TDOUBLE, "SITELAT", &siteLat, nullptr, &status);
            status = 0;
            fits_read_key(fptr, TDOUBLE, "SITELONG", &siteLong, nullptr, &status);
            status = 0;
            fits_read_key(fptr, TDOUBLE, "OBJCTAZ", &fi.objAzimuth, nullptr, &status);
            status = 0;
            fits_read_key(fptr, TDOUBLE, "OBJCTALT", &fi.objAltitude, nullptr, &status);
            status = 0;
            char bayerpat[80] = {};
            fits_read_key(fptr, TSTRING, "BAYERPAT", bayerpat, nullptr, &status);
            if (!status) fi.bayerPattern = QString(bayerpat).trimmed();
            status = 0;

            // Read WCS if present
            {
                SimpleTANWCS &w = fi.wcs;
                char ctype1[80] = {};
                fits_read_key(fptr, TSTRING, "CTYPE1", ctype1, nullptr, &status);
                if (!status && QString(ctype1).contains("TAN")) {
                    status = 0;
                    fits_read_key(fptr, TDOUBLE, "CRVAL1", &w.crval1, nullptr, &status);
                    fits_read_key(fptr, TDOUBLE, "CRVAL2", &w.crval2, nullptr, &status);
                    fits_read_key(fptr, TDOUBLE, "CRPIX1", &w.crpix1, nullptr, &status);
                    fits_read_key(fptr, TDOUBLE, "CRPIX2", &w.crpix2, nullptr, &status);
                    if (!status) {
                        int cdst = 0;
                        fits_read_key(fptr, TDOUBLE, "CD1_1", &w.cd11, nullptr, &cdst);
                        if (!cdst) {
                            fits_read_key(fptr, TDOUBLE, "CD1_2", &w.cd12, nullptr, &cdst);
                            fits_read_key(fptr, TDOUBLE, "CD2_1", &w.cd21, nullptr, &cdst);
                            fits_read_key(fptr, TDOUBLE, "CD2_2", &w.cd22, nullptr, &cdst);
                        } else {
                            // Fallback: CDELT + CROTA
                            double cdelt1 = 0, cdelt2 = 0, crota = 0;
                            cdst = 0;
                            fits_read_key(fptr, TDOUBLE, "CDELT1", &cdelt1, nullptr, &cdst);
                            fits_read_key(fptr, TDOUBLE, "CDELT2", &cdelt2, nullptr, &cdst);
                            int rot_st = 0;
                            fits_read_key(fptr, TDOUBLE, "CROTA2", &crota, nullptr, &rot_st);
                            double rad = crota * M_PI / 180.0;
                            w.cd11 = cdelt1 * cos(rad);
                            w.cd12 = -cdelt2 * sin(rad);
                            w.cd21 = cdelt1 * sin(rad);
                            w.cd22 = cdelt2 * cos(rad);
                        }
                        w.valid = !cdst;
                    }
                }
                status = 0;
            }

            fits_close_file(fptr, &status);

            if (!fi.dateObs.isEmpty() && (siteLat != 0 || siteLong != 0)) {
                auto sunPos = computeSolarPosition(fi.dateObs, siteLat, siteLong);
                fi.solarAltitude = sunPos.altitude;
                fi.solarAzimuth = sunPos.azimuth;
            }
        }

        // Solar depression pre-filter
        if (params.minSolarDepression > 0 && fi.solarAltitude > -params.minSolarDepression) {
            fi.enabled = false;
            preFiltered++;
        }

        if (i % 50 == 0 || i == total - 1)
            emit progress(i + 1, total, QString("Pre-scan: %1/%2 headers read").arg(i + 1).arg(total));
        emit frameAnalyzed(i, fi);
    }

    int activeCount = total - preFiltered;
    emit progress(total, total, QString("Pre-scan complete: %1 frames, %2 filtered by solar altitude, %3 active")
                   .arg(total).arg(preFiltered).arg(activeCount));
    emit preselectComplete();
}

void StackEngine::analyzeFrames(const StackParams &params)
{
    m_cancelled = false;
    m_debayer = params.debayer;
    loadCalibrationFrames(params);
    int total = m_frames.size();
    if (total == 0) { emit analysisComplete(); return; }

    int nThreads = params.threadCount > 0 ? params.threadCount
                   : std::max(1, (int)std::thread::hardware_concurrency());

    // Count enabled frames
    int activeCount = 0;
    for (auto &f : m_frames) if (f.enabled) activeCount++;
    if (activeCount == 0) { emit analysisComplete(); return; }

    // ── Phase 1: parallel star detection + quality (only enabled frames) ──
    emit progress(0, activeCount, QString("Phase 1: Detecting stars (%1 threads, %2 frames)...")
                   .arg(nThreads).arg(activeCount));
    m_progressCounter = 0;

    auto detectTask = [&](int i) {
        if (m_cancelled) return;
        FrameInfo &fi = m_frames[i];
        if (!fi.enabled) return;  // skip pre-filtered

        cv::Mat img = calibrateFrame(loadFits(fi.path));
        if (img.empty() || m_cancelled) { fi.enabled = false; return; }

        fi.width = img.cols;
        fi.height = img.rows;
        fi.channels = img.channels();

        cv::Mat gray = toGray(img);
        if (m_cancelled) return;
        fi.stars = params.usePCLStarDetector
                 ? detectStarsPCL(gray, params.maxStarsAlign)
                 : detectStars(gray, params.maxStarsAlign);
        if (m_cancelled) return;
        fi.starCount = fi.stars.size();

        std::vector<float> fwhms;
        for (auto &s : fi.stars)
            if (s.fwhm > 0 && s.fwhm < 30) fwhms.push_back(s.fwhm);
        if (!fwhms.empty()) {
            std::nth_element(fwhms.begin(), fwhms.begin() + fwhms.size() / 2, fwhms.end());
            fi.fwhm = fwhms[fwhms.size() / 2];
        }

        fi.backgroundLevel = robustMedian(gray);
        fi.backgroundNoise = robustMAD(gray, fi.backgroundLevel);

        // PSF fitting for better quality metrics
        if (params.psfWeighting && !m_cancelled) {
            std::vector<float> psfFwhms, eccs;
            double totalFlux = 0;
            for (auto &s : fi.stars) {
                float pA, px0, py0, psx, psy, pB, presid;
                if (fitPSF(gray, s.pos, pA, px0, py0, psx, psy, pB, presid)) {
                    s.fwhmX = 2.3548f * psx;
                    s.fwhmY = 2.3548f * psy;
                    float minS = std::min(psx, psy), maxS = std::max(psx, psy);
                    s.eccentricity = std::sqrt(1.0f - (minS * minS) / (maxS * maxS));
                    s.psfFlux = 2.0f * (float)M_PI * pA * psx * psy;
                    s.psfResidual = presid;
                    s.fwhm = (s.fwhmX + s.fwhmY) / 2.0f;
                    psfFwhms.push_back(s.fwhm);
                    eccs.push_back(s.eccentricity);
                    totalFlux += s.psfFlux;
                }
            }
            if (!psfFwhms.empty()) {
                std::nth_element(psfFwhms.begin(), psfFwhms.begin() + psfFwhms.size() / 2, psfFwhms.end());
                fi.fwhm = psfFwhms[psfFwhms.size() / 2];
                std::nth_element(eccs.begin(), eccs.begin() + eccs.size() / 2, eccs.end());
                fi.medianEccentricity = eccs[eccs.size() / 2];
                fi.totalPSFFlux = totalFlux;
            }
        }

        // Per-channel stats (used for normalization; overwritten in Phase 1b if gradient enabled)
        {
            std::vector<cv::Mat> chans;
            splitChannels(img, chans);
            for (int c = 0; c < std::min((int)chans.size(), 3); c++) {
                fi.flatMedian[c] = robustMedian(chans[c]);
                fi.flatMAD[c] = robustMAD(chans[c], fi.flatMedian[c]);
            }
        }

        if (fi.fwhm > 0 && fi.backgroundNoise > 0) {
            if (params.psfWeighting && fi.totalPSFFlux > 0) {
                double avgFlux = fi.totalPSFFlux / fi.starCount;
                double ecc = fi.medianEccentricity;
                fi.qualityScore = avgFlux / (fi.fwhm * fi.fwhm * (1.0 + ecc * ecc) * fi.backgroundNoise);
            } else {
                fi.qualityScore = fi.starCount / (fi.fwhm * fi.fwhm * fi.backgroundNoise);
            }
        }

        if (m_cancelled) return;
        // Satellite trail detection: threshold → morphological close → line scan
        {
            float thresh = fi.backgroundLevel + 15.0f * fi.backgroundNoise;
            int rows = gray.rows, cols = gray.cols;
            // Binary threshold
            cv::Mat bright(rows, cols, CV_8UC1);
            for (int y = 0; y < rows; y++) {
                const float *sp = gray.ptr<float>(y);
                uchar *dp = bright.ptr<uchar>(y);
                for (int x = 0; x < cols; x++)
                    dp[x] = sp[x] > thresh ? 255 : 0;
            }
            // Morphological close (dilate then erode) with 5x1 kernel
            auto dilateRect = [](const cv::Mat &src, cv::Mat &dst, int kw, int kh) {
                int rows = src.rows, cols = src.cols;
                int rx = kw/2, ry = kh/2;
                dst = cv::Mat(rows, cols, CV_8UC1);
                for (int y = 0; y < rows; y++) {
                    uchar *dp = dst.ptr<uchar>(y);
                    for (int x = 0; x < cols; x++) {
                        uchar mx = 0;
                        for (int dy = -ry; dy <= ry; dy++) {
                            int yy = y + dy;
                            if (yy < 0 || yy >= rows) continue;
                            const uchar *sp = src.ptr<uchar>(yy);
                            for (int dx = -rx; dx <= rx; dx++) {
                                int xx = x + dx;
                                if (xx >= 0 && xx < cols && sp[xx] > mx) mx = sp[xx];
                            }
                        }
                        dp[x] = mx;
                    }
                }
            };
            auto erodeRect = [](const cv::Mat &src, cv::Mat &dst, int kw, int kh) {
                int rows = src.rows, cols = src.cols;
                int rx = kw/2, ry = kh/2;
                dst = cv::Mat(rows, cols, CV_8UC1);
                for (int y = 0; y < rows; y++) {
                    uchar *dp = dst.ptr<uchar>(y);
                    for (int x = 0; x < cols; x++) {
                        uchar mn = 255;
                        for (int dy = -ry; dy <= ry; dy++) {
                            int yy = y + dy;
                            if (yy < 0 || yy >= rows) continue;
                            const uchar *sp = src.ptr<uchar>(yy);
                            for (int dx = -rx; dx <= rx; dx++) {
                                int xx = x + dx;
                                if (xx >= 0 && xx < cols && sp[xx] < mn) mn = sp[xx];
                            }
                        }
                        dp[x] = mn;
                    }
                }
            };
            cv::Mat tmp1, tmp2;
            dilateRect(bright, tmp1, 5, 1); erodeRect(tmp1, tmp2, 5, 1);
            dilateRect(tmp2, tmp1, 1, 5);   erodeRect(tmp1, bright, 1, 5);

            // Simple connected-component line detection (replaces HoughLinesP)
            // Scan rows for long horizontal runs, columns for long vertical runs
            double minLen = std::min(cols, rows) * 0.15;
            double maxTrailLen = 0;
            // Horizontal scan
            for (int y = 0; y < rows; y++) {
                const uchar *p = bright.ptr<uchar>(y);
                int run = 0;
                for (int x = 0; x < cols; x++) {
                    if (p[x]) { run++; }
                    else { if (run > maxTrailLen) maxTrailLen = run; run = 0; }
                }
                if (run > maxTrailLen) maxTrailLen = run;
            }
            // Vertical scan
            for (int x = 0; x < cols; x++) {
                int run = 0;
                for (int y = 0; y < rows; y++) {
                    if (bright.ptr<uchar>(y)[x]) { run++; }
                    else { if (run > maxTrailLen) maxTrailLen = run; run = 0; }
                }
                if (run > maxTrailLen) maxTrailLen = run;
            }

            if (maxTrailLen > minLen && maxTrailLen > fi.fwhm * 10) {
                fi.hasSatelliteTrail = true;
                fi.qualityScore *= 0.01;
            }
        }

        int done = ++m_progressCounter;
        QMutexLocker lock(&m_progressMutex);
        emit progress(done, activeCount, QString("Phase 1: %1/%2 — Stars: %3, FWHM: %4")
                       .arg(done).arg(activeCount).arg(fi.starCount).arg(fi.fwhm, 0, 'f', 1));
        emit frameAnalyzed(i, fi);
    };

    {
        QThreadPool pool;
        pool.setMaxThreadCount(nThreads);
        QVector<QFuture<void>> futures;
        for (int i = 0; i < total; i++)
            futures.append(QtConcurrent::run(&pool, detectTask, i));
        for (auto &f : futures) f.waitForFinished();
    }
    if (m_cancelled) return;

    // ── Select reference frame ──
    m_refIndex = params.referenceFrame;
    if (m_refIndex < 0) {
        double bestQ = -1;
        for (int i = 0; i < total; i++)
            if (m_frames[i].enabled && !m_frames[i].hasSatelliteTrail
                && m_frames[i].qualityScore > bestQ) {
                bestQ = m_frames[i].qualityScore;
                m_refIndex = i;
            }
        if (m_refIndex < 0) {
            for (int i = 0; i < total; i++)
                if (m_frames[i].enabled && m_frames[i].qualityScore > bestQ) {
                    bestQ = m_frames[i].qualityScore;
                    m_refIndex = i;
                }
        }
    }
    if (m_refIndex < 0) { emit analysisComplete(); return; }

    m_refImage = calibrateFrame(loadFits(m_frames[m_refIndex].path));
    if (m_debayer && m_refImage.channels() == 1 && !m_frames[m_refIndex].bayerPattern.isEmpty()) {
        int code = bayerCode(m_frames[m_refIndex].bayerPattern);
        if (code >= 0)
            m_refImage = demosaicFloat(m_refImage, code);
    }
    m_refStars = m_frames[m_refIndex].stars;

    int matchStarCount = std::min(40, (int)m_refStars.size());
    buildRefTriangleHash(matchStarCount);

    m_frames[m_refIndex].homography = cv::Mat::eye(3, 3, CV_64F);
    m_frames[m_refIndex].aligned = true;
    m_frames[m_refIndex].inlierCount = m_frames[m_refIndex].starCount;

    // ── Phase 2a: star matching (fast, no I/O) ──
    emit progress(0, total, "Phase 2a: Matching star patterns...");
    m_progressCounter = 0;

    auto matchTask = [&](int i) {
        if (m_cancelled) return;
        if (i == m_refIndex) { ++m_progressCounter; return; }
        FrameInfo &fi = m_frames[i];
        if (!fi.enabled || fi.stars.empty()) {
            fi.enabled = false;
            ++m_progressCounter;
            return;
        }

        auto ar = matchStars(fi.stars);
        fi.homography = ar.H;
        fi.inlierCount = ar.inliers;
        fi.aligned = !fi.homography.empty();
        if (!fi.aligned) fi.enabled = false;

        if (fi.aligned && params.distortionCorrection && ar.srcPts.size() >= 6)
            fi.tpsCoeffs = fitTPS(ar.dstPts, ar.srcPts);

        int done = ++m_progressCounter;
        if (done % 100 == 0 || done == total) {
            QMutexLocker lock(&m_progressMutex);
            emit progress(done, total, QString("Phase 2a: %1/%2 matched").arg(done).arg(total));
        }
    };

    {
        QThreadPool pool;
        pool.setMaxThreadCount(nThreads);
        QVector<QFuture<void>> futures;
        for (int i = 0; i < total; i++)
            futures.append(QtConcurrent::run(&pool, matchTask, i));
        for (auto &f : futures) f.waitForFinished();
    }
    if (m_cancelled) return;

    // ── Phase 1b: gradient fitting + normalization ──
    refitGradients(params);

    // Normalize quality scores → weights
    double maxQ = 0;
    for (auto &f : m_frames)
        if (f.enabled && f.aligned) maxQ = std::max(maxQ, f.qualityScore);
    if (maxQ > 0) {
        for (auto &f : m_frames)
            if (f.enabled && f.aligned) {
                f.qualityScore /= maxQ;
                f.weight = f.qualityScore;
            }
    }

    // Final emit for all frames (to update table after normalization)
    for (int i = 0; i < total; i++)
        emit frameAnalyzed(i, m_frames[i]);

    emit analysisComplete();
}

// ─── Gradient Refitting ─────────────────────────────────────────────────────

void StackEngine::refitGradients(const StackParams &params)
{
    m_cancelled = false;
    int total = (int)m_frames.size();

    // Build time-sorted index of enabled+aligned frames
    std::vector<int> timeOrder;
    for (int i = 0; i < total; i++)
        if (m_frames[i].enabled && m_frames[i].aligned && !m_frames[i].dateObs.isEmpty())
            timeOrder.push_back(i);
    if (timeOrder.empty()) {
        // Also accept enabled but not-yet-aligned (called from analyzeFrames before alignment)
        for (int i = 0; i < total; i++)
            if (m_frames[i].enabled && !m_frames[i].dateObs.isEmpty())
                timeOrder.push_back(i);
    }
    std::sort(timeOrder.begin(), timeOrder.end(), [this](int a, int b) {
        return m_frames[a].dateObs < m_frames[b].dateObs;
    });

    if (timeOrder.empty()) { emit gradientsComplete(); return; }

    // Select key frames at regular intervals
    int interval = std::max(1, params.gradientKeyInterval);
    std::vector<int> keyFrames;
    for (int k = 0; k < (int)timeOrder.size(); k += interval)
        keyFrames.push_back(timeOrder[k]);
    if (keyFrames.back() != timeOrder.back())
        keyFrames.push_back(timeOrder.back());

    int nKeys = keyFrames.size();
    int gradDeg = params.gradientDegree;

    if (gradDeg < 1) {
        // Clear any previous gradient coefficients
        for (int i = 0; i < total; i++) {
            for (int c = 0; c < 3; c++)
                m_frames[i].gradientCoeffs[c].clear();
        }
        emit progress(nKeys, nKeys, "Gradient fitting skipped (degree=0)");
    } else {

    emit progress(0, nKeys, QString("Gradient fitting (%1 key frames)...").arg(nKeys));
    for (int ki = 0; ki < nKeys; ki++) {
        if (m_cancelled) return;
        int idx = keyFrames[ki];
        FrameInfo &fi = m_frames[idx];

        cv::Mat img = calibrateFrame(loadFits(fi.path));
        if (img.empty()) continue;

        std::vector<cv::Mat> chans;
        splitChannels(img, chans);
        int nc = std::min((int)chans.size(), 3);

        if (params.rayleighModel && nc == 3) {
            double dAz = fi.solarAzimuth - fi.objAzimuth;
            if (dAz > 180) dAz -= 360;
            if (dAz < -180) dAz += 360;
            dAz *= cos(fi.objAltitude * M_PI / 180.0);
            double dAlt = fi.solarAltitude - fi.objAltitude;
            double mag = sqrt(dAz * dAz + dAlt * dAlt);
            double sunDirX = 0, sunDirY = 0;
            if (mag > 0.01) { sunDirX = dAz / mag; sunDirY = -dAlt / mag; }
            fitRayleighGradient(chans, fi.stars, gradDeg, sunDirX, sunDirY, fi.gradientCoeffs);
        } else {
            for (int c = 0; c < nc; c++)
                fitGradient(chans[c], fi.stars, gradDeg, fi.gradientCoeffs[c]);
        }

        // Compute flat statistics for key frame
        int usedDeg = (params.rayleighModel) ? std::max(1, gradDeg) : gradDeg;
        for (int c = 0; c < nc; c++) {
            if (fi.gradientCoeffs[c].empty()) continue;
            cv::Mat grad = evalGradient(fi.height, fi.width, fi.gradientCoeffs[c], usedDeg);
            cv::Mat flat = chans[c] - grad;
            fi.flatMedian[c] = robustMedian(flat);
            fi.flatMAD[c] = robustMAD(flat, fi.flatMedian[c]);

            if (params.localNormalization) {
                int gs = params.localNormGridSize;
                fi.localGridSize = gs;
                fi.localMedian[c].resize(gs * gs);
                fi.localMAD[c].resize(gs * gs);
                int cellH = flat.rows / gs, cellW = flat.cols / gs;
                for (int gy = 0; gy < gs; gy++)
                    for (int gx = 0; gx < gs; gx++) {
                        int y0 = gy * cellH, x0 = gx * cellW;
                        int y1 = (gy == gs - 1) ? flat.rows : y0 + cellH;
                        int x1 = (gx == gs - 1) ? flat.cols : x0 + cellW;
                        std::vector<float> vals;
                        vals.reserve((y1 - y0) * (x1 - x0) / 4);
                        for (int y = y0; y < y1; y += 2) {
                            const float *row = flat.ptr<float>(y);
                            for (int x = x0; x < x1; x += 2) {
                                float v = row[x];
                                if (std::isfinite(v)) vals.push_back(v);
                            }
                        }
                        int idx = gy * gs + gx;
                        if (vals.size() >= 100) {
                            std::nth_element(vals.begin(), vals.begin() + vals.size() / 2, vals.end());
                            double med = vals[vals.size() / 2];
                            fi.localMedian[c][idx] = med;
                            std::vector<float> dev(vals.size());
                            for (size_t k = 0; k < vals.size(); k++)
                                dev[k] = std::abs(vals[k] - (float)med);
                            std::nth_element(dev.begin(), dev.begin() + dev.size() / 2, dev.end());
                            fi.localMAD[c][idx] = dev[dev.size() / 2] * 1.4826;
                        } else {
                            fi.localMedian[c][idx] = fi.flatMedian[c];
                            fi.localMAD[c][idx] = fi.flatMAD[c];
                        }
                    }
            }
        }

        emit progress(ki + 1, nKeys, QString("Gradient fitting %1/%2").arg(ki + 1).arg(nKeys));
    }
    if (m_cancelled) return;

    // Interpolate gradient coefficients + flat stats for non-key frames
    for (int ti = 0; ti < (int)timeOrder.size(); ti++) {
        int idx = timeOrder[ti];
        bool isKey = false;
        for (int k : keyFrames) if (k == idx) { isKey = true; break; }
        if (isKey) continue;

        int prevKey = -1, nextKey = -1;
        for (int ki = 0; ki < nKeys; ki++) {
            if (m_frames[keyFrames[ki]].dateObs <= m_frames[idx].dateObs)
                prevKey = ki;
        }
        for (int ki = nKeys - 1; ki >= 0; ki--) {
            if (m_frames[keyFrames[ki]].dateObs >= m_frames[idx].dateObs)
                nextKey = ki;
        }

        if (prevKey < 0) prevKey = nextKey;
        if (nextKey < 0) nextKey = prevKey;
        if (prevKey < 0) continue;

        int pi = keyFrames[prevKey], ni = keyFrames[nextKey];
        FrameInfo &fi = m_frames[idx];
        FrameInfo &pf = m_frames[pi];
        FrameInfo &nf = m_frames[ni];

        double t = 0.5;
        if (pi != ni) {
            double posP = 0, posN = 0, posI = 0;
            for (int k = 0; k < (int)timeOrder.size(); k++) {
                if (timeOrder[k] == pi) posP = k;
                if (timeOrder[k] == ni) posN = k;
                if (timeOrder[k] == idx) posI = k;
            }
            t = (posN > posP) ? (posI - posP) / (posN - posP) : 0.5;
        }

        for (int c = 0; c < 3; c++) {
            size_t ncoeffs = std::max(pf.gradientCoeffs[c].size(), nf.gradientCoeffs[c].size());
            fi.gradientCoeffs[c].resize(ncoeffs, 0);
            for (size_t j = 0; j < ncoeffs; j++) {
                double vp = (j < pf.gradientCoeffs[c].size()) ? pf.gradientCoeffs[c][j] : 0;
                double vn = (j < nf.gradientCoeffs[c].size()) ? nf.gradientCoeffs[c][j] : 0;
                fi.gradientCoeffs[c][j] = vp * (1.0 - t) + vn * t;
            }
            fi.flatMedian[c] = pf.flatMedian[c] * (1.0 - t) + nf.flatMedian[c] * t;
            fi.flatMAD[c] = pf.flatMAD[c] * (1.0 - t) + nf.flatMAD[c] * t;

            // Interpolate local normalization grids
            if (params.localNormalization && pf.localGridSize > 0 && nf.localGridSize > 0) {
                int gs = pf.localGridSize;
                fi.localGridSize = gs;
                fi.localMedian[c].resize(gs * gs);
                fi.localMAD[c].resize(gs * gs);
                for (int j = 0; j < gs * gs; j++) {
                    fi.localMedian[c][j] = pf.localMedian[c][j] * (1.0 - t) + nf.localMedian[c][j] * t;
                    fi.localMAD[c][j] = pf.localMAD[c][j] * (1.0 - t) + nf.localMAD[c][j] * t;
                }
            }
        }
    }

    } // end if gradDeg >= 1

    // ── Compute normalization from stored statistics (no I/O) ──
    if (m_refIndex >= 0) {
        FrameInfo &ref = m_frames[m_refIndex];
        for (int c = 0; c < 3; c++) {
            ref.scale[c] = 1.0;
            ref.offset[c] = 0.0;
        }
        for (int i = 0; i < (int)m_frames.size(); i++) {
            if (i == m_refIndex) continue;
            FrameInfo &fi = m_frames[i];
            if (!fi.enabled || !fi.aligned) continue;

            for (int c = 0; c < 3; c++) {
                double refSig = ref.flatMAD[c];
                double frmSig = fi.flatMAD[c];
                fi.scale[c] = (frmSig > 1e-6) ? refSig / frmSig : 1.0;
                fi.offset[c] = ref.flatMedian[c] - fi.scale[c] * fi.flatMedian[c];
            }
        }
    }

    emit gradientsComplete();
}

// ─── Stacking ───────────────────────────────────────────────────────────────

cv::Mat StackEngine::stackFrames(const StackParams &params)
{
    m_cancelled = false;
    if (m_refImage.empty() || m_frames.empty()) return {};

    int h = m_refImage.rows, w = m_refImage.cols;

    // When debayering, output is always 3 channels (ref image is demosaiced in analyzeFrames)
    int nc = m_refImage.channels();

    std::vector<int> selected;
    for (int i = 0; i < (int)m_frames.size(); i++) {
        auto &f = m_frames[i];
        if (f.enabled && f.aligned && f.qualityScore >= params.qualityThreshold)
            selected.push_back(i);
    }
    if (selected.empty()) return {};

    int nFrames = selected.size();
    int totalPasses = params.clipIterations;
    int totalOps = nFrames * totalPasses;

    std::vector<cv::Mat> sum(nc), sumSq(nc), weightSum(nc);
    cv::Mat countMat;
    std::vector<cv::Mat> mean(nc), sigma(nc);

    auto resetAccum = [&]() {
        for (int c = 0; c < nc; c++) {
            sum[c] = cv::Mat::zeros(h, w, CV_64FC1);
            sumSq[c] = cv::Mat::zeros(h, w, CV_64FC1);
            weightSum[c] = cv::Mat::zeros(h, w, CV_64FC1);
        }
        countMat = cv::Mat::zeros(h, w, CV_32SC1);
    };

    for (int pass = 0; pass < totalPasses; pass++) {
        resetAccum();
        bool doClip = (pass > 0) && !mean[0].empty();

        // Pipeline: kick off first load asynchronously
        std::future<cv::Mat> nextLoad;
        if (nFrames > 0)
            nextLoad = std::async(std::launch::async, &StackEngine::loadFits, m_frames[selected[0]].path);

        for (int si = 0; si < nFrames; si++) {
            if (m_cancelled) return {};

            int idx = selected[si];
            FrameInfo &fi = m_frames[idx];
            double w_i = fi.weight;

            // Get pre-loaded frame from pipeline and apply calibration
            cv::Mat raw = calibrateFrame(nextLoad.get());

            // Start loading the next frame while we process this one
            if (si + 1 < nFrames)
                nextLoad = std::async(std::launch::async, &StackEngine::loadFits,
                                      m_frames[selected[si + 1]].path);

            if (raw.empty()) continue;

            // Decide preparation path:
            // - Bayer drizzle: raw mono CFA → 3ch with per-channel mask
            // - Normal: demosaic-then-warp (or WCS reproject)
            bool useBayerDrizzle = m_debayer
                                && raw.channels() == 1
                                && !fi.bayerPattern.isEmpty();

            cv::Mat prepared;
            cv::Mat mask;        // 1-channel mask (normal path)
            cv::Mat mask3;       // 3-channel mask (Bayer drizzle path)

            if (useBayerDrizzle) {
                prepared = prepareFrameBayerDrizzle(fi, raw, mask3);
            } else if (params.wcsAlignment) {
                prepared = prepareFrameWCS(fi, raw, mask);
            } else {
                prepared = prepareFrame(fi, raw, mask);
            }

            std::vector<cv::Mat> channels;
            splitChannels(prepared, channels);

            std::vector<cv::Mat> maskCh;
            if (useBayerDrizzle)
                splitChannels(mask3, maskCh);

            // Parallel accumulation over rows
            float sigLo = (float)params.sigmaLow;
            float sigHi = (float)params.sigmaHigh;
            parallelFor(0, h, [&](int lo, int hi) {
                for (int y = lo; y < hi; y++) {
                    const uchar *pm1 = useBayerDrizzle ? nullptr : mask.ptr<uchar>(y);
                    int *pc = countMat.ptr<int>(y);

                    for (int c = 0; c < nc; c++) {
                        const float *pv = channels[c].ptr<float>(y);
                        double *ps = sum[c].ptr<double>(y);
                        double *psq = sumSq[c].ptr<double>(y);
                        double *pw = weightSum[c].ptr<double>(y);
                        const float *pmean = doClip ? mean[c].ptr<float>(y) : nullptr;
                        const float *psigma = doClip ? sigma[c].ptr<float>(y) : nullptr;
                        const uchar *pmC = useBayerDrizzle ? maskCh[c].ptr<uchar>(y) : nullptr;

                        for (int x = 0; x < w; x++) {
                            // Per-channel mask for Bayer drizzle, single mask otherwise
                            if (useBayerDrizzle ? (pmC[x] == 0) : (pm1[x] == 0))
                                continue;
                            float val = pv[x];
                            if (!std::isfinite(val)) continue;

                            if (doClip && pmean && psigma) {
                                float lo = pmean[x] - sigLo * psigma[x];
                                float hi = pmean[x] + sigHi * psigma[x];
                                if (val < lo || val > hi) continue;
                            }

                            ps[x] += w_i * val;
                            psq[x] += w_i * val * val;
                            pw[x] += w_i;
                            if (c == 0) pc[x]++;
                        }
                    }
                }
            });

            emit progress(pass * nFrames + si + 1, totalOps,
                          QString("Pass %1: frame %2/%3").arg(pass + 1).arg(si + 1).arg(nFrames));
        }

        for (int c = 0; c < nc; c++) {
            mean[c] = cv::Mat(h, w, CV_32FC1);
            sigma[c] = cv::Mat(h, w, CV_32FC1);
        }
        parallelFor(0, h, [&](int lo, int hi) {
            for (int y = lo; y < hi; y++) {
                for (int c = 0; c < nc; c++) {
                    const double *ps = sum[c].ptr<double>(y);
                    const double *psq = sumSq[c].ptr<double>(y);
                    const double *pw = weightSum[c].ptr<double>(y);
                    float *pm = mean[c].ptr<float>(y);
                    float *psg = sigma[c].ptr<float>(y);
                    for (int x = 0; x < w; x++) {
                        if (pw[x] > 0) {
                            double m = ps[x] / pw[x];
                            double v = psq[x] / pw[x] - m * m;
                            pm[x] = (float)m;
                            psg[x] = (float)std::sqrt(std::max(0.0, v));
                        } else {
                            pm[x] = 0;
                            psg[x] = 0;
                        }
                    }
                }
            }
        });
    }

    cv::Mat result;
    mergeChannels(mean, result);
    emit stackComplete(result);
    return result;
}

// ─── Display Helper ─────────────────────────────────────────────────────────

cv::Mat StackEngine::autoStretch(const cv::Mat &image, double blackPct, double whitePct)
{
    cv::Mat gray = toGray(image);
    std::vector<float> vals;
    vals.reserve(gray.rows * gray.cols / 16);
    for (int y = 0; y < gray.rows; y += 4)
        for (int x = 0; x < gray.cols; x += 4) {
            float v = gray.at<float>(y, x);
            if (std::isfinite(v)) vals.push_back(v);
        }
    std::sort(vals.begin(), vals.end());

    float black = vals[(int)(vals.size() * blackPct / 100.0)];
    float white = vals[(int)(vals.size() * whitePct / 100.0)];
    if (white <= black) white = black + 1;

    cv::Mat stretched;
    image.convertTo(stretched, CV_32F);
    stretched = (stretched - black) / (white - black);

    float midtone = 0.25f;
    stretched.forEach<cv::Vec3f>([midtone](cv::Vec3f &pixel, const int *) {
        for (int i = 0; i < 3; i++) {
            float v = std::clamp(pixel[i], 0.0f, 1.0f);
            if (v > 0 && v < 1)
                v = (midtone - 1.0f) * v / ((2.0f * midtone - 1.0f) * v - midtone);
            pixel[i] = v;
        }
    });

    cv::Mat out;
    stretched.convertTo(out, CV_8U, 255.0);
    return out;
}

// ─── SimpleTANWCS implementation ────────────────────────────────────────────

bool SimpleTANWCS::pixelToWorld(double px, double py, double &ra, double &dec) const
{
    if (!valid) return false;
    double dx = px - crpix1;
    double dy = py - crpix2;
    double xi  = cd11 * dx + cd12 * dy;   // degrees
    double eta = cd21 * dx + cd22 * dy;

    double xi_r  = xi  * M_PI / 180.0;
    double eta_r = eta * M_PI / 180.0;
    double dec0  = crval2 * M_PI / 180.0;

    double denom = cos(dec0) - eta_r * sin(dec0);
    if (std::abs(denom) < 1e-12) return false;

    ra  = crval1 + atan2(xi_r, denom) * 180.0 / M_PI;
    dec = atan(cos(atan2(xi_r, denom)) * (sin(dec0) + eta_r * cos(dec0)) / denom) * 180.0 / M_PI;

    if (ra < 0)   ra += 360.0;
    if (ra >= 360) ra -= 360.0;
    return true;
}

bool SimpleTANWCS::worldToPixel(double ra, double dec, double &px, double &py) const
{
    if (!valid) return false;
    double ra_r  = ra  * M_PI / 180.0;
    double dec_r = dec * M_PI / 180.0;
    double ra0_r = crval1 * M_PI / 180.0;
    double dec0  = crval2 * M_PI / 180.0;
    double dra   = ra_r - ra0_r;

    double denom = sin(dec_r) * sin(dec0) + cos(dec_r) * cos(dec0) * cos(dra);
    if (std::abs(denom) < 1e-12) return false;

    double xi  = (cos(dec_r) * sin(dra)) / denom;
    double eta = (sin(dec_r) * cos(dec0) - cos(dec_r) * sin(dec0) * cos(dra)) / denom;

    xi  *= 180.0 / M_PI;
    eta *= 180.0 / M_PI;

    double det = cd11 * cd22 - cd12 * cd21;
    if (std::abs(det) < 1e-20) return false;

    px = ( cd22 * xi - cd12 * eta) / det + crpix1;
    py = (-cd21 * xi + cd11 * eta) / det + crpix2;
    return true;
}

double SimpleTANWCS::pixelScale() const
{
    double det = std::abs(cd11 * cd22 - cd12 * cd21);
    return std::sqrt(det) * 3600.0;   // arcsec/pixel
}

// ─── Additional stretch modes ───────────────────────────────────────────────

cv::Mat StackEngine::stretch(const cv::Mat &image, StretchMode mode,
                             double blackPct, double whitePct, double gamma)
{
    if (mode == StretchMode::Auto)
        return autoStretch(image, blackPct, whitePct);

    // Common: compute percentile black/white from luminance
    cv::Mat gray = toGray(image);
    std::vector<float> vals;
    vals.reserve(gray.rows * gray.cols / 16);
    for (int y = 0; y < gray.rows; y += 4)
        for (int x = 0; x < gray.cols; x += 4) {
            float v = gray.at<float>(y, x);
            if (std::isfinite(v)) vals.push_back(v);
        }
    std::sort(vals.begin(), vals.end());

    float black = vals[(size_t)(vals.size() * blackPct / 100.0)];
    float white = vals[(size_t)(vals.size() * whitePct / 100.0)];
    if (white <= black) white = black + 1;

    cv::Mat stretched;
    image.convertTo(stretched, CV_32F);
    stretched = (stretched - black) / (white - black);

    int nc = stretched.channels();
    float invGamma = 1.0f / (float)gamma;
    float sinh3 = std::sinh(3.0f);

    if (nc == 3) {
        stretched.forEach<cv::Vec3f>([&](cv::Vec3f &pixel, const int *) {
            for (int i = 0; i < 3; i++) {
                float v = std::clamp(pixel[i], 0.0f, 1.0f);
                switch (mode) {
                    case StretchMode::Linear: break;
                    case StretchMode::Gamma:  v = std::pow(v, invGamma); break;
                    case StretchMode::Log:    v = std::log2(1.0f + v); break;
                    case StretchMode::Sinh:   v = std::sinh(v * 3.0f) / sinh3; break;
                    default: break;
                }
                pixel[i] = std::clamp(v, 0.0f, 1.0f);
            }
        });
    } else {
        stretched.forEach<float>([&](float &v, const int *) {
            v = std::clamp(v, 0.0f, 1.0f);
            switch (mode) {
                case StretchMode::Linear: break;
                case StretchMode::Gamma:  v = std::pow(v, invGamma); break;
                case StretchMode::Log:    v = std::log2(1.0f + v); break;
                case StretchMode::Sinh:   v = std::sinh(v * 3.0f) / sinh3; break;
                default: break;
            }
            v = std::clamp(v, 0.0f, 1.0f);
        });
    }

    cv::Mat out;
    stretched.convertTo(out, CV_8U, 255.0);
    return out;
}

// ─── CFA-aware 2x2 binning ─────────────────────────────────────────────────

cv::Mat StackEngine::cfaBin2x2(const cv::Mat &bayer)
{
    if (bayer.channels() != 1 || bayer.rows < 2 || bayer.cols < 2)
        return bayer;

    int bh = bayer.rows / 2;
    int bw = bayer.cols / 2;
    cv::Mat out(bh, bw, CV_32FC1);

    for (int y = 0; y < bh; y++) {
        const float *r0 = bayer.ptr<float>(y * 2);
        const float *r1 = bayer.ptr<float>(y * 2 + 1);
        float *dst = out.ptr<float>(y);
        for (int x = 0; x < bw; x++) {
            int x2 = x * 2;
            dst[x] = (r0[x2] + r0[x2 + 1] + r1[x2] + r1[x2 + 1]) * 0.25f;
        }
    }
    return out;
}

// ─── WCS reprojection ───────────────────────────────────────────────────────

cv::Mat StackEngine::prepareFrameWCS(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask)
{
    if (!fi.wcs.valid || !m_frames[m_refIndex].wcs.valid)
        return prepareFrame(fi, raw, mask);   // fallback to homography

    const SimpleTANWCS &refWCS = m_frames[m_refIndex].wcs;
    int h = m_refImage.rows, w = m_refImage.cols;
    int nc = m_refImage.channels();

    // Demosaic if needed
    cv::Mat input = raw;
    if (m_debayer && raw.channels() == 1 && !fi.bayerPattern.isEmpty()) {
        int code = bayerCode(fi.bayerPattern);
        if (code >= 0)
            input = demosaicFloat(raw, code);
    }

    cv::Mat result = cv::Mat::zeros(h, w, input.type());
    mask = cv::Mat::zeros(h, w, CV_8UC1);

    // For each output pixel, find corresponding source pixel via WCS
    parallelFor(0, h, [&](int lo, int hi) {
        for (int y = lo; y < hi; y++) {
            for (int x = 0; x < w; x++) {
                // Output pixel → world
                double ra, dec;
                if (!refWCS.pixelToWorld(x + 1.0, y + 1.0, ra, dec))
                    continue;

                // World → source pixel
                double srcX, srcY;
                if (!fi.wcs.worldToPixel(ra, dec, srcX, srcY))
                    continue;

                // Convert to 0-indexed
                float sx = (float)(srcX - 1.0);
                float sy = (float)(srcY - 1.0);

                if (sx < 0 || sx >= input.cols - 1 || sy < 0 || sy >= input.rows - 1)
                    continue;

                // Bilinear interpolation
                int x0 = (int)std::floor(sx);
                int y0 = (int)std::floor(sy);
                float fx = sx - x0;
                float fy = sy - y0;

                if (nc >= 3) {
                    const cv::Vec3f &p00 = input.at<cv::Vec3f>(y0, x0);
                    const cv::Vec3f &p01 = input.at<cv::Vec3f>(y0, x0 + 1);
                    const cv::Vec3f &p10 = input.at<cv::Vec3f>(y0 + 1, x0);
                    const cv::Vec3f &p11 = input.at<cv::Vec3f>(y0 + 1, x0 + 1);

                    cv::Vec3f val;
                    for (int c = 0; c < 3; c++) {
                        float v0 = p00[c] * (1 - fx) + p01[c] * fx;
                        float v1 = p10[c] * (1 - fx) + p11[c] * fx;
                        val[c] = v0 * (1 - fy) + v1 * fy;
                    }
                    result.at<cv::Vec3f>(y, x) = val;
                } else {
                    float v00 = input.at<float>(y0, x0);
                    float v01 = input.at<float>(y0, x0 + 1);
                    float v10 = input.at<float>(y0 + 1, x0);
                    float v11 = input.at<float>(y0 + 1, x0 + 1);
                    float v0 = v00 * (1 - fx) + v01 * fx;
                    float v1 = v10 * (1 - fx) + v11 * fx;
                    result.at<float>(y, x) = v0 * (1 - fy) + v1 * fy;
                }
                mask.at<uchar>(y, x) = 255;
            }
        }
    });

    // Apply normalization (scale/offset from analyzeFrames)
    std::vector<cv::Mat> channels;
    splitChannels(result, channels);
    for (int c = 0; c < nc; c++) {
        channels[c] = fi.scale[c] * (channels[c] - fi.offset[c])
                      + m_frames[m_refIndex].flatMedian[c];
    }
    mergeChannels(channels, result);

    return result;
}

// ─── Bayer drizzle ──────────────────────────────────────────────────────────
// Map Bayer pattern + pixel position to channel index (0=R, 1=G, 2=B)
static int bayerChannel(const QString &pat, int x, int y)
{
    // Bayer 2x2 pattern: index into RGGB / BGGR / GRBG / GBRG
    //   (x%2, y%2) selects one of 4 positions in the 2x2 tile
    int bx = x & 1, by = y & 1;
    int pos = by * 2 + bx;  // 0=TL, 1=TR, 2=BL, 3=BR

    if (pat == "RGGB") { static const int ch[] = {0,1,1,2}; return ch[pos]; }
    if (pat == "BGGR") { static const int ch[] = {2,1,1,0}; return ch[pos]; }
    if (pat == "GRBG") { static const int ch[] = {1,0,2,1}; return ch[pos]; }
    if (pat == "GBRG") { static const int ch[] = {1,2,0,1}; return ch[pos]; }
    return 1; // default: treat as green
}

cv::Mat StackEngine::prepareFrameBayerDrizzle(const FrameInfo &fi,
                                               const cv::Mat &raw,
                                               cv::Mat &mask3)
{
    int h = m_refImage.rows, w = m_refImage.cols;

    // Output: 3-channel float, one value per Bayer colour
    cv::Mat result = cv::Mat::zeros(h, w, CV_32FC3);
    mask3 = cv::Mat::zeros(h, w, CV_8UC3);

    // We need the inverse homography: output → source
    cv::Mat Hinv;
    if (fi.tpsCoeffs.valid) {
        // TPS: generate inverse remap maps (already map output→source)
        cv::Mat mapX, mapY;
        generateTPSRemapMaps(fi.tpsCoeffs, h, w, mapX, mapY);

        parallelFor(0, h, [&](int lo, int hi) {
            for (int y = lo; y < hi; y++) {
                const float *mx = mapX.ptr<float>(y);
                const float *my = mapY.ptr<float>(y);
                for (int x = 0; x < w; x++) {
                    // Nearest-neighbour in source (don't interpolate across colours)
                    int sx = (int)std::round(mx[x]);
                    int sy = (int)std::round(my[x]);
                    if (sx < 0 || sx >= raw.cols || sy < 0 || sy >= raw.rows)
                        continue;

                    float val = raw.at<float>(sy, sx);
                    if (!std::isfinite(val)) continue;

                    int ch = bayerChannel(fi.bayerPattern, sx, sy);
                    result.at<cv::Vec3f>(y, x)[ch] = val;
                    mask3.at<cv::Vec3b>(y, x)[ch] = 255;
                }
            }
        });
    } else {
        // Homography: invert H to get output→source mapping
        Hinv = fi.homography.inv();

        parallelFor(0, h, [&](int lo, int hi) {
            // Cache homography inverse elements
            double h00 = Hinv.at<double>(0,0), h01 = Hinv.at<double>(0,1), h02 = Hinv.at<double>(0,2);
            double h10 = Hinv.at<double>(1,0), h11 = Hinv.at<double>(1,1), h12 = Hinv.at<double>(1,2);
            double h20 = Hinv.at<double>(2,0), h21 = Hinv.at<double>(2,1), h22 = Hinv.at<double>(2,2);

            for (int y = lo; y < hi; y++) {
                for (int x = 0; x < w; x++) {
                    // Apply inverse homography: output (x,y) → source (sx,sy)
                    double denom = h20 * x + h21 * y + h22;
                    if (std::abs(denom) < 1e-12) continue;
                    double srcXf = (h00 * x + h01 * y + h02) / denom;
                    double srcYf = (h10 * x + h11 * y + h12) / denom;

                    // Nearest-neighbour — essential to preserve Bayer colour identity
                    int sx = (int)std::round(srcXf);
                    int sy = (int)std::round(srcYf);
                    if (sx < 0 || sx >= raw.cols || sy < 0 || sy >= raw.rows)
                        continue;

                    float val = raw.at<float>(sy, sx);
                    if (!std::isfinite(val)) continue;

                    int ch = bayerChannel(fi.bayerPattern, sx, sy);
                    result.at<cv::Vec3f>(y, x)[ch] = val;
                    mask3.at<cv::Vec3b>(y, x)[ch] = 255;
                }
            }
        });
    }

    // Apply per-channel normalization (scale/offset)
    // Note: gradient subtraction uses the source-space coefficients
    std::vector<cv::Mat> channels;
    splitChannels(result, channels);
    std::vector<cv::Mat> maskCh;
    splitChannels(mask3, maskCh);

    for (int c = 0; c < 3; c++) {
        if (!fi.gradientCoeffs[c].empty()) {
            int deg = (int)std::round((-1 + std::sqrt(1 + 8.0 * fi.gradientCoeffs[c].size())) / 2.0);
            cv::Mat origGrad = evalGradient(raw.rows, raw.cols, fi.gradientCoeffs[c], deg);
            // For Bayer drizzle we need the gradient in output space
            // Use the homography to warp it (this is low-frequency so bilinear is fine)
            cv::Mat grad;
            if (fi.tpsCoeffs.valid) {
                cv::Mat gMapX, gMapY;
                generateTPSRemapMaps(fi.tpsCoeffs, h, w, gMapX, gMapY);
                cv::remap(origGrad, grad, gMapX, gMapY, cv::INTER_LINEAR,
                          cv::BORDER_CONSTANT, cv::Scalar(0));
            } else {
                cv::warpPerspective(origGrad, grad, fi.homography, cv::Size(w, h),
                                    cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
            }
            // Apply only where this channel has data
            for (int y = 0; y < h; y++) {
                float *pch = channels[c].ptr<float>(y);
                const float *pg = grad.ptr<float>(y);
                const uchar *pm = maskCh[c].ptr<uchar>(y);
                for (int x = 0; x < w; x++) {
                    if (pm[x])
                        pch[x] = (float)(fi.scale[c] * (pch[x] - pg[x]) + fi.offset[c]);
                }
            }
        } else {
            for (int y = 0; y < h; y++) {
                float *pch = channels[c].ptr<float>(y);
                const uchar *pm = maskCh[c].ptr<uchar>(y);
                for (int x = 0; x < w; x++) {
                    if (pm[x])
                        pch[x] = (float)(fi.scale[c] * pch[x] + fi.offset[c]);
                }
            }
        }
    }

    mergeChannels(channels, result);
    return result;
}
