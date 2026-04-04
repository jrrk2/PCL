#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QFormLayout>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QDir>
#include <QApplication>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QPainterPath>
#include <QtConcurrent>
#include <algorithm>
#include <thread>
#include <opencv2/imgproc.hpp>

// ─── HistogramWidget ────────────────────────────────────────────────────────

HistogramWidget::HistogramWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void HistogramWidget::setImage(const cv::Mat &image)
{
    const int nBins = 1024;
    m_histR.assign(nBins, 0);
    m_histG.assign(nBins, 0);
    m_histB.assign(nBins, 0);
    m_hasData = false;
    if (image.empty()) { update(); return; }

    // Find data range (subsampled)
    float dmin = 1e30f, dmax = -1e30f;
    for (int y = 0; y < image.rows; y += 4) {
        const float *p = image.ptr<float>(y);
        int stride = image.channels();
        for (int x = 0; x < image.cols; x += 4) {
            for (int c = 0; c < stride; c++) {
                float v = p[x * stride + c];
                if (std::isfinite(v)) {
                    dmin = std::min(dmin, v);
                    dmax = std::max(dmax, v);
                }
            }
        }
    }
    if (dmax <= dmin) dmax = dmin + 1;
    m_dataMin = dmin;
    m_dataMax = dmax;
    m_mono = (image.channels() == 1);

    float scale = (nBins - 1) / (dmax - dmin);
    for (int y = 0; y < image.rows; y += 2) {
        const float *p = image.ptr<float>(y);
        if (image.channels() == 3) {
            for (int x = 0; x < image.cols; x += 2) {
                for (int c = 0; c < 3; c++) {
                    float v = p[x * 3 + c];
                    if (!std::isfinite(v)) continue;
                    int bin = std::clamp((int)((v - dmin) * scale), 0, nBins - 1);
                    if (c == 0) m_histR[bin]++;
                    else if (c == 1) m_histG[bin]++;
                    else m_histB[bin]++;
                }
            }
        } else {
            for (int x = 0; x < image.cols; x += 2) {
                float v = p[x];
                if (!std::isfinite(v)) continue;
                int bin = std::clamp((int)((v - dmin) * scale), 0, nBins - 1);
                m_histR[bin]++;
            }
        }
    }

    m_maxCount = 1;
    for (int i = 0; i < nBins; i++) {
        m_maxCount = std::max(m_maxCount, m_histR[i]);
        m_maxCount = std::max(m_maxCount, m_histG[i]);
        m_maxCount = std::max(m_maxCount, m_histB[i]);
    }
    m_hasData = true;
    update();
}

void HistogramWidget::setStretchMarkers(float blackPct, float whitePct)
{
    m_blackPct = blackPct;
    m_whitePct = whitePct;
    update();
}

void HistogramWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(30, 30, 30));
    if (!m_hasData || m_histR.empty()) return;

    int nBins = m_histR.size();
    int w = width() - 20;
    int h = height() - 30;
    int x0 = 10, y0 = 10;
    double logMax = std::log(m_maxCount + 1);

    auto drawHist = [&](const std::vector<int> &hist, QColor color) {
        QPainterPath path;
        path.moveTo(x0, y0 + h);
        for (int i = 0; i < nBins; i++) {
            double xp = x0 + (double)i / nBins * w;
            double yp = y0 + h - (hist[i] > 0 ? std::log(hist[i] + 1) / logMax * h : 0);
            if (i == 0) path.moveTo(xp, yp);
            else path.lineTo(xp, yp);
        }
        p.setPen(QPen(color, 1.2));
        p.setBrush(Qt::NoBrush);
        p.drawPath(path);
    };

    if (m_mono) {
        drawHist(m_histR, QColor(200, 200, 200));
    } else {
        drawHist(m_histR, QColor(255, 80, 80));
        drawHist(m_histG, QColor(80, 255, 80));
        drawHist(m_histB, QColor(80, 120, 255));
    }

    // Draw black/white markers as vertical lines
    // Compute marker positions from percentile of cumulated data
    auto markerX = [&](float pct) -> double {
        // pct is a percentile (0-100) — find the corresponding bin
        long total = 0;
        for (int i = 0; i < nBins; i++)
            total += m_histR[i] + (m_mono ? 0 : m_histG[i] + m_histB[i]);
        long target = (long)(total * pct / 100.0);
        long cum = 0;
        for (int i = 0; i < nBins; i++) {
            cum += m_histR[i] + (m_mono ? 0 : m_histG[i] + m_histB[i]);
            if (cum >= target)
                return x0 + (double)i / nBins * w;
        }
        return x0 + w;
    };

    p.setPen(QPen(QColor(255, 255, 0, 180), 1, Qt::DashLine));
    double bx = markerX(m_blackPct);
    p.drawLine(QPointF(bx, y0), QPointF(bx, y0 + h));
    double wx = markerX(m_whitePct);
    p.drawLine(QPointF(wx, y0), QPointF(wx, y0 + h));

    // Labels
    p.setPen(QColor(180, 180, 180));
    p.setFont(QFont("sans-serif", 8));
    p.drawText(QPointF(bx - 5, y0 + h + 14), QString("B:%1%").arg(m_blackPct, 0, 'f', 1));
    p.drawText(QPointF(wx - 5, y0 + h + 14), QString("W:%1%").arg(m_whitePct, 0, 'f', 1));

    // Data range
    p.drawText(QPointF(x0, y0 + h + 14), QString::number(m_dataMin, 'f', 0));
    p.drawText(QPointF(x0 + w - 40, y0 + h + 14), QString::number(m_dataMax, 'f', 0));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_engine = new StackEngine();
    m_thread = new QThread(this);
    m_engine->moveToThread(m_thread);

    connect(m_engine, &StackEngine::progress, this, &MainWindow::onProgress, Qt::QueuedConnection);
    connect(m_engine, &StackEngine::frameAnalyzed, this, &MainWindow::onFrameAnalyzed, Qt::QueuedConnection);
    connect(m_engine, &StackEngine::preselectComplete, this, [this]() {
        m_btnPreselect->setEnabled(true);
        m_btnAnalyze->setEnabled(true);
        m_btnCancel->setEnabled(false);
        m_table->setSortingEnabled(true);
        updateSelectionSummary();
        m_statusLabel->setText(QString("Preselect complete. Adjust selection, then click Analyze."));
    }, Qt::QueuedConnection);
    connect(m_engine, &StackEngine::analysisComplete, this, &MainWindow::onAnalysisComplete, Qt::QueuedConnection);
    connect(m_engine, &StackEngine::gradientsComplete, this, [this]() {
        m_btnRefitGrad->setEnabled(true);
        m_btnStack->setEnabled(true);
        m_btnDumpGrad->setEnabled(true);
        m_btnCancel->setEnabled(false);
        m_statusLabel->setText("Gradient refitting complete.");
    }, Qt::QueuedConnection);
    connect(m_engine, &StackEngine::stackComplete, this, &MainWindow::onStackComplete, Qt::QueuedConnection);

    m_thread->start();

    setupUi();
    resize(1400, 900);
    setWindowTitle("FITS Stacker — M101");
}

MainWindow::~MainWindow()
{
    m_engine->cancel();
    m_thread->quit();
    m_thread->wait();
    delete m_engine;
}

void MainWindow::setupUi()
{
    auto *central = new QWidget;
    setCentralWidget(central);
    auto *mainLayout = new QVBoxLayout(central);

    // Toolbar
    auto *toolbar = new QToolBar;
    m_fitsPathEdit = new QLineEdit;
    m_fitsPathEdit->setPlaceholderText("Path to FITS directory...");
    m_fitsPathEdit->setMinimumWidth(300);
    QDir defaultDir(QDir::currentPath() + "/../FITS");
    if (defaultDir.exists()) m_fitsPathEdit->setText(defaultDir.absolutePath());
    m_btnBrowse = new QPushButton("Browse...");
    m_btnPreselect = new QPushButton("Preselect");
    m_btnAnalyze = new QPushButton("Analyze");
    m_btnRefitGrad = new QPushButton("Refit Gradients");
    m_btnStack = new QPushButton("Stack");
    m_btnSave = new QPushButton("Save FITS");
    m_btnDumpGrad = new QPushButton("Dump Gradients");
    m_btnCancel = new QPushButton("Cancel");
    m_btnAnalyze->setEnabled(false);
    m_btnRefitGrad->setEnabled(false);
    m_btnRefitGrad->setToolTip("Re-run gradient fitting + normalization without redoing star detection/alignment");
    m_btnStack->setEnabled(false);
    m_btnSave->setEnabled(false);
    m_btnDumpGrad->setEnabled(false);
    m_btnDumpGrad->setToolTip("Save per-frame gradient surfaces as FITS files for diagnostics");
    m_btnCancel->setEnabled(false);
    toolbar->addWidget(new QLabel(" FITS Dir: "));
    toolbar->addWidget(m_fitsPathEdit);
    toolbar->addWidget(m_btnBrowse);
    toolbar->addSeparator();
    toolbar->addWidget(m_btnPreselect);
    toolbar->addWidget(m_btnAnalyze);
    toolbar->addWidget(m_btnRefitGrad);
    toolbar->addWidget(m_btnStack);
    toolbar->addWidget(m_btnSave);
    toolbar->addWidget(m_btnDumpGrad);
    toolbar->addSeparator();
    toolbar->addWidget(m_btnCancel);
    addToolBar(toolbar);

    connect(m_btnBrowse, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select FITS Directory",
                                                         m_fitsPathEdit->text());
        if (!dir.isEmpty()) m_fitsPathEdit->setText(dir);
    });
    connect(m_btnPreselect, &QPushButton::clicked, this, &MainWindow::onPreselect);
    connect(m_btnAnalyze, &QPushButton::clicked, this, &MainWindow::onAnalyze);
    connect(m_btnStack, &QPushButton::clicked, this, &MainWindow::onStack);
    connect(m_btnSave, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(m_btnRefitGrad, &QPushButton::clicked, this, &MainWindow::onRefitGradients);
    connect(m_btnDumpGrad, &QPushButton::clicked, this, &MainWindow::onDumpGradients);
    connect(m_btnCancel, &QPushButton::clicked, this, &MainWindow::onCancel);

    // Splitter: left=table, right=preview
    auto *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter, 1);

    // Left panel: table + selection controls
    auto *leftPanel = new QWidget;
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    m_table = new QTableWidget;
    m_table->setColumnCount(10);
    m_table->setHorizontalHeaderLabels({"On", "File", "Date", "Stars", "FWHM", "Inliers", "Quality", "Weight", "Sun Alt", "Trail"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setSortingEnabled(true);
    m_table->setColumnWidth(0, 30);
    m_table->setColumnWidth(1, 180);
    m_table->setColumnWidth(2, 150);
    m_table->setColumnWidth(3, 50);
    m_table->setColumnWidth(4, 60);
    m_table->setColumnWidth(5, 60);
    m_table->setColumnWidth(6, 60);
    m_table->setColumnWidth(7, 60);
    m_table->setColumnWidth(8, 60);
    leftLayout->addWidget(m_table, 1);

    // Quality filter slider — prominent, above selection buttons
    auto *qualBar = new QHBoxLayout;
    qualBar->addWidget(new QLabel("Quality filter:"));
    m_qualSlider = new QSlider(Qt::Horizontal);
    m_qualSlider->setRange(0, 100);
    m_qualSlider->setValue(0);
    m_qualSlider->setTickPosition(QSlider::TicksBelow);
    m_qualSlider->setTickInterval(10);
    m_qualSliderLabel = new QLabel("0%  (all frames)");
    m_qualSliderLabel->setMinimumWidth(120);
    qualBar->addWidget(m_qualSlider, 1);
    qualBar->addWidget(m_qualSliderLabel);
    leftLayout->addLayout(qualBar);

    connect(m_qualSlider, &QSlider::valueChanged, this, [this](int val) {
        onQualityThresholdChanged(val / 100.0);
    });

    // Solar depression filter
    auto *solarBar = new QHBoxLayout;
    solarBar->addWidget(new QLabel("Min sun depression:"));
    m_solarDepression = new QDoubleSpinBox;
    m_solarDepression->setRange(0, 30);
    m_solarDepression->setValue(0);
    m_solarDepression->setSingleStep(1);
    m_solarDepression->setSuffix(QChar(0x00B0));  // degree symbol
    m_solarDepression->setSpecialValueText("Off");
    m_solarDepression->setToolTip("Minimum solar depression below horizon.\nCivil twilight: 6\u00b0, Nautical: 12\u00b0, Astronomical: 18\u00b0");
    solarBar->addWidget(m_solarDepression);
    auto *solarHelp = new QLabel("(Civil=6\u00b0 Nautical=12\u00b0 Astro=18\u00b0)");
    solarHelp->setStyleSheet("color: gray; font-size: 10px;");
    solarBar->addWidget(solarHelp);
    auto *btnSolarApply = new QPushButton("Apply");
    btnSolarApply->setMaximumWidth(60);
    solarBar->addWidget(btnSolarApply);
    solarBar->addStretch();
    leftLayout->addLayout(solarBar);

    connect(btnSolarApply, &QPushButton::clicked, this, [this]() {
        if (m_analyzed) applyQualityFilter();
    });

    // Selection controls below quality slider
    auto *selBar = new QHBoxLayout;
    m_btnSelectAll = new QPushButton("All");
    m_btnSelectNone = new QPushButton("None");
    m_btnSelectInvert = new QPushButton("Invert");
    m_selectionLabel = new QLabel("0/0 frames selected");
    m_btnSelectAll->setMaximumWidth(60);
    m_btnSelectNone->setMaximumWidth(60);
    m_btnSelectInvert->setMaximumWidth(60);
    auto *btnHideUnsel = new QPushButton("Hide unsel.");
    btnHideUnsel->setCheckable(true);
    btnHideUnsel->setMaximumWidth(80);
    selBar->addWidget(new QLabel("Select:"));
    selBar->addWidget(m_btnSelectAll);
    selBar->addWidget(m_btnSelectNone);
    selBar->addWidget(m_btnSelectInvert);
    selBar->addWidget(btnHideUnsel);
    selBar->addStretch();
    selBar->addWidget(m_selectionLabel);
    leftLayout->addLayout(selBar);

    connect(m_btnSelectAll, &QPushButton::clicked, this, [this]() {
        auto &frames = m_engine->frames();
        for (int i = 0; i < m_table->rowCount(); i++) {
            bool canEnable = (i < (int)frames.size()) ? frames[i].aligned : true;
            m_table->item(i, 0)->setCheckState(canEnable ? Qt::Checked : Qt::Unchecked);
        }
        updateSelectionSummary();
    });
    connect(m_btnSelectNone, &QPushButton::clicked, this, [this]() {
        for (int i = 0; i < m_table->rowCount(); i++)
            m_table->item(i, 0)->setCheckState(Qt::Unchecked);
        updateSelectionSummary();
    });
    connect(m_btnSelectInvert, &QPushButton::clicked, this, [this]() {
        for (int i = 0; i < m_table->rowCount(); i++) {
            auto *item = m_table->item(i, 0);
            item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        }
        updateSelectionSummary();
    });
    connect(btnHideUnsel, &QPushButton::toggled, this, [this](bool hide) {
        for (int i = 0; i < m_table->rowCount(); i++) {
            bool checked = m_table->item(i, 0)->checkState() == Qt::Checked;
            m_table->setRowHidden(i, hide && !checked);
        }
    });

    splitter->addWidget(leftPanel);

    // Right panel: preview + stretch controls
    auto *rightPanel = new QWidget;
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);

    m_histogram = new HistogramWidget;

    m_viewStack = new QStackedWidget;
    m_viewStack->addWidget(m_view);       // index 0 = image
    m_viewStack->addWidget(m_histogram);  // index 1 = histogram
    rightLayout->addWidget(m_viewStack, 1);

    // Zoom + channel + histogram bar
    auto *viewBar = new QHBoxLayout;
    viewBar->addWidget(new QLabel("Zoom:"));
    m_zoomSlider = new QSlider(Qt::Horizontal);
    m_zoomSlider->setRange(0, 100);  // 0=fit, 100=1:1
    m_zoomSlider->setValue(0);
    m_zoomSlider->setMaximumWidth(200);
    m_zoomLabel = new QLabel("Fit");
    m_zoomLabel->setMinimumWidth(40);
    viewBar->addWidget(m_zoomSlider);
    viewBar->addWidget(m_zoomLabel);

    viewBar->addSpacing(20);
    viewBar->addWidget(new QLabel("Channel:"));
    m_channelCombo = new QComboBox;
    m_channelCombo->addItems({"RGB", "Red", "Green", "Blue"});
    m_channelCombo->setMaximumWidth(80);
    viewBar->addWidget(m_channelCombo);

    viewBar->addSpacing(20);
    m_btnHistogram = new QPushButton("Histogram");
    m_btnHistogram->setCheckable(true);
    m_btnHistogram->setMaximumWidth(100);
    viewBar->addWidget(m_btnHistogram);

    viewBar->addStretch();
    rightLayout->addLayout(viewBar);

    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int val) {
        if (val == 0) {
            m_zoomLabel->setText("Fit");
        } else {
            int pct = val;
            m_zoomLabel->setText(QString("%1%").arg(pct));
        }
        applyZoom();
    });
    connect(m_channelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { onStretchChanged(); });
    connect(m_btnHistogram, &QPushButton::toggled, this, [this](bool on) {
        m_viewStack->setCurrentIndex(on ? 1 : 0);
        if (on) updateHistogram();
    });

    // Stretch controls
    auto *stretchGroup = new QGroupBox("Display Stretch");
    auto *stretchLayout = new QFormLayout(stretchGroup);

    m_blackSlider = new QSlider(Qt::Horizontal);
    m_blackSlider->setRange(0, 1000);
    m_blackSlider->setValue(1);
    m_blackLabel = new QLabel("0.1%");
    auto *bh = new QHBoxLayout;
    bh->addWidget(m_blackSlider);
    bh->addWidget(m_blackLabel);
    stretchLayout->addRow("Black:", bh);

    m_whiteSlider = new QSlider(Qt::Horizontal);
    m_whiteSlider->setRange(0, 1000);
    m_whiteSlider->setValue(999);
    m_whiteLabel = new QLabel("99.9%");
    auto *wh = new QHBoxLayout;
    wh->addWidget(m_whiteSlider);
    wh->addWidget(m_whiteLabel);
    stretchLayout->addRow("White:", wh);

    m_midSlider = new QSlider(Qt::Horizontal);
    m_midSlider->setRange(1, 99);
    m_midSlider->setValue(25);
    m_midLabel = new QLabel("0.25");
    auto *mh = new QHBoxLayout;
    mh->addWidget(m_midSlider);
    mh->addWidget(m_midLabel);
    stretchLayout->addRow("Midtone:", mh);

    connect(m_blackSlider, &QSlider::valueChanged, this, &MainWindow::onStretchChanged);
    connect(m_whiteSlider, &QSlider::valueChanged, this, &MainWindow::onStretchChanged);
    connect(m_midSlider, &QSlider::valueChanged, this, &MainWindow::onStretchChanged);

    rightLayout->addWidget(stretchGroup);
    splitter->addWidget(rightPanel);
    splitter->setSizes({400, 1000});

    // Parameters
    auto *paramGroup = new QGroupBox("Stacking Parameters");
    auto *paramLayout = new QHBoxLayout(paramGroup);

    auto *form1 = new QFormLayout;
    m_sigmaLow = new QDoubleSpinBox;
    m_sigmaLow->setRange(0.5, 10);
    m_sigmaLow->setValue(2.5);
    m_sigmaLow->setSingleStep(0.1);
    form1->addRow("Sigma Low:", m_sigmaLow);

    m_sigmaHigh = new QDoubleSpinBox;
    m_sigmaHigh->setRange(0.5, 10);
    m_sigmaHigh->setValue(2.5);
    m_sigmaHigh->setSingleStep(0.1);
    form1->addRow("Sigma High:", m_sigmaHigh);
    paramLayout->addLayout(form1);

    auto *form2 = new QFormLayout;
    m_clipIter = new QSpinBox;
    m_clipIter->setRange(1, 5);
    m_clipIter->setValue(3);
    form2->addRow("Clip Passes:", m_clipIter);

    m_gradDegree = new QSpinBox;
    m_gradDegree->setRange(0, 4);
    m_gradDegree->setValue(2);
    form2->addRow("Gradient Deg:", m_gradDegree);

    m_rayleighCheck = new QCheckBox("Rayleigh sky model");
    m_rayleighCheck->setToolTip(
        "Constrain sky gradient removal using Rayleigh scattering physics.\n"
        "Models twilight glow as wavelength-dependent (\u03bb\u207b\u2074) gradient\n"
        "toward the sun, plus an achromatic residual polynomial.\n"
        "Uses solar position computed from DATE-OBS and site coordinates.\n"
        "7 parameters (degree 2) vs 18 for independent per-channel fit.");
    form2->addRow("", m_rayleighCheck);
    paramLayout->addLayout(form2);

    auto *form3 = new QFormLayout;
    m_refFrameCombo = new QComboBox;
    m_refFrameCombo->addItem("Auto (best quality)");
    form3->addRow("Reference:", m_refFrameCombo);

    m_threadCount = new QSpinBox;
    m_threadCount->setRange(0, (int)std::thread::hardware_concurrency());
    m_threadCount->setValue(0);
    m_threadCount->setSpecialValueText("Auto");
    form3->addRow("Threads:", m_threadCount);

    m_maxFrames = new QSpinBox;
    m_maxFrames->setRange(0, 99999);
    m_maxFrames->setValue(0);
    m_maxFrames->setSpecialValueText("All");
    form3->addRow("Max Frames:", m_maxFrames);
    paramLayout->addLayout(form3);

    mainLayout->addWidget(paramGroup);

    // Progress
    auto *statusLayout = new QHBoxLayout;
    m_progress = new QProgressBar;
    m_statusLabel = new QLabel("Ready");
    statusLayout->addWidget(m_progress, 1);
    statusLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(statusLayout);
}

// ─── Actions ────────────────────────────────────────────────────────────────

QStringList MainWindow_getFiles(MainWindow *w, QLineEdit *edit, QSpinBox *maxFrames)
{
    QString fitsPath = edit->text().trimmed();
    if (fitsPath.isEmpty()) {
        fitsPath = QFileDialog::getExistingDirectory(w, "Select FITS Directory");
        if (fitsPath.isEmpty()) return {};
        edit->setText(fitsPath);
    }
    QDir dir(fitsPath);
    if (!dir.exists()) {
        QMessageBox::warning(w, "Error", "Directory does not exist: " + fitsPath);
        return {};
    }

    QStringList filters = {"*.fits", "*.fit", "*.fts", "*.FITS"};
    QStringList files;
    for (auto &f : dir.entryList(filters, QDir::Files, QDir::Name))
        files.append(dir.absoluteFilePath(f));

    if (files.isEmpty()) {
        QMessageBox::warning(w, "Error", "No FITS files found");
        return {};
    }

    int max = maxFrames->value();
    if (max > 0 && files.size() > max)
        files = files.mid(0, max);

    return files;
}

void MainWindow::onPreselect()
{
    QStringList files = MainWindow_getFiles(this, m_fitsPathEdit, m_maxFrames);
    if (files.isEmpty()) return;

    m_table->setSortingEnabled(false);
    m_table->setRowCount(files.size());
    for (int i = 0; i < files.size(); i++) {
        auto *chk = new QTableWidgetItem();
        chk->setCheckState(Qt::Checked);
        chk->setFlags(chk->flags() | Qt::ItemIsUserCheckable);
        m_table->setItem(i, 0, chk);
        m_table->setItem(i, 1, new QTableWidgetItem(QFileInfo(files[i]).fileName()));
        for (int c = 2; c < 10; c++)
            m_table->setItem(i, c, new QTableWidgetItem("..."));
    }

    m_refFrameCombo->clear();
    m_refFrameCombo->addItem("Auto (best quality)");
    for (int i = 0; i < files.size(); i++)
        m_refFrameCombo->addItem(QFileInfo(files[i]).fileName());

    m_btnPreselect->setEnabled(false);
    m_btnAnalyze->setEnabled(false);
    m_btnStack->setEnabled(false);
    m_btnCancel->setEnabled(true);
    m_analyzed = false;
    m_selectionLabel->setText(QString("%1 frames, reading headers...").arg(files.size()));

    StackParams params;
    params.minSolarDepression = m_solarDepression->value();

    QMetaObject::invokeMethod(m_engine, [this, files, params]() {
        m_engine->resetCancel();
        m_engine->preselectFrames(files, params);
    }, Qt::QueuedConnection);
}

void MainWindow::onAnalyze()
{
    if (m_engine->frames().empty()) {
        QMessageBox::warning(this, "Error", "Run Preselect first to load frames.");
        return;
    }

    // Sync enabled flags from checkboxes
    auto &frames = m_engine->frames();
    for (int i = 0; i < (int)frames.size() && i < m_table->rowCount(); i++) {
        auto *item = m_table->item(i, 0);
        if (item) frames[i].enabled = (item->checkState() == Qt::Checked);
    }

    int activeCount = 0;
    for (auto &f : frames) if (f.enabled) activeCount++;
    if (activeCount == 0) {
        QMessageBox::warning(this, "Error", "No frames enabled. Adjust selection and try again.");
        return;
    }

    m_btnPreselect->setEnabled(false);
    m_btnAnalyze->setEnabled(false);
    m_btnStack->setEnabled(false);
    m_btnCancel->setEnabled(true);
    m_analyzed = false;
    m_selectionLabel->setText(QString("Analyzing %1 frames...").arg(activeCount));

    StackParams params;
    params.gradientDegree = m_gradDegree->value();
    params.maxStarsAlign = 500;
    params.referenceFrame = m_refFrameCombo->currentIndex() - 1;
    params.threadCount = m_threadCount->value();
    params.rayleighModel = m_rayleighCheck->isChecked();

    QMetaObject::invokeMethod(m_engine, [this, params]() {
        m_engine->resetCancel();
        m_engine->analyzeFrames(params);
    }, Qt::QueuedConnection);
}

void MainWindow::onStack()
{
    if (!m_analyzed) return;

    // Sync enabled flags from checkboxes
    auto &frames = m_engine->frames();
    for (int i = 0; i < (int)frames.size() && i < m_table->rowCount(); i++) {
        auto *item = m_table->item(i, 0);
        if (item) frames[i].enabled = (item->checkState() == Qt::Checked);
    }

    // Count selected
    int selected = 0;
    for (auto &f : frames)
        if (f.enabled && f.aligned) selected++;
    if (selected == 0) {
        QMessageBox::warning(this, "Nothing to stack", "No frames are selected and aligned.");
        return;
    }

    m_btnStack->setEnabled(false);
    m_btnCancel->setEnabled(true);
    m_statusLabel->setText(QString("Stacking %1 frames...").arg(selected));

    StackParams params;
    params.sigmaLow = m_sigmaLow->value();
    params.sigmaHigh = m_sigmaHigh->value();
    params.clipIterations = m_clipIter->value();
    params.gradientDegree = m_gradDegree->value();
    params.qualityThreshold = m_qualSlider->value() / 100.0;

    QMetaObject::invokeMethod(m_engine, [this, params]() {
        m_engine->resetCancel();
        m_engine->stackFrames(params);
    }, Qt::QueuedConnection);
}

void MainWindow::onSave()
{
    if (m_stackResult.empty()) return;

    QString path = QFileDialog::getSaveFileName(this, "Save Stacked FITS",
                                                 "stacked_M101.fits", "FITS (*.fits)");
    if (path.isEmpty()) return;

    if (StackEngine::saveFits(path, m_stackResult))
        m_statusLabel->setText("Saved: " + path);
    else
        QMessageBox::warning(this, "Error", "Failed to save FITS");
}

void MainWindow::onDumpGradients()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory for Gradients",
                                                     m_fitsPathEdit->text());
    if (dir.isEmpty()) return;

    StackParams params;
    params.gradientDegree = m_gradDegree->value();
    m_btnDumpGrad->setEnabled(false);
    m_statusLabel->setText("Dumping gradient surfaces...");

    // Run on worker thread
    (void)QtConcurrent::run([this, dir, params]() {
        m_engine->dumpGradients(dir, params);
        QMetaObject::invokeMethod(this, [this]() {
            m_btnDumpGrad->setEnabled(true);
            m_statusLabel->setText("Gradient dump complete.");
        }, Qt::QueuedConnection);
    });
}

void MainWindow::onRefitGradients()
{
    if (!m_analyzed) return;

    m_btnRefitGrad->setEnabled(false);
    m_btnStack->setEnabled(false);
    m_btnCancel->setEnabled(true);
    m_statusLabel->setText("Refitting gradients...");

    StackParams params;
    params.gradientDegree = m_gradDegree->value();
    params.rayleighModel = m_rayleighCheck->isChecked();
    params.gradientKeyInterval = 10;

    QMetaObject::invokeMethod(m_engine, [this, params]() {
        m_engine->resetCancel();
        m_engine->refitGradients(params);
    }, Qt::QueuedConnection);
}

void MainWindow::onCancel()
{
    m_engine->cancel();
    m_statusLabel->setText("Cancelling...");
}

void MainWindow::onProgress(int current, int total, const QString &msg)
{
    m_progress->setRange(0, total);
    m_progress->setValue(current);
    m_statusLabel->setText(msg);
}

void MainWindow::onFrameAnalyzed(int index, const FrameInfo &info)
{
    if (index >= m_table->rowCount()) return;

    m_table->item(index, 0)->setCheckState(info.enabled ? Qt::Checked : Qt::Unchecked);
    m_table->item(index, 2)->setText(info.dateObs.left(23));
    m_table->item(index, 3)->setText(QString::number(info.starCount));
    m_table->item(index, 4)->setText(QString::number(info.fwhm, 'f', 1));
    m_table->item(index, 5)->setText(QString::number(info.inlierCount));
    m_table->item(index, 6)->setText(QString::number(info.qualityScore, 'f', 3));
    m_table->item(index, 7)->setText(QString::number(info.weight, 'f', 3));
    m_table->item(index, 8)->setText(QString::number(info.solarAltitude, 'f', 1));
    m_table->item(index, 9)->setText(info.hasSatelliteTrail ? "YES" : "");

    // Color code by quality
    QColor bg;
    if (info.hasSatelliteTrail)
        bg = QColor(255, 160, 255); // purple for trails
    else if (!info.enabled || !info.aligned)
        bg = QColor(255, 180, 180);
    else if (info.qualityScore > 0.7)
        bg = QColor(180, 255, 180);
    else if (info.qualityScore > 0.3)
        bg = QColor(255, 255, 200);
    else
        bg = QColor(255, 220, 180);

    for (int c = 0; c < 10; c++)
        if (m_table->item(index, c))
            m_table->item(index, c)->setBackground(bg);
}

void MainWindow::onAnalysisComplete()
{
    m_analyzed = true;
    m_btnPreselect->setEnabled(true);
    m_btnAnalyze->setEnabled(true);
    m_btnRefitGrad->setEnabled(true);
    m_btnStack->setEnabled(true);
    m_btnDumpGrad->setEnabled(true);
    m_btnCancel->setEnabled(false);
    m_table->setSortingEnabled(true);

    // Auto-select: enable all successfully aligned frames, disable failed ones
    auto &frames = m_engine->frames();
    int aligned = 0, failed = 0;
    for (int i = 0; i < (int)frames.size() && i < m_table->rowCount(); i++) {
        bool ok = frames[i].aligned && frames[i].enabled;
        m_table->item(i, 0)->setCheckState(ok ? Qt::Checked : Qt::Unchecked);
        frames[i].enabled = ok;

        // Update quality/weight after normalization
        m_table->item(i, 6)->setText(QString::number(frames[i].qualityScore, 'f', 3));
        m_table->item(i, 7)->setText(QString::number(frames[i].weight, 'f', 3));

        // Re-color
        QColor bg;
        if (frames[i].hasSatelliteTrail)        bg = QColor(255, 160, 255);
        else if (!ok)                            bg = QColor(255, 180, 180);
        else if (frames[i].qualityScore > 0.7)  bg = QColor(180, 255, 180);
        else if (frames[i].qualityScore > 0.3)  bg = QColor(255, 255, 200);
        else                                     bg = QColor(255, 220, 180);
        for (int c = 0; c < 10; c++)
            if (m_table->item(i, c))
                m_table->item(i, c)->setBackground(bg);

        if (ok) aligned++; else failed++;
    }

    updateSelectionSummary();
    m_statusLabel->setText(QString("Analysis complete. %1 aligned, %2 failed. Ref: frame %3. Click Stack or adjust parameters.")
                               .arg(aligned).arg(failed).arg(m_engine->referenceIndex()));

    // Show reference frame as preview with label
    if (m_engine->referenceIndex() >= 0) {
        cv::Mat ref = StackEngine::loadFits(frames[m_engine->referenceIndex()].path);
        if (!ref.empty()) {
            m_currentImage = ref;
            onStretchChanged();
            // Overlay label
            auto *label = m_scene->addText(
                QString("Reference frame (single exposure) — #%1")
                    .arg(m_engine->referenceIndex()),
                QFont("sans-serif", 12, QFont::Bold));
            label->setDefaultTextColor(Qt::yellow);
            label->setPos(10, 10);
        }
    }
}

void MainWindow::onStackComplete(cv::Mat result)
{
    m_stackResult = result;
    m_currentImage = result;
    m_btnStack->setEnabled(true);
    m_btnSave->setEnabled(true);
    m_btnCancel->setEnabled(false);

    int selected = 0;
    for (auto &f : m_engine->frames())
        if (f.enabled && f.aligned) selected++;
    m_statusLabel->setText(QString("Stacking complete (%1 frames). Adjust and re-stack, or save.").arg(selected));
    onStretchChanged();
}

void MainWindow::onStretchChanged()
{
    if (m_currentImage.empty()) return;

    double blackPct = m_blackSlider->value() / 10.0;
    double whitePct = m_whiteSlider->value() / 10.0;
    float midtone = m_midSlider->value() / 100.0f;

    m_blackLabel->setText(QString("%1%").arg(blackPct, 0, 'f', 1));
    m_whiteLabel->setText(QString("%1%").arg(whitePct, 0, 'f', 1));
    m_midLabel->setText(QString::number(midtone, 'f', 2));

    // Channel selection: 0=RGB, 1=R, 2=G, 3=B
    int chanSel = m_channelCombo->currentIndex();
    cv::Mat displaySrc;
    if (chanSel > 0 && m_currentImage.channels() == 3) {
        std::vector<cv::Mat> ch;
        cv::split(m_currentImage, ch);
        displaySrc = ch[chanSel - 1];  // 1→R(0), 2→G(1), 3→B(2)
    } else {
        displaySrc = m_currentImage;
    }

    // Compute luminance for percentile stretch levels
    cv::Mat lum;
    if (displaySrc.channels() == 3) {
        std::vector<cv::Mat> ch;
        cv::split(displaySrc, ch);
        lum = 0.2126f * ch[0] + 0.7152f * ch[1] + 0.0722f * ch[2];
    } else {
        lum = displaySrc;
    }

    std::vector<float> vals;
    vals.reserve(lum.rows * lum.cols / 16);
    for (int y = 0; y < lum.rows; y += 4)
        for (int x = 0; x < lum.cols; x += 4) {
            float v = lum.at<float>(y, x);
            if (std::isfinite(v)) vals.push_back(v);
        }
    std::sort(vals.begin(), vals.end());
    if (vals.empty()) return;

    float black = vals[std::clamp((int)(vals.size() * blackPct / 100.0), 0, (int)vals.size() - 1)];
    float white = vals[std::clamp((int)(vals.size() * whitePct / 100.0), 0, (int)vals.size() - 1)];
    if (white <= black) white = black + 1;

    cv::Mat stretched;
    displaySrc.convertTo(stretched, CV_32F);
    stretched = (stretched - black) / (white - black);

    for (int y = 0; y < stretched.rows; y++) {
        float *p = stretched.ptr<float>(y);
        for (int x = 0; x < stretched.cols * stretched.channels(); x++) {
            float v = std::clamp(p[x], 0.0f, 1.0f);
            if (v > 0 && v < 1 && midtone > 0.01f && midtone < 0.99f) {
                v = (midtone - 1.0f) * v / ((2.0f * midtone - 1.0f) * v - midtone);
            }
            p[x] = v;
        }
    }

    cv::Mat display8;
    stretched.convertTo(display8, CV_8U, 255.0);
    displayImage(display8);
    updateHistogram();
}

void MainWindow::updateHistogram()
{
    if (m_currentImage.empty()) return;
    m_histogram->setImage(m_currentImage);
    double blackPct = m_blackSlider->value() / 10.0;
    double whitePct = m_whiteSlider->value() / 10.0;
    m_histogram->setStretchMarkers(blackPct, whitePct);
}

void MainWindow::onQualityThresholdChanged(double val)
{
    if (!m_analyzed) return;
    int pct = (int)(val * 100);
    m_qualSliderLabel->setText(QString("%1%  ").arg(pct) +
        (pct == 0 ? "(all frames)" : QString("(>%1 quality)").arg(val, 0, 'f', 2)));
    applyQualityFilter();
}

void MainWindow::applyQualityFilter()
{
    double val = m_qualSlider->value() / 100.0;
    double minDepression = m_solarDepression->value();
    auto &frames = m_engine->frames();
    for (int i = 0; i < (int)frames.size() && i < m_table->rowCount(); i++) {
        bool shouldEnable = frames[i].aligned && frames[i].qualityScore >= val;
        // Solar depression filter: sun altitude must be <= -minDepression
        if (minDepression > 0 && frames[i].solarAltitude > -minDepression)
            shouldEnable = false;
        m_table->item(i, 0)->setCheckState(shouldEnable ? Qt::Checked : Qt::Unchecked);
    }
    updateSelectionSummary();
}

void MainWindow::updateSelectionSummary()
{
    int total = m_table->rowCount();
    int checked = 0, failedAlign = 0;
    auto &frames = m_engine->frames();
    for (int i = 0; i < total; i++) {
        if (m_table->item(i, 0) && m_table->item(i, 0)->checkState() == Qt::Checked)
            checked++;
        if (i < (int)frames.size() && !frames[i].aligned)
            failedAlign++;
    }
    m_selectionLabel->setText(QString("%1/%2 selected (%3 failed alignment)")
                                  .arg(checked).arg(total).arg(failedAlign));
}

void MainWindow::displayImage(const cv::Mat &img)
{
    m_scene->clear();
    QImage qi = matToQImage(img);
    m_scene->addPixmap(QPixmap::fromImage(qi));
    m_scene->setSceneRect(0, 0, qi.width(), qi.height());
    applyZoom();
}

void MainWindow::applyZoom()
{
    if (m_scene->items().isEmpty()) return;
    int val = m_zoomSlider->value();
    m_view->resetTransform();
    if (val == 0) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    } else {
        // val goes from 1..100, map to scale where 100 = 1:1 (native pixels)
        // At 1:1, scale = 1.0 (one scene pixel = one screen pixel)
        // At val=1, use the fit-to-view scale as minimum
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        QTransform t = m_view->transform();
        double fitScale = t.m11();  // current fit scale
        double targetScale = fitScale + (1.0 - fitScale) * val / 100.0;
        m_view->resetTransform();
        m_view->scale(targetScale, targetScale);
    }
}

QImage MainWindow::matToQImage(const cv::Mat &mat)
{
    if (mat.type() == CV_8UC3) {
        QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.copy();
    }
    if (mat.type() == CV_8UC1) {
        QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return img.copy();
    }
    cv::Mat tmp;
    mat.convertTo(tmp, CV_8U, 255.0);
    return matToQImage(tmp);
}
