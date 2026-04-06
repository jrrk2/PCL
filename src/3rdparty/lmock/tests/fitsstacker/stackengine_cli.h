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

    bool hasSatelliteTrail = false;
    bool enabled = true;
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

    static cv::Mat autoStretch(const cv::Mat &image, double blackPct = 0.1, double whitePct = 99.9);

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
    static double measureFWHM(const cv::Mat &gray, cv::Point2f center);
    static cv::Mat toGray(const cv::Mat &color);

    struct AlignResult { cv::Mat H; int inliers; };
    AlignResult matchStars(const std::vector<StarPos> &frameStars);

    static void fitRayleighGradient(const std::vector<cv::Mat> &channels,
                                     const std::vector<StarPos> &stars,
                                     int greyDegree, double sunDirX, double sunDirY,
                                     std::vector<double> coeffsOut[3]);

    static void computeNormalization(const cv::Mat &refChan, const cv::Mat &frameChan,
                                     const cv::Mat &refGrad, const cv::Mat &frameGrad,
                                     double &scale, double &offset);

    cv::Mat prepareFrame(const FrameInfo &fi, const cv::Mat &raw, cv::Mat &mask);

    std::vector<FrameInfo> m_frames;
    int m_refIndex = -1;
    cv::Mat m_refImage;
    std::vector<StarPos> m_refStars;

    struct TriRef { int v[3]; double r1, r2; };
    std::unordered_map<uint64_t, std::vector<TriRef>> m_refTriHash;
    void buildRefTriangleHash(int maxStars);

    QMutex m_progressMutex;
    std::atomic<int> m_progressCounter{0};
    std::atomic<bool> m_cancelled{false};
};
