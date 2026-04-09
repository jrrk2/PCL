// stackengine_cli.h — CLI-compatible wrapper for StackEngine
// Strips Q_OBJECT and signals to avoid requiring MOC for command-line use.
// Include this instead of stackengine.h when building without Qt MOC.

#pragma once

// Suppress Q_OBJECT macro and signals section
#define STACKENGINE_CLI_MODE

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <vector>
#include <atomic>
#include <unordered_map>
#include <opencv2/core.hpp>

// ─── Stretch modes ──────────────────────────────────────────────────────────
enum class StretchMode {
    Auto,       // percentile clip + midtone transfer function
    Linear,     // percentile black/white clip, no curve
    Gamma,      // linear + power-law gamma
    Log,        // linear + log2(1+x)
    Sinh        // linear + sinh(3x)/sinh(3)
};

// ─── TAN-projection WCS ─────────────────────────────────────────────────────
struct SimpleTANWCS {
    double crval1 = 0, crval2 = 0;           // reference RA, Dec (degrees)
    double crpix1 = 0, crpix2 = 0;           // reference pixel (1-indexed FITS)
    double cd11 = 0, cd12 = 0, cd21 = 0, cd22 = 0; // CD matrix (deg/pix)
    bool valid = false;

    bool pixelToWorld(double px, double py, double &ra, double &dec) const;
    bool worldToPixel(double ra, double dec, double &px, double &py) const;
    double pixelScale() const;               // arcsec/pixel

    // PascalCase aliases (used by galaxy_processor code)
    bool PixelToWorld(double px, double py, double &ra, double &dec) const { return pixelToWorld(px,py,ra,dec); }
    bool WorldToPixel(double ra, double dec, double &px, double &py) const { return worldToPixel(ra,dec,px,py); }
    double Resolution() const { return std::sqrt(std::abs(cd11*cd22 - cd12*cd21)); }
};

struct StarPos {
    cv::Point2f pos;
    float brightness;
    float fwhm;
    // PSF fit results (populated when psfWeighting is enabled)
    float fwhmX = 0, fwhmY = 0;
    float eccentricity = 0;
    float psfFlux = 0;
    float psfResidual = 0;
};

struct FrameInfo {
    QString path;
    QString dateObs;
    double exptime = 0;
    int width = 0, height = 0, channels = 0;

    std::vector<StarPos> stars;

    double fwhm = 0;
    int starCount = 0;
    double backgroundLevel = 0;
    double backgroundNoise = 0;
    double qualityScore = 0;
    double weight = 1.0;

    cv::Mat homography;
    int inlierCount = 0;
    bool aligned = false;

    double scale[3] = {1, 1, 1};
    double offset[3] = {0, 0, 0};

    double flatMedian[3] = {0, 0, 0};
    double flatMAD[3] = {0, 0, 0};

    std::vector<double> gradientCoeffs[3];

    double solarAltitude = 0;
    double solarAzimuth = 0;
    double objAzimuth = 0;
    double objAltitude = 0;

    // PSF aggregate stats (computed when psfWeighting is enabled)
    double medianEccentricity = 0;
    double totalPSFFlux = 0;

    // Local normalization grids (gridSize x gridSize per channel)
    std::vector<double> localMedian[3];
    std::vector<double> localMAD[3];
    int localGridSize = 0;

    // TPS distortion correction coefficients
    struct TPSCoeffs {
        std::vector<cv::Point2f> ctrlPts;  // source control points
        std::vector<double> wx, wy;         // TPS weights (N each)
        double ax[3] = {}, ay[3] = {};      // affine terms [a0, a1, a2]
        bool valid = false;
    };
    TPSCoeffs tpsCoeffs;

    bool hasSatelliteTrail = false;
    bool enabled = true;

    QString bayerPattern;  // e.g. "RGGB", empty if not Bayer

    // WCS (populated from FITS headers when available)
    SimpleTANWCS wcs;
};

struct StackParams {
    double sigmaLow = 2.5;
    double sigmaHigh = 2.5;
    int clipIterations = 3;
    int gradientDegree = 2;       // 0=none, 1=linear, 2=quadratic, 3=cubic
    double qualityThreshold = 0;
    int referenceFrame = -1;
    int maxStarsAlign = 500;
    int threadCount = 0;
    bool rayleighModel = false;
    double minSolarDepression = 0;
    int gradientKeyInterval = 10;
    bool psfWeighting = false;        // PSF-based subframe weighting
    bool localNormalization = false;  // spatially-varying normalization
    int localNormGridSize = 16;       // NxN grid for local normalization
    bool distortionCorrection = false;// TPS distortion correction
    bool usePCLStarDetector = false;  // use pcl::StarDetector instead of DoG
    bool debayer = true;              // demosaic Bayer frames during stacking
    bool wcsAlignment = false;        // use WCS reprojection instead of homography
    StretchMode stretchMode = StretchMode::Auto;
    double gamma = 2.2;              // gamma value for Gamma stretch mode

    // Calibration frames (master flat, dark, bias)
    QString masterBiasPath;           // master bias/offset frame
    QString masterDarkPath;           // master dark frame
    QString masterFlatPath;           // master flat-field frame
};

// CLI version: plain C++ class, no QObject/MOC dependency
class StackEngine {
public:
    StackEngine();

    void preselectFrames(const QStringList &paths, const StackParams &params);
    void analyzeFrames(const StackParams &params);
    void refitGradients(const StackParams &params);
    cv::Mat stackFrames(const StackParams &params);

    void cancel() { m_cancelled = true; }
    bool isCancelled() const { return m_cancelled; }
    void resetCancel() { m_cancelled = false; }

    const std::vector<FrameInfo> &frames() const { return m_frames; }
    std::vector<FrameInfo> &frames() { return m_frames; }
    int referenceIndex() const { return m_refIndex; }

    static cv::Mat loadFits(const QString &path);
    static bool saveFits(const QString &path, const cv::Mat &image);

    void dumpGradients(const QString &outDir, const StackParams &params);

    // Stretch helpers
    static cv::Mat autoStretch(const cv::Mat &image, double blackPct = 0.1, double whitePct = 99.9);
    static cv::Mat stretch(const cv::Mat &image, StretchMode mode,
                           double blackPct = 0.1, double whitePct = 99.9,
                           double gamma = 2.2);

    // CFA-aware 2x2 binning (for plate solving at reduced resolution)
    static cv::Mat cfaBin2x2(const cv::Mat &bayer);

    // Public for CLI use (gradient fitting on stacked result)
    static cv::Mat fitGradient(const cv::Mat &channel, const std::vector<StarPos> &stars,
                               int degree, std::vector<double> &coeffs);
    static cv::Mat fitGradient(const cv::Mat &channel, const std::vector<StarPos> &stars,
                               int degree, std::vector<double> &coeffs,
                               const cv::Mat &signalMask);
    static cv::Mat evalGradient(int rows, int cols, const std::vector<double> &coeffs, int degree);

    // Signal stubs (no-ops in CLI mode, original class emits Qt signals)
    void progress(int, int, const QString &) {}
    void frameAnalyzed(int, const FrameInfo &) {}
    void preselectComplete() {}
    void analysisComplete() {}
    void gradientsComplete() {}
    void stackComplete(cv::Mat) {}

private:
    struct SolarPosition { double altitude; double azimuth; };
    static SolarPosition computeSolarPosition(const QString &dateObs, double siteLat, double siteLong);

    static std::vector<StarPos> detectStars(const cv::Mat &gray, int maxStars = 500);
    static std::vector<StarPos> detectStarsPCL(const cv::Mat &gray, int maxStars = 500);
    static double measureFWHM(const cv::Mat &gray, cv::Point2f center);
    static cv::Mat toGray(const cv::Mat &color);

    struct AlignResult {
        cv::Mat H;
        int inliers;
        std::vector<cv::Point2f> srcPts, dstPts;
    };
    AlignResult matchStars(const std::vector<StarPos> &frameStars);

    // PSF fitting (2D elliptical Gaussian via Levenberg-Marquardt)
    static bool fitPSF(const cv::Mat &gray, cv::Point2f center,
                       float &A, float &x0, float &y0,
                       float &sigmaX, float &sigmaY, float &B,
                       float &residual);

    // TPS distortion correction
    static FrameInfo::TPSCoeffs fitTPS(const std::vector<cv::Point2f> &srcPts,
                                        const std::vector<cv::Point2f> &dstPts);
    static void generateTPSRemapMaps(const FrameInfo::TPSCoeffs &tps, int rows, int cols,
                                      cv::Mat &mapX, cv::Mat &mapY);

    static void fitRayleighGradient(const std::vector<cv::Mat> &channels,
                                     const std::vector<StarPos> &stars,
                                     int greyDegree, double sunDirX, double sunDirY,
                                     std::vector<double> coeffsOut[3]);

    static void computeNormalization(const cv::Mat &refChan, const cv::Mat &frameChan,
                                     const cv::Mat &refGrad, const cv::Mat &frameGrad,
                                     double &scale, double &offset);

    cv::Mat prepareFrame(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask);

    // WCS reprojection (alternative to homography warp)
    cv::Mat prepareFrameWCS(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask);
    static SimpleTANWCS readWCSFromFITS(const QString &path);

    // Bayer drizzle: resample raw CFA data directly into R/G/B channels
    // without demosaicing — each raw pixel goes to its true colour channel.
    // Returns 3-channel float image; mask is 3-channel (per-channel validity).
    cv::Mat prepareFrameBayerDrizzle(const FrameInfo &fi, const cv::Mat &raw,
                                     cv::Mat &mask3);

    // Calibration: apply bias/dark/flat correction to a raw frame
    void loadCalibrationFrames(const StackParams &params);
    cv::Mat calibrateFrame(const cv::Mat &raw) const;

    std::vector<FrameInfo> m_frames;
    int m_refIndex = -1;
    cv::Mat m_refImage;
    std::vector<StarPos> m_refStars;
    bool m_debayer = true;

    // Master calibration frames (loaded once, reused for all lights)
    cv::Mat m_masterBias;
    cv::Mat m_masterDark;
    cv::Mat m_masterFlat;  // normalized so median ≈ 1.0

    struct TriRef { int v[3]; double r1, r2; };
    std::unordered_map<uint64_t, std::vector<TriRef>> m_refTriHash;
    void buildRefTriangleHash(int maxStars);

    QMutex m_progressMutex;
    std::atomic<int> m_progressCounter{0};
    std::atomic<bool> m_cancelled{false};
};
