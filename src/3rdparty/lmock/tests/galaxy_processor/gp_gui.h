// gp_gui.h -- Qt5 GUI for galaxy_processor
#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QImage>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDialog>
#include <QPainter>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>

#include "gp_common.h"
#include "gp_plate_solver.h"
#include "gp_color_calibration.h"
#include "gp_stretch.h"
#include "gp_histogram.h"
#include "gp_survey.h"
#include "gp_stacking.h"
#include "gp_lua.h"
#include "gp_pcl_modules.h"
#include "galaxy_processor.h"

// ============================================================================
// Histogram display widget
// ============================================================================

class HistogramWidget : public QDialog
{
   Q_OBJECT
public:
   explicit HistogramWidget( const ImageHistogram& hist, QWidget* parent = nullptr );

protected:
   void paintEvent( QPaintEvent* event ) override;

private:
   ImageHistogram m_hist;
};

// ============================================================================
// Main window
// ============================================================================

class GalaxyProcessorWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit GalaxyProcessorWindow( QWidget* parent = nullptr );
   ~GalaxyProcessorWindow() override = default;

protected:
   void keyPressEvent( QKeyEvent* event ) override;

private slots:
   // File
   void onFileOpen();
   void onFileSaveAs();
   void onRunLuaScript();

   // Edit
   void onUndo();

   // Process
   void onPlateSolve();
   void onBgNeutralize();
   void onSPCC();
   void onSDSSCalibrate();
   void onSmartCrop();

   // Stretch
   void onVeraLuxStretch();
   void onWaveletEnhance();

   // PCL Modules
   void onHistogramTransform();
   void onCurvesTransform();
   void onUnsharpMask();
   void onMorphology();
   void onAutoHistogram();
   void onConvolution();
   void onResample();
   void onCropImage();
   void onRotateImage();
   void onPixelMath();

   // Stack
   void onStackRGB();
   void onStackLRGB();

   // View
   void onZoomIn();
   void onZoomOut();
   void onFitToWindow();
   void onShowHistogram();

private:
   // Image loading / saving helpers
   void loadFile( const QString& path );
   void saveFile( const QString& path );

   // Convert PCL Image to QImage with auto-stretch for display
   QImage imageToQImage( const Image& img ) const;

   // Update the display label from current m_image
   void refreshDisplay();

   // Save current image state for undo, then run async processing
   void runAsyncWithUndo( const QString& taskName, std::function<void()> func );

   // Run a processing task on a background thread (no undo snapshot)
   void runAsync( const QString& taskName, std::function<void()> func );

   // Set up menus
   void createMenus();

   // Members
   QScrollArea*      m_scrollArea;
   QLabel*           m_imageLabel;
   QAction*          m_undoAction;

   Image             m_image;
   FITSKeywordArray  m_keywords;
   PropertyArray     m_properties;
   SimpleTANWCS      m_wcs;

   // Undo state (one level)
   Image             m_undoImage;
   bool              m_hasUndo;

   double            m_zoomFactor;
   QString           m_currentPath;
};

// Launch the GUI application.  argc/argv are forwarded from main.
int RunGUI( int argc, char** argv );
