#include "stackengine_cli.h"
#ifndef emit
#define emit
#endif
#include <fitsio.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <thread>
#include <future>
#include <set>
#include <map>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

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
    cv::merge(planes, merged);
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
    cv::split(image, channels);
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
        cv::merge(gradChans, gradient);

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
    cv::split(color, ch);
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

// ─── Star Detection (DoG) ───────────────────────────────────────────────────

std::vector<StarPos> StackEngine::detectStars(const cv::Mat &gray, int maxStars)
{
    cv::Mat blur1, blur2, dog;
    cv::GaussianBlur(gray, blur1, cv::Size(0, 0), 1.5);
    cv::GaussianBlur(gray, blur2, cv::Size(0, 0), 4.0);
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
    cv::dilate(dog, dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));

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

// ─── Alignment: Geometric Triangle Matching ─────────────────────────────────

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
    cv::Mat H = cv::findHomography(srcPts, dstPts, cv::RANSAC, 3.0, mask, 2000, 0.995);
    if (H.empty()) return res;

    res.inliers = cv::countNonZero(mask);
    if (res.inliers < 4) return res;
    res.H = H;
    return res;
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
        cv::solve(A, b, c, cv::DECOMP_SVD);
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
        cv::solve(A, b, c, cv::DECOMP_SVD);
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

        cv::solve(A, b, solution, cv::DECOMP_SVD);

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

// ─── Frame Preparation ─────────────────────────────────────────────────────

cv::Mat StackEngine::prepareFrame(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask)
{
    int h = m_refImage.rows, w = m_refImage.cols;

    cv::Mat warped;
    cv::warpPerspective(raw, warped, fi.homography, cv::Size(w, h),
                        cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

    cv::Mat ones = cv::Mat::ones(raw.size(), CV_8UC1);
    cv::warpPerspective(ones, mask, fi.homography, cv::Size(w, h),
                        cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));

    std::vector<cv::Mat> channels;
    cv::split(warped, channels);

    int nc = std::min((int)channels.size(), 3);
    for (int c = 0; c < nc; c++) {
        cv::Mat grad;
        if (!fi.gradientCoeffs[c].empty()) {
            int deg = (int)std::round((-1 + std::sqrt(1 + 8.0 * fi.gradientCoeffs[c].size())) / 2.0);
            cv::Mat origGrad = evalGradient(raw.rows, raw.cols, fi.gradientCoeffs[c], deg);
            cv::warpPerspective(origGrad, grad, fi.homography, cv::Size(w, h),
                                cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
        } else {
            grad = cv::Mat::zeros(h, w, CV_32FC1);
        }
        channels[c] = fi.scale[c] * (channels[c] - grad) + fi.offset[c];
    }

    cv::Mat result;
    cv::merge(channels, result);
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

        cv::Mat img = loadFits(fi.path);
        if (img.empty() || m_cancelled) { fi.enabled = false; return; }

        fi.width = img.cols;
        fi.height = img.rows;
        fi.channels = img.channels();

        cv::Mat gray = toGray(img);
        if (m_cancelled) return;
        fi.stars = detectStars(gray, params.maxStarsAlign);
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

        // Per-channel stats (used for normalization; overwritten in Phase 1b if gradient enabled)
        {
            std::vector<cv::Mat> chans;
            cv::split(img, chans);
            for (int c = 0; c < std::min((int)chans.size(), 3); c++) {
                fi.flatMedian[c] = robustMedian(chans[c]);
                fi.flatMAD[c] = robustMAD(chans[c], fi.flatMedian[c]);
            }
        }

        if (fi.fwhm > 0 && fi.backgroundNoise > 0)
            fi.qualityScore = fi.starCount / (fi.fwhm * fi.fwhm * fi.backgroundNoise);

        if (m_cancelled) return;
        // Satellite trail detection: threshold → Hough lines
        {
            float thresh = fi.backgroundLevel + 15.0f * fi.backgroundNoise;
            cv::Mat bright;
            cv::threshold(gray, bright, thresh, 255, cv::THRESH_BINARY);
            bright.convertTo(bright, CV_8U);

            // Morphological close to connect trail segments
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1));
            cv::morphologyEx(bright, bright, cv::MORPH_CLOSE, kernel);
            kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 5));
            cv::morphologyEx(bright, bright, cv::MORPH_CLOSE, kernel);

            std::vector<cv::Vec4i> lines;
            cv::HoughLinesP(bright, lines, 1, CV_PI / 180, 50,
                            std::min(gray.cols, gray.rows) * 0.15, // minLineLength: 15% of image
                            20);                                    // maxLineGap

            if (!lines.empty()) {
                // Check for lines significantly longer than star FWHM
                double maxLen = 0;
                for (auto &l : lines) {
                    double len = std::sqrt((l[2]-l[0])*(l[2]-l[0]) + (l[3]-l[1])*(l[3]-l[1]));
                    maxLen = std::max(maxLen, len);
                }
                if (maxLen > fi.fwhm * 10) {
                    fi.hasSatelliteTrail = true;
                    fi.qualityScore *= 0.01; // heavily penalize
                }
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

    m_refImage = loadFits(m_frames[m_refIndex].path);
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

        cv::Mat img = loadFits(fi.path);
        if (img.empty()) continue;

        std::vector<cv::Mat> chans;
        cv::split(img, chans);
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

    int h = m_refImage.rows, w = m_refImage.cols, nc = m_refImage.channels();

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

            // Get pre-loaded frame from pipeline
            cv::Mat raw = nextLoad.get();

            // Start loading the next frame while we process this one
            if (si + 1 < nFrames)
                nextLoad = std::async(std::launch::async, &StackEngine::loadFits,
                                      m_frames[selected[si + 1]].path);

            if (raw.empty()) continue;

            cv::Mat mask;
            cv::Mat prepared = prepareFrame(fi, raw, mask);

            std::vector<cv::Mat> channels;
            cv::split(prepared, channels);

            // Parallel accumulation over rows
            float sigLo = (float)params.sigmaLow;
            float sigHi = (float)params.sigmaHigh;
            cv::parallel_for_(cv::Range(0, h), [&](const cv::Range &range) {
                for (int y = range.start; y < range.end; y++) {
                    const uchar *pm = mask.ptr<uchar>(y);
                    int *pc = countMat.ptr<int>(y);

                    for (int c = 0; c < nc; c++) {
                        const float *pv = channels[c].ptr<float>(y);
                        double *ps = sum[c].ptr<double>(y);
                        double *psq = sumSq[c].ptr<double>(y);
                        double *pw = weightSum[c].ptr<double>(y);
                        const float *pmean = doClip ? mean[c].ptr<float>(y) : nullptr;
                        const float *psigma = doClip ? sigma[c].ptr<float>(y) : nullptr;

                        for (int x = 0; x < w; x++) {
                            if (pm[x] == 0) continue;
                            float val = pv[x];
                            if (!std::isfinite(val)) continue;

                            if (doClip && pmean && psigma) {
                                float lo = pmean[x] - sigLo * psigma[x];
                                float hi = pmean[x] + sigHi * psigma[x];
                                if (val < lo || val > hi) continue; // reject outlier
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
        cv::parallel_for_(cv::Range(0, h), [&](const cv::Range &range) {
            for (int y = range.start; y < range.end; y++) {
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
    cv::merge(mean, result);
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
