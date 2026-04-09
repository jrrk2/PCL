// gp_gui.cpp -- Qt5 GUI implementation for galaxy_processor
#include "gp_gui.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QShortcut>
#include <algorithm>
#include <cmath>

// ============================================================================
// HistogramWidget
// ============================================================================

HistogramWidget::HistogramWidget( const ImageHistogram& hist, QWidget* parent )
   : QDialog( parent )
   , m_hist( hist )
{
   setWindowTitle( "Histogram" );
   resize( 540, 320 );
}

void HistogramWidget::paintEvent( QPaintEvent* )
{
   QPainter p( this );
   p.fillRect( rect(), Qt::black );

   const int margin = 30;
   const int plotW  = width()  - 2 * margin;
   const int plotH  = height() - 2 * margin;
   if ( plotW < 10 || plotH < 10 )
      return;

   // Find peak across all channels (skip bin 0 which is often background)
   float peak = 1e-30f;
   for ( int c = 0; c < 3; ++c )
      for ( int b = 1; b < HIST_BINS; ++b )
         peak = std::max( peak, m_hist.ch[c][b] );

   // Draw per-channel histograms
   QColor colors[3] = { QColor( 200, 60, 60 ), QColor( 60, 200, 60 ), QColor( 80, 80, 220 ) };

   for ( int c = 0; c < 3; ++c )
   {
      p.setPen( colors[c] );
      for ( int b = 0; b < HIST_BINS; ++b )
      {
         int x = margin + b * plotW / HIST_BINS;
         float h = m_hist.ch[c][b] / peak;
         if ( h > 1.0f ) h = 1.0f;
         int barH = (int)( h * plotH );
         p.drawLine( x, margin + plotH, x, margin + plotH - barH );
      }
   }

   // Axes
   p.setPen( Qt::white );
   p.drawRect( margin, margin, plotW, plotH );
   p.drawText( margin, height() - 5, "0.0" );
   p.drawText( margin + plotW - 20, height() - 5, "1.0" );
}

// ============================================================================
// Percentile auto-stretch helpers
// ============================================================================

static void computeChannelPercentiles( const Image& img, int channel,
                                       float& lo, float& hi )
{
   int w = img.Width(), h = img.Height();
   int totalPixels = w * h;
   if ( totalPixels == 0 || channel >= img.NumberOfChannels() )
   {
      lo = 0.0f;
      hi = 1.0f;
      return;
   }

   // Sample up to 500000 pixels for speed
   const float* data = img.PixelData( channel );
   std::vector<float> samples;
   int step = std::max( 1, totalPixels / 500000 );
   samples.reserve( totalPixels / step + 1 );
   for ( int i = 0; i < totalPixels; i += step )
      samples.push_back( data[i] );

   std::sort( samples.begin(), samples.end() );

   int n = (int)samples.size();
   // p0.1 and p99.9
   int idxLo = std::max( 0, (int)( 0.001f * n ) );
   int idxHi = std::min( n - 1, (int)( 0.999f * n ) );
   lo = samples[idxLo];
   hi = samples[idxHi];
   if ( hi <= lo )
      hi = lo + 1e-6f;
}

// ============================================================================
// GalaxyProcessorWindow
// ============================================================================

GalaxyProcessorWindow::GalaxyProcessorWindow( QWidget* parent )
   : QMainWindow( parent )
   , m_undoAction( nullptr )
   , m_hasUndo( false )
   , m_zoomFactor( 1.0 )
{
   setWindowTitle( "Galaxy Processor" );
   resize( 1200, 900 );

   // Central widget: scroll area with image label
   m_scrollArea = new QScrollArea( this );
   m_scrollArea->setBackgroundRole( QPalette::Dark );
   m_scrollArea->setAlignment( Qt::AlignCenter );

   m_imageLabel = new QLabel;
   m_imageLabel->setScaledContents( true );
   m_imageLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

   m_scrollArea->setWidget( m_imageLabel );
   setCentralWidget( m_scrollArea );

   // Status bar
   statusBar()->showMessage( "Ready" );

   // Menus
   createMenus();
}

void GalaxyProcessorWindow::createMenus()
{
   // ---- File ----
   QMenu* fileMenu = menuBar()->addMenu( "&File" );

   QAction* openAct = fileMenu->addAction( "&Open...", this, &GalaxyProcessorWindow::onFileOpen );
   openAct->setShortcut( QKeySequence::Open );

   QAction* saveAct = fileMenu->addAction( "Save &As...", this, &GalaxyProcessorWindow::onFileSaveAs );
   saveAct->setShortcut( QKeySequence::SaveAs );

   fileMenu->addSeparator();
   fileMenu->addAction( "Run &Lua Script...", this, &GalaxyProcessorWindow::onRunLuaScript );

   fileMenu->addSeparator();
   QAction* exitAct = fileMenu->addAction( "E&xit", this, &QWidget::close );
   exitAct->setShortcut( QKeySequence::Quit );

   // ---- Edit ----
   QMenu* editMenu = menuBar()->addMenu( "&Edit" );
   m_undoAction = editMenu->addAction( "&Undo", this, &GalaxyProcessorWindow::onUndo );
   m_undoAction->setShortcut( QKeySequence::Undo );
   m_undoAction->setEnabled( false );

   // ---- Process ----
   QMenu* procMenu = menuBar()->addMenu( "&Process" );
   procMenu->addAction( "&Plate Solve",           this, &GalaxyProcessorWindow::onPlateSolve );
   procMenu->addAction( "&Background Neutralize", this, &GalaxyProcessorWindow::onBgNeutralize );
   procMenu->addAction( "S&PCC...",               this, &GalaxyProcessorWindow::onSPCC );
   procMenu->addAction( "&SDSS Calibrate",        this, &GalaxyProcessorWindow::onSDSSCalibrate );
   procMenu->addAction( "Smart &Crop",            this, &GalaxyProcessorWindow::onSmartCrop );

   // ---- Stretch ----
   QMenu* stretchMenu = menuBar()->addMenu( "&Stretch" );
   stretchMenu->addAction( "&VeraLux Stretch...", this, &GalaxyProcessorWindow::onVeraLuxStretch );
   stretchMenu->addAction( "&Wavelet Enhance",    this, &GalaxyProcessorWindow::onWaveletEnhance );

   // ---- Stack ----
   QMenu* stackMenu = menuBar()->addMenu( "Stac&k" );
   stackMenu->addAction( "Stack &RGB...",  this, &GalaxyProcessorWindow::onStackRGB );
   stackMenu->addAction( "Stack &LRGB...", this, &GalaxyProcessorWindow::onStackLRGB );

   // ---- Modules (PCL open-source process wrappers) ----
   QMenu* modMenu = menuBar()->addMenu( "&Modules" );
   modMenu->addAction( "&Histogram Transform...", this, &GalaxyProcessorWindow::onHistogramTransform );
   modMenu->addAction( "&Curves Transform...",    this, &GalaxyProcessorWindow::onCurvesTransform );
   modMenu->addAction( "&Unsharp Mask...",        this, &GalaxyProcessorWindow::onUnsharpMask );
   modMenu->addAction( "&Morphology...",          this, &GalaxyProcessorWindow::onMorphology );
   modMenu->addAction( "&Auto Histogram",         this, &GalaxyProcessorWindow::onAutoHistogram );
   modMenu->addAction( "Con&volution...",          this, &GalaxyProcessorWindow::onConvolution );
   modMenu->addSeparator();
   modMenu->addAction( "&Resample...",            this, &GalaxyProcessorWindow::onResample );
   modMenu->addAction( "Cro&p...",                this, &GalaxyProcessorWindow::onCropImage );
   modMenu->addAction( "R&otate...",              this, &GalaxyProcessorWindow::onRotateImage );
   modMenu->addSeparator();
   modMenu->addAction( "&PixelMath...",           this, &GalaxyProcessorWindow::onPixelMath );

   // ---- View ----
   QMenu* viewMenu = menuBar()->addMenu( "&View" );

   QAction* zoomInAct = viewMenu->addAction( "Zoom &In",  this, &GalaxyProcessorWindow::onZoomIn );
   zoomInAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Equal ) );

   QAction* zoomOutAct = viewMenu->addAction( "Zoom &Out", this, &GalaxyProcessorWindow::onZoomOut );
   zoomOutAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_Minus ) );

   viewMenu->addAction( "&Fit to Window", this, &GalaxyProcessorWindow::onFitToWindow );
   viewMenu->addSeparator();
   viewMenu->addAction( "Show &Histogram", this, &GalaxyProcessorWindow::onShowHistogram );
}

// ============================================================================
// Key handling (zoom shortcuts also available via menu)
// ============================================================================

void GalaxyProcessorWindow::keyPressEvent( QKeyEvent* event )
{
   if ( event->modifiers() & Qt::ControlModifier )
   {
      if ( event->key() == Qt::Key_Equal || event->key() == Qt::Key_Plus )
      { onZoomIn(); return; }
      if ( event->key() == Qt::Key_Minus )
      { onZoomOut(); return; }
   }
   QMainWindow::keyPressEvent( event );
}

// ============================================================================
// Undo
// ============================================================================

void GalaxyProcessorWindow::onUndo()
{
   if ( !m_hasUndo )
      return;

   m_image = std::move( m_undoImage );
   m_hasUndo = false;
   m_undoAction->setEnabled( false );
   refreshDisplay();
   statusBar()->showMessage( "Undo" );
}

// ============================================================================
// Image <-> QImage conversion
// ============================================================================

QImage GalaxyProcessorWindow::imageToQImage( const Image& img ) const
{
   int w = img.Width(), h = img.Height();
   if ( w == 0 || h == 0 )
      return QImage();

   int nc = img.NumberOfChannels();

   // Compute per-channel percentile clipping for auto-stretch
   float lo[3], hi[3];
   for ( int c = 0; c < std::min( nc, 3 ); ++c )
      computeChannelPercentiles( img, c, lo[c], hi[c] );

   // For mono images, replicate channel 0 to all three
   if ( nc < 3 )
   {
      for ( int c = nc; c < 3; ++c )
      {
         lo[c] = lo[0];
         hi[c] = hi[0];
      }
   }

   QImage qimg( w, h, QImage::Format_RGB888 );

   for ( int y = 0; y < h; ++y )
   {
      uchar* scanline = qimg.scanLine( y );
      for ( int x = 0; x < w; ++x )
      {
         for ( int c = 0; c < 3; ++c )
         {
            int srcChan = ( c < nc ) ? c : 0;
            float v = img( x, y, srcChan );
            float range = hi[c] - lo[c];
            v = ( v - lo[c] ) / range;
            if ( v < 0.0f ) v = 0.0f;
            if ( v > 1.0f ) v = 1.0f;
            scanline[x * 3 + c] = (uchar)( v * 255.0f + 0.5f );
         }
      }
   }

   return qimg;
}

void GalaxyProcessorWindow::refreshDisplay()
{
   if ( m_image.Width() == 0 )
   {
      m_imageLabel->clear();
      statusBar()->showMessage( "No image loaded" );
      return;
   }

   QImage qimg = imageToQImage( m_image );

   int dispW = int( qimg.width()  * m_zoomFactor );
   int dispH = int( qimg.height() * m_zoomFactor );

   m_imageLabel->setPixmap( QPixmap::fromImage( qimg ).scaled(
      dispW, dispH, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
   m_imageLabel->adjustSize();

   statusBar()->showMessage(
      QString( "%1x%2  %3 ch  zoom %4%%5" )
         .arg( m_image.Width() )
         .arg( m_image.Height() )
         .arg( m_image.NumberOfChannels() )
         .arg( int( m_zoomFactor * 100 ) )
         .arg( m_hasUndo ? "  [Ctrl+Z to undo]" : "" ) );
}

// ============================================================================
// Async processing helpers
// ============================================================================

void GalaxyProcessorWindow::runAsync( const QString& taskName, std::function<void()> func )
{
   statusBar()->showMessage( taskName + "..." );
   setEnabled( false );

   auto* watcher = new QFutureWatcher<void>( this );
   connect( watcher, &QFutureWatcher<void>::finished, this, [this, watcher, taskName]()
   {
      setEnabled( true );
      refreshDisplay();
      statusBar()->showMessage( taskName + " done" );
      watcher->deleteLater();
   } );

   QFuture<void> future = QtConcurrent::run( func );
   watcher->setFuture( future );
}

void GalaxyProcessorWindow::runAsyncWithUndo( const QString& taskName, std::function<void()> func )
{
   // Save undo snapshot before processing
   m_undoImage.Assign( m_image );
   m_hasUndo = true;
   m_undoAction->setEnabled( true );

   runAsync( taskName, func );
}

// ============================================================================
// File operations
// ============================================================================

void GalaxyProcessorWindow::loadFile( const QString& path )
{
   std::string stdPath = path.toStdString();
   String pclPath = String::UTF8ToUTF16( stdPath.c_str() );

   bool isFITS = EndsWithCI( stdPath, ".fits" ) || EndsWithCI( stdPath, ".fit" )
              || EndsWithCI( stdPath, ".fts" );

   if ( isFITS )
   {
      FITSReader reader;
      reader.Open( pclPath );
      reader.SetIndex( 0 );
      m_keywords = reader.ReadFITSKeywords();
      reader.ReadImage( m_image );
      reader.Close();
   }
   else
   {
      XISFReader reader;
      reader.Open( pclPath );
      reader.SelectImage( 0 );
      m_keywords = reader.ReadFITSKeywords();
      m_properties = reader.ReadImageProperties();
      reader.ReadImage( m_image );

      // Handle UInt16 fallback (same pattern as gp_lua.cpp)
      bool allZero = true;
      if ( m_image.Width() > 0 && m_image.PixelData( 0 ) != nullptr )
      {
         const float* p = m_image.PixelData( 0 );
         for ( int i = 0; i < std::min( m_image.Width() * m_image.Height(), 10000 ); ++i )
            if ( p[i] != 0.0f ) { allZero = false; break; }
      }
      if ( allZero && m_image.Width() > 0 )
      {
         reader.Close();
         reader.Open( pclPath );
         reader.SelectImage( 0 );
         UInt16Image u16;
         reader.ReadImage( u16 );
         reader.Close();
         if ( u16.Width() > 0 && u16.PixelData( 0 ) )
         {
            int iw = u16.Width(), ih = u16.Height(), ic = u16.NumberOfChannels();
            m_image.AllocateData( iw, ih, ic, u16.ColorSpace() );
            for ( int c = 0; c < ic; ++c )
            {
               const uint16* src = u16.PixelData( c );
               float* dst = m_image.PixelData( c );
               for ( int i = 0; i < iw * ih; ++i )
                  dst[i] = float( src[i] ) / 65535.0f;
            }
         }
      }
      else
         reader.Close();
   }

   // Try to build WCS from loaded metadata
   BuildWCSFromMetadata( m_wcs, m_properties, m_keywords );

   m_currentPath = path;
   m_hasUndo = false;
   m_undoAction->setEnabled( false );
   setWindowTitle( "Galaxy Processor - " + QFileInfo( path ).fileName() );
}

void GalaxyProcessorWindow::saveFile( const QString& path )
{
   std::string stdPath = path.toStdString();
   String pclPath = String::UTF8ToUTF16( stdPath.c_str() );

   bool isFITS = EndsWithCI( stdPath, ".fits" ) || EndsWithCI( stdPath, ".fit" );
   if ( isFITS )
   {
      FITSWriter writer;
      writer.Create( pclPath, 1 );
      ImageOptions opts;
      opts.bitsPerSample = 32;
      opts.ieeefpSampleFormat = true;
      writer.SetOptions( opts );
      writer.WriteFITSKeywords( m_keywords );
      writer.WriteImage( m_image );
      writer.Close();
   }
   else
   {
      XISFWriter writer;
      writer.Create( pclPath, 1 );
      ImageOptions opts;
      opts.bitsPerSample = 32;
      opts.ieeefpSampleFormat = true;
      writer.SetImageOptions( opts );
      writer.WriteFITSKeywords( m_keywords );
      for ( const auto& p : m_properties )
         writer.WriteImageProperty( p.Id(), p.Value() );
      writer.WriteImage( m_image );
      writer.Close();
   }
}

void GalaxyProcessorWindow::onFileOpen()
{
   QString path = QFileDialog::getOpenFileName( this, "Open Image",
      QString(), "Astro Images (*.xisf *.fits *.fit *.fts);;All Files (*)" );
   if ( path.isEmpty() )
      return;

   runAsync( "Loading " + QFileInfo( path ).fileName(), [this, path]()
   {
      loadFile( path );
   } );
}

void GalaxyProcessorWindow::onFileSaveAs()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "Save As", "No image loaded." );
      return;
   }

   QString path = QFileDialog::getSaveFileName( this, "Save Image As",
      m_currentPath.isEmpty() ? "output.xisf" : m_currentPath,
      "XISF (*.xisf);;FITS (*.fits);;All Files (*)" );
   if ( path.isEmpty() )
      return;

   runAsync( "Saving", [this, path]()
   {
      saveFile( path );
   } );
}

void GalaxyProcessorWindow::onRunLuaScript()
{
   QString path = QFileDialog::getOpenFileName( this, "Run Lua Script",
      QString(), "Lua Scripts (*.lua);;All Files (*)" );
   if ( path.isEmpty() )
      return;

   runAsync( "Running Lua script", [path]()
   {
      RunLuaScript( path.toStdString() );
   } );
}

// ============================================================================
// Process menu actions
// ============================================================================

void GalaxyProcessorWindow::onPlateSolve()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "Plate Solve", "No image loaded." );
      return;
   }

   runAsync( "Plate Solving", [this]()
   {
      bool ok = PlateSolve( m_image, m_wcs, m_keywords );
      if ( !ok )
         std::cerr << "Plate solve failed\n";
      else
         std::cout << "Plate solve: RA=" << m_wcs.crval1
                   << " Dec=" << m_wcs.crval2
                   << " scale=" << m_wcs.Resolution() * 3600.0 << " arcsec/px\n";
   } );
}

void GalaxyProcessorWindow::onBgNeutralize()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "Background Neutralize", "No image loaded." );
      return;
   }

   runAsyncWithUndo( "Background Neutralize", [this]()
   {
      BackgroundNeutralize( m_image );
   } );
}

void GalaxyProcessorWindow::onSPCC()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "SPCC", "No image loaded." );
      return;
   }

   QStringList files = QFileDialog::getOpenFileNames( this,
      "Select Gaia DR3/SP Database Files", QString(),
      "Gaia DB (*.xpsd);;All Files (*)" );
   if ( files.isEmpty() )
      return;

   std::vector<std::string> dbPaths;
   for ( const QString& f : files )
      dbPaths.push_back( f.toStdString() );

   runAsyncWithUndo( "SPCC Calibration", [this, dbPaths]()
   {
      SpectrophotometricColorCalibrate( m_image, m_keywords, m_properties, dbPaths );
   } );
}

void GalaxyProcessorWindow::onSDSSCalibrate()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "SDSS Calibrate", "No image loaded." );
      return;
   }

   runAsyncWithUndo( "SDSS Color Calibration", [this]()
   {
      SDSSColorCalibrate( m_image, m_keywords );
   } );
}

void GalaxyProcessorWindow::onSmartCrop()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "Smart Crop", "No image loaded." );
      return;
   }

   runAsyncWithUndo( "Smart Crop", [this]()
   {
      int cropX = 0, cropY = 0;
      SmartCropStackingEdges( m_image, cropX, cropY );
      std::cout << "Smart crop offsets: x=" << cropX << " y=" << cropY << "\n";
   } );
}

// ============================================================================
// Stretch menu actions
// ============================================================================

void GalaxyProcessorWindow::onVeraLuxStretch()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "VeraLux Stretch", "No image loaded." );
      return;
   }
   if ( m_image.NumberOfChannels() < 3 )
   {
      QMessageBox::warning( this, "VeraLux Stretch", "Image must be RGB (3 channels)." );
      return;
   }

   QDialog dlg( this );
   dlg.setWindowTitle( "VeraLux Stretch" );
   QFormLayout* form = new QFormLayout( &dlg );

   auto* targetBg = new QDoubleSpinBox;
   targetBg->setRange( 0.01, 0.50 );
   targetBg->setDecimals( 3 );
   targetBg->setSingleStep( 0.01 );
   targetBg->setValue( 0.10 );
   form->addRow( "Target background:", targetBg );

   auto* protectB = new QDoubleSpinBox;
   protectB->setRange( 0.5, 50.0 );
   protectB->setDecimals( 1 );
   protectB->setSingleStep( 0.5 );
   protectB->setValue( 5.0 );
   form->addRow( "Highlight protection (b):", protectB );

   auto* convergence = new QDoubleSpinBox;
   convergence->setRange( 0.0, 20.0 );
   convergence->setDecimals( 1 );
   convergence->setSingleStep( 0.5 );
   convergence->setValue( 3.5 );
   form->addRow( "Colour convergence:", convergence );

   auto* colorGrip = new QDoubleSpinBox;
   colorGrip->setRange( 0.0, 5.0 );
   colorGrip->setDecimals( 2 );
   colorGrip->setSingleStep( 0.1 );
   colorGrip->setValue( 1.0 );
   form->addRow( "Colour grip:", colorGrip );

   auto* adaptive = new QComboBox;
   adaptive->addItems( { "Off", "On" } );
   form->addRow( "Adaptive:", adaptive );

   auto* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
   connect( buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );
   connect( buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject );
   form->addRow( buttons );

   if ( dlg.exec() != QDialog::Accepted )
      return;

   VeraLuxParams params;
   params.target_bg   = targetBg->value();
   params.protect_b   = protectB->value();
   params.convergence = convergence->value();
   params.color_grip  = colorGrip->value();
   params.adaptive    = ( adaptive->currentIndex() == 1 );

   runAsyncWithUndo( "VeraLux Stretch", [this, params]()
   {
      VeraLuxStretch( m_image, params );
   } );
}

void GalaxyProcessorWindow::onWaveletEnhance()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "Wavelet Enhance", "No image loaded." );
      return;
   }
   if ( m_image.NumberOfChannels() < 3 )
   {
      QMessageBox::warning( this, "Wavelet Enhance", "Image must be RGB (3 channels)." );
      return;
   }

   runAsyncWithUndo( "Wavelet Enhance", [this]()
   {
      VeraLuxParams p;
      WaveletEnhance( m_image, p );
   } );
}

// ============================================================================
// Stack menu actions
// ============================================================================

void GalaxyProcessorWindow::onStackRGB()
{
   QStringList files = QFileDialog::getOpenFileNames( this,
      "Select Frames for RGB Stack", QString(),
      "Astro Images (*.xisf *.fits *.fit *.fts);;All Files (*)" );
   if ( files.size() < 2 )
   {
      if ( !files.isEmpty() )
         QMessageBox::information( this, "Stack RGB", "Need at least 2 frames to stack." );
      return;
   }

   runAsyncWithUndo( "Stacking RGB", [this, files]()
   {
      StackParams sp;
      m_image = StackFramesRGB( files, sp );
   } );
}

void GalaxyProcessorWindow::onStackLRGB()
{
   QStringList files = QFileDialog::getOpenFileNames( this,
      "Select Frames for LRGB Stack", QString(),
      "Astro Images (*.xisf *.fits *.fit *.fts);;All Files (*)" );
   if ( files.size() < 2 )
   {
      if ( !files.isEmpty() )
         QMessageBox::information( this, "Stack LRGB", "Need at least 2 frames to stack." );
      return;
   }

   runAsyncWithUndo( "Stacking LRGB", [this, files]()
   {
      StackParams sp;
      m_image = StackFramesLRGB( files, sp );
   } );
}

// ============================================================================
// View menu actions
// ============================================================================

void GalaxyProcessorWindow::onZoomIn()
{
   m_zoomFactor *= 1.25;
   if ( m_zoomFactor > 8.0 )
      m_zoomFactor = 8.0;
   refreshDisplay();
}

void GalaxyProcessorWindow::onZoomOut()
{
   m_zoomFactor /= 1.25;
   if ( m_zoomFactor < 0.05 )
      m_zoomFactor = 0.05;
   refreshDisplay();
}

void GalaxyProcessorWindow::onFitToWindow()
{
   if ( m_image.Width() == 0 )
      return;

   QSize viewSize = m_scrollArea->viewport()->size();
   double scaleX = double( viewSize.width() )  / m_image.Width();
   double scaleY = double( viewSize.height() ) / m_image.Height();
   m_zoomFactor = std::min( scaleX, scaleY );
   refreshDisplay();
}

void GalaxyProcessorWindow::onShowHistogram()
{
   if ( m_image.Width() == 0 )
   {
      QMessageBox::warning( this, "Histogram", "No image loaded." );
      return;
   }

   ImageHistogram hist = compute_histogram( m_image );
   HistogramWidget* hw = new HistogramWidget( hist, this );
   hw->setAttribute( Qt::WA_DeleteOnClose );
   hw->show();
}

// ============================================================================
// PCL Module slots — combined parameter dialogs
// ============================================================================

void GalaxyProcessorWindow::onHistogramTransform()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   QDialog dlg( this );
   dlg.setWindowTitle( "Histogram Transformation" );
   QFormLayout* form = new QFormLayout( &dlg );

   auto* midtones = new QDoubleSpinBox;
   midtones->setRange( 0.001, 0.999 );
   midtones->setDecimals( 4 );
   midtones->setSingleStep( 0.01 );
   midtones->setValue( 0.25 );
   form->addRow( "Midtones balance:", midtones );

   auto* shadows = new QDoubleSpinBox;
   shadows->setRange( 0.0, 0.5 );
   shadows->setDecimals( 4 );
   shadows->setSingleStep( 0.001 );
   shadows->setValue( 0.0 );
   form->addRow( "Shadows clipping:", shadows );

   auto* highlights = new QDoubleSpinBox;
   highlights->setRange( 0.5, 1.0 );
   highlights->setDecimals( 4 );
   highlights->setSingleStep( 0.001 );
   highlights->setValue( 1.0 );
   form->addRow( "Highlights clipping:", highlights );

   auto* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
   connect( buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );
   connect( buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject );
   form->addRow( buttons );

   if ( dlg.exec() != QDialog::Accepted ) return;

   float m = (float)midtones->value();
   float s = (float)shadows->value();
   float h = (float)highlights->value();

   runAsyncWithUndo( "HistogramTransform", [this, m, s, h]() {
      ApplyHistogramTransformation( m_image, m, s, h );
   });
}

void GalaxyProcessorWindow::onCurvesTransform()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   QDialog dlg( this );
   dlg.setWindowTitle( "Curves Transformation" );
   QFormLayout* form = new QFormLayout( &dlg );

   auto* boost = new QDoubleSpinBox;
   boost->setRange( 0.0, 1.0 );
   boost->setDecimals( 2 );
   boost->setSingleStep( 0.05 );
   boost->setValue( 0.30 );
   form->addRow( "Contrast boost:", boost );

   auto* brightness = new QDoubleSpinBox;
   brightness->setRange( -0.3, 0.3 );
   brightness->setDecimals( 2 );
   brightness->setSingleStep( 0.02 );
   brightness->setValue( 0.0 );
   form->addRow( "Brightness shift:", brightness );

   auto* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
   connect( buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );
   connect( buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject );
   form->addRow( buttons );

   if ( dlg.exec() != QDialog::Accepted ) return;

   double b = boost->value();
   double br = brightness->value();
   std::vector<CurvePoint> curve = {
      {0, 0},
      {0.25, 0.25 - 0.15 * b + br},
      {0.5,  0.5 + br},
      {0.75, 0.75 + 0.15 * b + br},
      {1, 1}
   };

   runAsyncWithUndo( "CurvesTransform", [this, curve]() {
      ApplyCurvesTransformation( m_image, curve );
   });
}

void GalaxyProcessorWindow::onUnsharpMask()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   QDialog dlg( this );
   dlg.setWindowTitle( "Unsharp Mask" );
   QFormLayout* form = new QFormLayout( &dlg );

   auto* sigma = new QDoubleSpinBox;
   sigma->setRange( 0.1, 50.0 );
   sigma->setDecimals( 1 );
   sigma->setSingleStep( 0.5 );
   sigma->setValue( 2.0 );
   form->addRow( "Sigma (pixels):", sigma );

   auto* amount = new QDoubleSpinBox;
   amount->setRange( 0.0, 1.0 );
   amount->setDecimals( 2 );
   amount->setSingleStep( 0.05 );
   amount->setValue( 0.50 );
   form->addRow( "Amount:", amount );

   auto* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
   connect( buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );
   connect( buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject );
   form->addRow( buttons );

   if ( dlg.exec() != QDialog::Accepted ) return;

   float s = (float)sigma->value();
   float a = (float)amount->value();

   runAsyncWithUndo( "UnsharpMask", [this, s, a]() {
      ApplyUnsharpMask( m_image, s, a );
   });
}

void GalaxyProcessorWindow::onMorphology()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   QDialog dlg( this );
   dlg.setWindowTitle( "Morphological Transformation" );
   QFormLayout* form = new QFormLayout( &dlg );

   auto* opCombo = new QComboBox;
   opCombo->addItems( { "Erosion", "Dilation", "Opening", "Closing", "Median" } );
   opCombo->setCurrentIndex( 4 ); // default = Median
   form->addRow( "Operation:", opCombo );

   auto* size = new QSpinBox;
   size->setRange( 1, 10 );
   size->setValue( 1 );
   form->addRow( "Radius (pixels):", size );

   auto* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
   connect( buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );
   connect( buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject );
   form->addRow( buttons );

   if ( dlg.exec() != QDialog::Accepted ) return;

   int op = opCombo->currentIndex();
   int sz = size->value();

   runAsyncWithUndo( "Morphology", [this, op, sz]() {
      ApplyMorphologicalTransformation( m_image, op, sz );
   });
}

void GalaxyProcessorWindow::onAutoHistogram()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   runAsyncWithUndo( "AutoHistogram", [this]() {
      ApplyAutoHistogram( m_image );
   });
}

void GalaxyProcessorWindow::onConvolution()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   bool ok;
   double sigma = QInputDialog::getDouble( this, "Gaussian Convolution",
      "Sigma (pixels):", 2.0, 0.1, 50.0, 1, &ok );
   if ( !ok ) return;

   runAsyncWithUndo( "Convolution", [this, sigma]() {
      ApplyConvolution( m_image, (float)sigma );
   });
}

void GalaxyProcessorWindow::onResample()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   bool ok;
   double scale = QInputDialog::getDouble( this, "Resample",
      "Scale factor (e.g. 0.5 = half, 2.0 = double):", 0.5, 0.01, 10.0, 2, &ok );
   if ( !ok ) return;

   runAsyncWithUndo( "Resample", [this, scale]() {
      ApplyResample( m_image, scale );
   });
}

void GalaxyProcessorWindow::onCropImage()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   int W = m_image.Width(), H = m_image.Height();

   QDialog dlg( this );
   dlg.setWindowTitle( QString( "Crop  (%1 x %2)" ).arg( W ).arg( H ) );
   QFormLayout* form = new QFormLayout( &dlg );

   auto* left = new QSpinBox;
   left->setRange( 0, W / 2 );
   left->setValue( 50 );
   form->addRow( "Left:", left );

   auto* top = new QSpinBox;
   top->setRange( 0, H / 2 );
   top->setValue( 50 );
   form->addRow( "Top:", top );

   auto* right = new QSpinBox;
   right->setRange( W / 2, W );
   right->setValue( W - 50 );
   form->addRow( "Right:", right );

   auto* bottom = new QSpinBox;
   bottom->setRange( H / 2, H );
   bottom->setValue( H - 50 );
   form->addRow( "Bottom:", bottom );

   auto* buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
   connect( buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept );
   connect( buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject );
   form->addRow( buttons );

   if ( dlg.exec() != QDialog::Accepted ) return;

   int l = left->value(), t = top->value(), r = right->value(), b = bottom->value();

   runAsyncWithUndo( "Crop", [this, l, t, r, b]() {
      ApplyCrop( m_image, l, t, r, b );
   });
}

void GalaxyProcessorWindow::onRotateImage()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   bool ok;
   double angle = QInputDialog::getDouble( this, "Rotate",
      "Rotation angle (degrees, positive=CCW):", 0.0, -360.0, 360.0, 1, &ok );
   if ( !ok ) return;

   runAsyncWithUndo( "Rotate", [this, angle]() {
      ApplyRotation( m_image, angle );
   });
}

void GalaxyProcessorWindow::onPixelMath()
{
   if ( m_image.Width() == 0 ) { QMessageBox::warning( this, "Error", "No image loaded." ); return; }

   bool ok;
   QString expr = QInputDialog::getText( this, "PixelMath",
      "Expression ($T = pixel value):", QLineEdit::Normal,
      "$T * 1.5", &ok );
   if ( !ok || expr.isEmpty() ) return;

   std::string exprStr = expr.toStdString();
   runAsyncWithUndo( "PixelMath", [this, exprStr]() {
      ApplyPixelMath( m_image, exprStr );
   });
}

// ============================================================================
// RunGUI entry point
// ============================================================================

int RunGUI( int argc, char** argv )
{
   // Create QApplication if one does not already exist
   QApplication* app = qobject_cast<QApplication*>( QCoreApplication::instance() );
   bool ownApp = false;
   if ( !app )
   {
      app = new QApplication( argc, argv );
      ownApp = true;
   }

   // PCL API is already initialized by main() before calling RunGUI

   GalaxyProcessorWindow window;
   window.show();

   int ret = app->exec();

   if ( ownApp )
      delete app;

   return ret;
}
