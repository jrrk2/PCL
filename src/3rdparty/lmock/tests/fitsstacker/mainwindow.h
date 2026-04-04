#pragma once

#include <QMainWindow>
#include <QTableWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QProgressBar>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include <QThread>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QStackedWidget>
#include <opencv2/core.hpp>
#include "stackengine.h"

class HistogramWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget *parent = nullptr);
    void setImage(const cv::Mat &image);  // float32, 1 or 3 channel
    void setStretchMarkers(float blackPct, float whitePct);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    std::vector<int> m_histR, m_histG, m_histB;
    int m_maxCount = 1;
    float m_blackPct = 0.1f, m_whitePct = 99.9f;
    float m_dataMin = 0, m_dataMax = 1;
    bool m_hasData = false;
    bool m_mono = false;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPreselect();
    void onAnalyze();
    void onStack();
    void onSave();
    void onDumpGradients();
    void onRefitGradients();
    void onCancel();
    void onProgress(int current, int total, const QString &msg);
    void onFrameAnalyzed(int index, const FrameInfo &info);
    void onAnalysisComplete();
    void onStackComplete(cv::Mat result);
    void onStretchChanged();
    void onQualityThresholdChanged(double val);

private:
    void setupUi();
    void displayImage(const cv::Mat &img);
    void applyZoom();
    void updateHistogram();
    QImage matToQImage(const cv::Mat &mat);
    void updateSelectionSummary();
    void applyQualityFilter();

    // UI
    QTableWidget *m_table;
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QProgressBar *m_progress;
    QLabel *m_statusLabel;
    QLabel *m_selectionLabel;  // "N/M frames selected (X failed alignment)"

    QPushButton *m_btnBrowse, *m_btnPreselect, *m_btnAnalyze, *m_btnRefitGrad, *m_btnStack, *m_btnSave, *m_btnDumpGrad, *m_btnCancel;
    QPushButton *m_btnSelectAll, *m_btnSelectNone, *m_btnSelectInvert;
    QLineEdit *m_fitsPathEdit;

    QDoubleSpinBox *m_sigmaLow, *m_sigmaHigh;
    QSpinBox *m_clipIter, *m_gradDegree, *m_threadCount, *m_maxFrames;
    QSlider *m_qualSlider;
    QLabel *m_qualSliderLabel;
    QDoubleSpinBox *m_solarDepression;
    QCheckBox *m_rayleighCheck;
    QComboBox *m_refFrameCombo;

    QSlider *m_blackSlider, *m_whiteSlider, *m_midSlider;
    QLabel *m_blackLabel, *m_whiteLabel, *m_midLabel;

    QSlider *m_zoomSlider;
    QLabel *m_zoomLabel;
    QComboBox *m_channelCombo;
    QPushButton *m_btnHistogram;
    QStackedWidget *m_viewStack;
    HistogramWidget *m_histogram;

    // Engine
    StackEngine *m_engine;
    QThread *m_thread;

    cv::Mat m_currentImage;   // current display image (float32)
    cv::Mat m_stackResult;    // last stack result

    bool m_analyzed = false;
};
