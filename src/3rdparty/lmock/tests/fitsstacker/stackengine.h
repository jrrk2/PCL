#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <vector>
#include <atomic>
#include <opencv2/core.hpp>

struct StarPos {
    cv::Point2f pos;
    float brightness;
    float fwhm;
};

struct FrameInfo {
    QString path;
    QString dateObs;
    double exptime = 0;
    int width = 0, height = 0, channels = 0;

    // Detected stars (stored from phase 1 for reuse in alignment)
    std::vector<StarPos> stars;

    // Quality
    double fwhm = 0;
    int starCount = 0;
    double backgroundLevel = 0;
    double backgroundNoise = 0;
    double qualityScore = 0;
    double weight = 1.0;

    // Alignment (homography to reference)
    cv::Mat homography;
    int inlierCount = 0;
    bool aligned = false;

    // Per-channel normalization: normalized = scale * (pixel - gradient) + offset
    double scale[3] = {1, 1, 1};
    double offset[3] = {0, 0, 0};

    // Per-channel statistics of gradient-subtracted image (computed in phase 1)
    double flatMedian[3] = {0, 0, 0};
    double flatMAD[3] = {0, 0, 0};

    // Per-channel gradient polynomial coefficients
    std::vector<double> gradientCoeffs[3];

    double solarAltitude = 0;   // degrees (negative = below horizon)
    double solarAzimuth = 0;    // degrees, 0=North, 90=East
    double objAzimuth = 0;      // from OBJCTAZ header
    double objAltitude = 0;     // from OBJCTALT header

    bool hasSatelliteTrail = false;
    bool enabled = true;
};

struct StackParams {
    double sigmaLow = 2.5;
    double sigmaHigh = 2.5;
    int clipIterations = 3;       // total passes (1 = no clipping, 2+ = clip rounds)
    int gradientDegree = 2;       // 0=none, 1=linear, 2=quadratic, 3=cubic
    double qualityThreshold = 0;  // 0-1, reject below this fraction of best
    int referenceFrame = -1;      // -1 = auto (best quality)
    int maxStarsAlign = 500;
    int threadCount = 0;          // 0 = auto (hardware concurrency)
    bool rayleighModel = false;   // Rayleigh scattering sky model
    double minSolarDepression = 0;// 0=off, >0 = require sun this many degrees below horizon
    int gradientKeyInterval = 10; // fit gradient every N frames, interpolate between
};

class StackEngine : public QObject {
    Q_OBJECT
public:
    explicit StackEngine(QObject *parent = nullptr);

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

    // FITS I/O
    static cv::Mat loadFits(const QString &path);
    static bool saveFits(const QString &path, const cv::Mat &image);

    // Dump gradient surfaces for diagnostics
    void dumpGradients(const QString &outDir, const StackParams &params);

    // Preview helpers
    static cv::Mat autoStretch(const cv::Mat &image, double blackPct = 0.1, double whitePct = 99.9);

signals:
    void progress(int current, int total, const QString &msg);
    void frameAnalyzed(int index, const FrameInfo &info);
    void preselectComplete();
    void analysisComplete();
    void gradientsComplete();
    void stackComplete(cv::Mat result);

private:
    // Solar position
    struct SolarPosition { double altitude; double azimuth; };
    static SolarPosition computeSolarPosition(const QString &dateObs, double siteLat, double siteLong);

    // Star detection using DoG
    static std::vector<StarPos> detectStars(const cv::Mat &gray, int maxStars = 500);
    static double measureFWHM(const cv::Mat &gray, cv::Point2f center);
    static cv::Mat toGray(const cv::Mat &color);

    // Alignment — geometric triangle matching
    struct AlignResult { cv::Mat H; int inliers; };
    AlignResult matchStars(const std::vector<StarPos> &frameStars);

    // Background gradient
    static cv::Mat fitGradient(const cv::Mat &channel, const std::vector<StarPos> &stars,
                               int degree, std::vector<double> &coeffs);
    static cv::Mat evalGradient(int rows, int cols, const std::vector<double> &coeffs, int degree);

    // Rayleigh scattering sky model
    static void fitRayleighGradient(const std::vector<cv::Mat> &channels,
                                     const std::vector<StarPos> &stars,
                                     int greyDegree, double sunDirX, double sunDirY,
                                     std::vector<double> coeffsOut[3]);

    // Normalization
    static void computeNormalization(const cv::Mat &refChan, const cv::Mat &frameChan,
                                     const cv::Mat &refGrad, const cv::Mat &frameGrad,
                                     double &scale, double &offset);

    // Warp + normalize a frame for stacking
    cv::Mat prepareFrame(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask);

    std::vector<FrameInfo> m_frames;
    int m_refIndex = -1;
    cv::Mat m_refImage;        // float32 reference
    std::vector<StarPos> m_refStars;

    // Cached reference triangle hash for fast matching
    struct TriRef { int v[3]; double r1, r2; };
    std::unordered_map<uint64_t, std::vector<TriRef>> m_refTriHash;
    void buildRefTriangleHash(int maxStars);

    QMutex m_progressMutex;
    std::atomic<int> m_progressCounter{0};
    std::atomic<bool> m_cancelled{false};
};
