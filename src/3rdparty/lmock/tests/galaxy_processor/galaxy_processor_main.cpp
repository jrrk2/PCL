#include "galaxy_processor.h"
#include "gp_plate_solver.h"
#include "gp_color_calibration.h"
#include "gp_stretch.h"
#include "gp_survey.h"
#include "gp_stacking.h"
#include "gp_lua.h"
#include "gp_gui.h"

extern "C" void* IdentifyPixInsightModule()
{
   static GalaxyProcessorModule module;
   return &module;
}

int main( int argc, char** argv )
{
   // Check for --gui early (before creating QCoreApplication)
   for ( int i = 1; i < argc; ++i )
      if ( std::string( argv[i] ) == "--gui" )
      {
         // Initialize Mock PCL API before GUI
         void* Module = IdentifyPixInsightModule();
         InitializePixInsightModule( (api_handle)Module, GetMockFunctionResolver(),
                                     PCL_API_Version, nullptr );
         return RunGUI( argc, argv );
      }

   // QCoreApplication needed for StellarSolver's event loop
   QCoreApplication app( argc, argv );

   if ( argc < 2 )
   {
      std::cerr << "Usage: galaxy_processor <input.xisf|fits ...> [-o output.xisf]\n"
                << "       [--spcc] [--bgneutralize] [--debayer] [--sdss]\n"
                << "       [--gaia-db /path/to/gdr3sp*.xpsd]\n"
                << "       [--stretch] [--optimize[=N]]\n"
                << "       [--stack] [--lrgb] [--gradient=0|1|2|3] [--global-gradient] [--survey-mask]\n"
                << "       [--sigma=N] [--clip-iters=N] [--max-frames=N]\n"
                << "       [--psf-weighting] [--local-norm[=N]] [--distortion-correction]\n"
                << "       [--pcl-stars] [--wcs-align]\n"
                << "       [--bias=<master_bias.fits>] [--dark=<master_dark.fits>] [--flat=<master_flat.fits>]\n"
                << "       [--stretch-mode=auto|linear|gamma|log|sinh] [--gamma=N]\n"
                << "       [--gaia-astro-db /path/to/gdr3-*.xpsd]\n"
                << "       [--script <script.lua>]\n"
                << "       [--gui]\n"
                ;
      return 1;
   }

   // Initialize Mock PCL API (required for StarDetector/Console)
   {
      void* Module = IdentifyPixInsightModule();
      InitializePixInsightModule( (api_handle)Module, GetMockFunctionResolver(),
                                  PCL_API_Version, nullptr );
   }

   // Parse arguments
   std::string inputPath;
   std::vector<std::string> inputPaths;  // multiple inputs for stacking
   std::string outputPath;
   std::vector<std::string> gaiaDbPaths;
   bool doSPCC = false;
   bool doBgNeutralize = false;
   bool doDebayer = true;
   bool doSDSS = false;
   bool doStretch = false;
   bool doOptimize = false;
   int  optIterations = 50;
   bool doStack = false;
   int maxFrames = 0; // 0 = no limit
   bool doLRGB = false;
   bool globalGradient = false;
   bool surveyMaskGradient = false;
   StackParams stackParams;

   for ( int i = 1; i < argc; ++i )
   {
      std::string arg( argv[i] );
      if ( arg == "-o" && i + 1 < argc )
         outputPath = argv[++i];
      else if ( arg == "--gaia-db" && i + 1 < argc )
         gaiaDbPaths.push_back( argv[++i] );
      else if ( arg == "--spcc" )
         doSPCC = true;
      else if ( arg == "--bgneutralize" )
         doBgNeutralize = true;
      else if ( arg == "--debayer" )
         doDebayer = true;
      else if ( arg == "--no-debayer" )
         doDebayer = false;
      else if ( arg == "--sdss" )
         doSDSS = true;
      else if ( arg == "--stretch" )
         doStretch = true;
      else if ( arg == "--optimize" )
      {
         doOptimize = true;
         doStretch = true; // optimize implies stretch
      }
      else if ( arg.substr( 0, 11 ) == "--optimize=" )
      {
         doOptimize = true;
         doStretch = true;
         optIterations = std::max( 1, std::atoi( arg.c_str() + 11 ) );
      }
      else if ( arg == "--stack" )
         doStack = true;
      else if ( arg == "--lrgb" )
      {
         doStack = true;
         doLRGB = true;
      }
      else if ( arg.substr( 0, 11 ) == "--gradient=" )
         stackParams.gradientDegree = std::atoi( arg.c_str() + 11 );
      else if ( arg == "--global-gradient" )
         globalGradient = true;
      else if ( arg == "--survey-mask" )
      {
         globalGradient = true;  // survey mask implies global gradient
         surveyMaskGradient = true;
      }
      else if ( arg.substr( 0, 8 ) == "--sigma=" )
      {
         stackParams.sigmaLow = std::atof( arg.c_str() + 8 );
         stackParams.sigmaHigh = stackParams.sigmaLow;
      }
      else if ( arg.substr( 0, 13 ) == "--clip-iters=" )
         stackParams.clipIterations = std::max( 1, std::atoi( arg.c_str() + 13 ) );
      else if ( arg.substr( 0, 13 ) == "--max-frames=" )
         maxFrames = std::max( 1, std::atoi( arg.c_str() + 13 ) );
      else if ( arg == "--psf-weighting" )
         stackParams.psfWeighting = true;
      else if ( arg == "--local-norm" )
         stackParams.localNormalization = true;
      else if ( arg.substr( 0, 13 ) == "--local-norm=" )
      {
         stackParams.localNormalization = true;
         stackParams.localNormGridSize = std::max( 4, std::atoi( arg.c_str() + 13 ) );
      }
      else if ( arg == "--distortion-correction" )
         stackParams.distortionCorrection = true;
      else if ( arg == "--pcl-stars" )
         stackParams.usePCLStarDetector = true;
      else if ( arg == "--wcs-align" )
         stackParams.wcsAlignment = true;
      else if ( arg == "--stretch-mode=linear" )
         stackParams.stretchMode = StretchMode::Linear;
      else if ( arg == "--stretch-mode=gamma" )
         stackParams.stretchMode = StretchMode::Gamma;
      else if ( arg == "--stretch-mode=log" )
         stackParams.stretchMode = StretchMode::Log;
      else if ( arg == "--stretch-mode=sinh" )
         stackParams.stretchMode = StretchMode::Sinh;
      else if ( arg == "--stretch-mode=auto" )
         stackParams.stretchMode = StretchMode::Auto;
      else if ( arg.substr( 0, 8 ) == "--gamma=" )
         stackParams.gamma = std::atof( arg.c_str() + 8 );
      else if ( arg.substr( 0, 7 ) == "--bias=" )
         stackParams.masterBiasPath = QString::fromStdString( arg.substr( 7 ) );
      else if ( arg.substr( 0, 7 ) == "--dark=" )
         stackParams.masterDarkPath = QString::fromStdString( arg.substr( 7 ) );
      else if ( arg.substr( 0, 7 ) == "--flat=" )
         stackParams.masterFlatPath = QString::fromStdString( arg.substr( 7 ) );
      else if ( arg == "--gaia-astro-db" && i + 1 < argc )
         g_gaiaAstroDbPath = argv[++i];
      else if ( arg == "--script" && i + 1 < argc )
      {
         std::string scriptPath = argv[++i];
         return RunLuaScript( scriptPath );
      }
      else if ( arg == "--gui" )
      {
         // Handled above, before QCoreApplication creation
         continue;
      }
      else if ( arg[0] != '-' )
      {
         if ( inputPath.empty() )
            inputPath = arg;
         inputPaths.push_back( arg );
      }
      else
      {
         std::cerr << "Unknown argument: " << arg << "\n";
         return 1;
      }
   }

   if ( inputPath.empty() )
   {
      std::cerr << "ERROR: No input file specified\n";
      return 1;
   }

   // Apply --max-frames limit (clamp to actual file count)
   if ( maxFrames > 0 )
   {
      maxFrames = std::min( maxFrames, (int)inputPaths.size() );
      if ( (int)inputPaths.size() > maxFrames )
      {
         std::cout << "Limiting to " << maxFrames << " of " << inputPaths.size() << " input frames\n";
         inputPaths.resize( maxFrames );
         inputPath = inputPaths[0];
      }
   }

   // Default output path — write to current directory
   if ( outputPath.empty() )
   {
      // Strip directory from input filename
      std::string baseName = inputPath;
      size_t slash = baseName.rfind( '/' );
      if ( slash != std::string::npos )
         baseName = baseName.substr( slash + 1 );

      // When stacking, strip per-frame date/time code (e.g. _20260305_193734)
      // from the first filename since it doesn't represent the whole stack.
      if ( doStack && inputPaths.size() > 1 )
      {
         size_t dot = baseName.rfind( '.' );
         std::string stem = ( dot != std::string::npos ) ? baseName.substr( 0, dot ) : baseName;
         // Match trailing _YYYYMMDD_HHMMSS pattern
         if ( stem.size() >= 16 )
         {
            size_t pos = stem.size() - 16;
            bool isDate = stem[pos] == '_';
            for ( int i = 1; i <= 8 && isDate; i++ )
               isDate = std::isdigit( stem[pos + i] );
            isDate = isDate && stem[pos + 9] == '_';
            for ( int i = 10; i <= 15 && isDate; i++ )
               isDate = std::isdigit( stem[pos + i] );
            if ( isDate )
               stem = stem.substr( 0, pos );
         }
         outputPath = stem + "_stacked.xisf";
      }
      else
      {
         size_t dot = baseName.rfind( '.' );
         if ( dot != std::string::npos )
            outputPath = baseName.substr( 0, dot ) + "_processed.xisf";
         else
            outputPath = baseName + "_processed.xisf";
      }
   }

   // Auto-detect Gaia astrometry database for catalog plate solving
   if ( g_gaiaAstroDbPath.empty() )
   {
      std::string home = QDir::homePath().toStdString();
      std::vector<std::string> searchDirs = {
         home + "/PixInsight/databases",
         "/usr/share/pixinsight",
         "/usr/local/share/pixinsight",
         home + "/.local/share/pixinsight"
      };
      for ( const auto& dir : searchDirs )
      {
         QDir d( QString::fromStdString( dir ) );
         QStringList astroFiles = d.entryList( { "gdr3-*.xpsd" }, QDir::Files );
         if ( !astroFiles.isEmpty() )
         {
            g_gaiaAstroDbPath = dir + "/" + astroFiles.first().toStdString();
            std::cout << "  Auto-detected Gaia astrometry DB: " << g_gaiaAstroDbPath << "\n";
            break;
         }
      }
   }

   // Default Gaia DB paths if not specified — search standard locations
   if ( gaiaDbPaths.empty() && doSPCC )
   {
      std::string home = QDir::homePath().toStdString();
      std::vector<std::string> searchDirs = {
         home + "/PixInsight/databases",
         "/usr/share/pixinsight",
         "/usr/local/share/pixinsight",
         home + "/.local/share/pixinsight"
      };
      const char* dbFiles[] = {
         "gdr3sp-1.0.0-s-01.xpsd",
         "gdr3sp-1.0.0-s-02.xpsd",
         "gdr3sp-1.0.0-s-03.xpsd",
         "gdr3sp-1.0.0-s-04.xpsd"
      };
      for ( const auto& dir : searchDirs )
      {
         QDir d( QString::fromStdString( dir ) );
         if ( d.exists( dbFiles[0] ) )
         {
            for ( const char* f : dbFiles )
               gaiaDbPaths.push_back( dir + "/" + f );
            break;
         }
      }
      if ( gaiaDbPaths.empty() )
      {
         std::cerr << "  Gaia DR3/SP database not found in:\n";
         for ( const auto& dir : searchDirs )
            std::cerr << "    " << dir << "/" << dbFiles[0] << "\n";
         std::cerr << "  Use --gaia-db <path> to specify SPCC database\n";
      }
   }

   try
   {
      std::cout << "======================================================================\n";
      std::cout << "Galaxy Processor Pipeline\n";
      std::cout << "======================================================================\n";
      std::cout << "Input:  " << inputPath << "\n";
      std::cout << "Output: " << outputPath << "\n\n";

      // ---------------------------------------------------------------
      // Load image (or stack multiple frames first)
      // ---------------------------------------------------------------
      Image image;
      FITSKeywordArray keywords;
      PropertyArray properties;

      if ( doStack && inputPaths.size() > 1 )
      {
         stackParams.debayer = doDebayer;
         std::cout << "--- Step 0: Stack " << inputPaths.size() << " frames ---\n";

         // Estimate disk space needed for temporary luminance frames.
         // Each frame produces a single-channel uncompressed float32 FITS
         // in ./galaxy_proc_lrgb/. Input is typically 16-bit RGB (6 bytes/px),
         // temp luminance is float32 mono (4 bytes/px) ≈ 2/3 of input size.
         {
            QFileInfo fi( QString::fromStdString( inputPaths[0] ) );
            double perFrameMB = fi.size() / ( 1024.0 * 1024.0 ) * 2.0 / 3.0;
            double totalGB = perFrameMB * inputPaths.size() / 1024.0;
            struct statvfs st;
            double availGB = 0;
            if ( statvfs( ".", &st ) == 0 )
               availGB = double( st.f_bavail ) * st.f_frsize / ( 1024.0 * 1024.0 * 1024.0 );
            std::cout << "  Temporary files: ./galaxy_proc_lrgb/ (~"
                      << std::fixed << std::setprecision( 1 ) << totalGB << " GB estimated";
            if ( availGB > 0 )
               std::cout << ", " << availGB << " GB available";
            std::cout << ")\n" << std::defaultfloat;
            if ( availGB > 0 && totalGB > availGB * 0.9 )
               std::cerr << "  WARNING: Estimated temp space may exceed available disk!\n";
         }

         if ( globalGradient )
         {
            std::cout << "  Gradient removal: global (degree " << stackParams.gradientDegree << ")\n";
            // Global gradient: stack first with gradientDegree=0,
            // then fit gradient on the stacked result
            StackParams noGradParams = stackParams;
            noGradParams.gradientDegree = 0;

            QStringList qPaths;
            for ( auto& p : inputPaths )
               qPaths.append( QString::fromStdString( p ) );

            if ( doLRGB )
               image = StackFramesLRGB( qPaths, noGradParams );
            else
               image = StackFramesRGB( qPaths, noGradParams );

            // Now fit and remove gradient from stacked result
            if ( image.Width() > 0 && stackParams.gradientDegree > 0 )
            {
               std::cout << "  Fitting global gradient (degree "
                         << stackParams.gradientDegree << ") on stacked image...\n";

               // Build survey-based signal mask if requested
               cv::Mat signalMask; // empty unless --survey-mask
               if ( surveyMaskGradient )
               {
                  std::cout << "  Building survey signal mask...\n";

                  // Plate-solve the stacked image to get WCS
                  SimpleTANWCS stackWcs;
                  bool solved = PlateSolve( image, stackWcs, keywords );
                  if ( !solved )
                     std::cerr << "  WARNING: Plate solve failed for survey mask — "
                               << "falling back to unmasked gradient fit\n";
                  else
                  {
                     // Compute field center
                     int iw = image.Width(), ih = image.Height();
                     double survCenterRA = stackWcs.crval1;
                     double survCenterDec = stackWcs.crval2;
                     double survPixscale = stackWcs.Resolution() * 3600.0;

                     if ( g_solvedWCS )
                     {
                        double cx = ( iw / 2.0 + g_cropOffsetX ) / 2.0 + 1.0;
                        double cy = ( ih / 2.0 + g_cropOffsetY ) / 2.0 + 1.0;
                        QPointF cp( cx, cy );
                        FITSImage::wcs_point sky;
                        if ( g_solvedWCS->pixelToWCS( cp, sky ) )
                        {
                           survCenterRA = sky.ra;
                           survCenterDec = sky.dec;
                        }
                     }

                     // Fetch survey tile and reproject to match stacked image
                     Image surveyRef;
                     if ( FetchSurveyReference( surveyRef, stackWcs, iw, ih,
                                                survCenterRA, survCenterDec, survPixscale, 0.0 ) )
                     {
                        // Convert reprojected survey to luminance
                        int sw = surveyRef.Width(), sh = surveyRef.Height();
                        std::vector<float> survLum( sw * sh );
                        int nch = std::min( surveyRef.NumberOfChannels(), 3 );
                        for ( int y = 0; y < sh; y++ )
                           for ( int x = 0; x < sw; x++ )
                           {
                              float lum = 0;
                              for ( int c = 0; c < nch; c++ )
                                 lum += surveyRef( x, y, c );
                              survLum[y * sw + x] = lum / nch;
                           }

                        // Compute median and MAD of survey luminance (non-zero pixels only)
                        std::vector<float> validPix;
                        validPix.reserve( sw * sh );
                        for ( float v : survLum )
                           if ( v > 0 )
                              validPix.push_back( v );

                        if ( validPix.size() > 100 )
                        {
                           std::sort( validPix.begin(), validPix.end() );
                           float median = validPix[validPix.size() / 2];
                           std::vector<float> absdev( validPix.size() );
                           for ( size_t i = 0; i < validPix.size(); i++ )
                              absdev[i] = std::abs( validPix[i] - median );
                           std::sort( absdev.begin(), absdev.end() );
                           float mad = absdev[absdev.size() / 2] * 1.4826f; // MAD → sigma

                           // Threshold: signal is anything above median + 3*MAD
                           float threshold = median + 3.0f * mad;
                           signalMask = cv::Mat( sh, sw, CV_8U, cv::Scalar( 255 ) );
                           for ( int y = 0; y < sh; y++ )
                              for ( int x = 0; x < sw; x++ )
                                 if ( survLum[y * sw + x] > threshold )
                                    signalMask.at<uint8_t>( y, x ) = 0; // exclude signal

                           int masked = cv::countNonZero( signalMask == 0 );
                           std::cout << "  Survey mask: " << masked << "/" << ( sw * sh )
                                     << " pixels masked as signal ("
                                     << std::setprecision( 1 ) << std::fixed
                                     << ( 100.0 * masked / ( sw * sh ) ) << "%)\n"
                                     << std::defaultfloat;
                           std::cout << "  Threshold: median=" << median
                                     << " MAD_sigma=" << mad
                                     << " thresh=" << threshold << "\n";
                        }
                        else
                           std::cerr << "  WARNING: Survey too few valid pixels for mask\n";
                     }
                     else
                        std::cerr << "  WARNING: Survey fetch failed — "
                                  << "falling back to unmasked gradient fit\n";
                  }
               }

               cv::Mat cvImg = PCLImageToCvMat( image );
               std::vector<cv::Mat> channels;
               cv::split( cvImg, channels );
               std::vector<StarPos> emptyStars; // no star mask needed on stacked image
               for ( int c = 0; c < std::min( (int)channels.size(), 3 ); c++ )
               {
                  std::vector<double> coeffs;
                  cv::Mat grad;
                  if ( !signalMask.empty() )
                     grad = StackEngine::fitGradient(
                        channels[c], emptyStars, stackParams.gradientDegree, coeffs, signalMask );
                  else
                     grad = StackEngine::fitGradient(
                        channels[c], emptyStars, stackParams.gradientDegree, coeffs );
                  channels[c] -= grad;
               }
               cv::Mat result;
               cv::merge( channels, result );
               image = CvMatToPCLImage( result );
               std::cout << "  Global gradient removed"
                         << ( signalMask.empty() ? ".\n" : " (survey-masked).\n" );
            }
         }
         else
         {
            std::cout << "  Gradient removal: per-frame (degree " << stackParams.gradientDegree << ")\n";

            QStringList qPaths;
            for ( auto& p : inputPaths )
               qPaths.append( QString::fromStdString( p ) );

            if ( doLRGB )
               image = StackFramesLRGB( qPaths, stackParams );
            else
               image = StackFramesRGB( qPaths, stackParams );
         }

         if ( image.Width() == 0 )
         {
            std::cerr << "ERROR: Stacking produced no result\n";
            return 1;
         }
         std::cout << "Stacked image: " << image.Width() << " x " << image.Height()
                   << " x " << image.NumberOfChannels() << "\n";

         // Normalize to [0,1] — stacker outputs raw ADU values
         {
            int nw = image.Width(), nh = image.Height();
            int nc = std::min( image.NumberOfChannels(), 3 );
            float vmax = 0;
            for ( int c = 0; c < nc; c++ )
               for ( int y = 0; y < nh; y++ )
                  for ( int x = 0; x < nw; x++ )
                     vmax = std::max( vmax, image( x, y, c ) );
            if ( vmax > 1.0f )
            {
               float scale = 1.0f / vmax;
               for ( int c = 0; c < nc; c++ )
                  for ( int y = 0; y < nh; y++ )
                     for ( int x = 0; x < nw; x++ )
                        image( x, y, c ) *= scale;
               std::cout << "  Normalized: peak " << std::setprecision( 1 )
                         << std::fixed << vmax << " → 1.0\n" << std::defaultfloat;
            }
         }
         std::cout << "\n";

         // Read keywords from first input for WCS metadata
         String pclFirstInput = String::UTF8ToUTF16( inputPaths[0].c_str() );
         bool firstIsFITS = EndsWithCI( inputPaths[0], ".fits" )
                         || EndsWithCI( inputPaths[0], ".fit" );
         if ( firstIsFITS )
         {
            FITSReader reader;
            reader.Open( pclFirstInput );
            reader.SetIndex( 0 );
            keywords = reader.ReadFITSKeywords();
            reader.Close();
         }
         else
         {
            XISFReader reader;
            reader.Open( pclFirstInput );
            reader.SelectImage( 0 );
            keywords = reader.ReadFITSKeywords();
            properties = reader.ReadImageProperties();
            reader.Close();
         }
      }
      else
      {
         // Single-file load path
         String pclInputPath = String::UTF8ToUTF16( inputPath.c_str() );
         bool isFITS = EndsWithCI( inputPath, ".fits" )
                    || EndsWithCI( inputPath, ".fit" )
                    || EndsWithCI( inputPath, ".fts" );

         if ( isFITS )
         {
            std::cout << "Loading FITS file...\n";
            FITSReader reader;
            reader.Open( pclInputPath );
            reader.SetIndex( 0 );
            keywords = reader.ReadFITSKeywords();
            reader.ReadImage( image );
            reader.Close();
         }
         else
         {
            std::cout << "Loading XISF file...\n";
            XISFReader reader;
            reader.Open( pclInputPath );
            reader.SelectImage( 0 );
            keywords = reader.ReadFITSKeywords();
            properties = reader.ReadImageProperties();

            // Read image — try Float32 first, then handle UInt16 conversion
            reader.ReadImage( image );

            // Check if pixel data is valid (UInt16→Float conversion may fail in mock API)
            bool allZero = true;
            if ( image.Width() > 0 && image.Height() > 0 && image.PixelData( 0 ) != nullptr )
            {
               const float* p = image.PixelData( 0 );
               int npx = image.Width() * image.Height();
               for ( int i = 0; i < std::min( npx, 10000 ); ++i )
                  if ( p[i] != 0.0f ) { allZero = false; break; }
            }
            if ( allZero && image.Width() > 0 )
            {
               std::cout << "  Float32 image has zero pixels, trying UInt16 load...\n";
               reader.Close();
               reader.Open( pclInputPath );
               reader.SelectImage( 0 );

               UInt16Image u16img;
               reader.ReadImage( u16img );
               reader.Close();

               if ( u16img.Width() > 0 && u16img.PixelData( 0 ) != nullptr )
               {
                  int iw = u16img.Width(), ih = u16img.Height(), ic = u16img.NumberOfChannels();
                  image.AllocateData( iw, ih, ic, u16img.ColorSpace() );
                  for ( int c = 0; c < ic; ++c )
                  {
                     const uint16* src = u16img.PixelData( c );
                     float* dst = image.PixelData( c );
                     for ( int i = 0; i < iw * ih; ++i )
                        dst[i] = float( src[i] ) / 65535.0f;
                  }
                  std::cout << "  UInt16→Float conversion done (" << iw << "x" << ih << "x" << ic << ")\n";
               }
            }
            else
               reader.Close();
         }
      }

      int w = image.Width();
      int h = image.Height();
      int ch = image.NumberOfChannels();
      std::cout << "Image dimensions: " << w << " x " << h << " x " << ch << "\n\n";

      // ---------------------------------------------------------------
      // Pipeline Step 1: Plate Solve (on full image before cropping)
      // ---------------------------------------------------------------
      SimpleTANWCS tanWcs;
      // Pre-populate WCS from XISF properties/FITS keywords so the Gaia solver
      // can extract pixel scale and position hints even without explicit RA/DEC keys
      BuildWCSFromMetadata( tanWcs, properties, keywords );
      if ( doSPCC || doOptimize )
      {
         std::cout << "--- Step 1: Plate Solve ---\n";
         if ( PlateSolve( image, tanWcs, keywords ) )
         {
            std::cout << "  Plate solve successful.\n";

            // Report field center and interesting object coordinates
            if ( g_solvedWCS )
            {
               // Field center
               QPointF centerPx( w / 4.0, h / 4.0 ); // /4 because 2x downsample
               FITSImage::wcs_point centerSky;
               if ( g_solvedWCS->pixelToWCS( centerPx, centerSky ) )
                  std::cout << "  Field center: RA=" << std::setprecision( 5 )
                            << centerSky.ra << " Dec=" << centerSky.dec << "\n";

               // Report corners to show field extent
               QPointF corners[4] = {
                  { 1.0, 1.0 }, { w / 2.0, 1.0 },
                  { 1.0, h / 2.0 }, { w / 2.0, h / 2.0 }
               };
               const char* cornerNames[4] = { "TL", "TR", "BL", "BR" };
               for ( int i = 0; i < 4; ++i )
               {
                  FITSImage::wcs_point sky;
                  if ( g_solvedWCS->pixelToWCS( corners[i], sky ) )
                     std::cout << "  Corner " << cornerNames[i] << ": RA=" << std::setprecision( 5 )
                               << sky.ra << " Dec=" << sky.dec << "\n";
               }

               // Report position hint object (if given in FITS keywords)
               double hintRA = -1, hintDec = -91;
               for ( const auto& kw : keywords )
               {
                  IsoString name = kw.name.Trimmed();
                  if ( name == "OBJCTRA" )
                  {
                     IsoString v = kw.value.Trimmed();
                     v.DeleteChar( '\'' );
                     StringList parts;
                     v.Break( parts, ' ', true );
                     if ( parts.Length() >= 3 )
                        hintRA = ( parts[0].ToDouble() + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600 ) * 15.0;
                  }
                  else if ( name == "OBJCTDEC" )
                  {
                     IsoString v = kw.value.Trimmed();
                     v.DeleteChar( '\'' );
                     StringList parts;
                     v.Break( parts, ' ', true );
                     if ( parts.Length() >= 3 )
                     {
                        double d = std::abs( parts[0].ToDouble() );
                        hintDec = d + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600;
                        if ( v.StartsWith( '-' ) || parts[0].ToDouble() < 0 )
                           hintDec = -hintDec;
                     }
                  }
               }
               if ( hintRA >= 0 && hintDec > -91 )
               {
                  // Convert target sky coord to pixel position
                  QPointF targetPx;
                  FITSImage::wcs_point targetSky;
                  targetSky.ra = float( hintRA );
                  targetSky.dec = float( hintDec );
                  if ( g_solvedWCS->wcsToPixel( targetSky, targetPx ) )
                  {
                     // Convert from downsampled FITS coords back to full image
                     double fullX = ( targetPx.x() - 1.0 ) * 2.0;
                     double fullY = ( targetPx.y() - 1.0 ) * 2.0;
                     std::cout << "  Target object RA=" << std::setprecision( 5 ) << hintRA
                               << " Dec=" << hintDec
                               << " → pixel (" << int( fullX ) << ", " << int( fullY ) << ")\n";
                  }
               }
            }
            std::cout << "\n";
         }
         else
            std::cout << "  Plate solve failed - SPCC will try metadata WCS.\n\n";
      }

      // ---------------------------------------------------------------
      // Pipeline Step 2: Smart crop of stacking edges
      // Only when freshly stacked — a pre-stacked input was already cropped.
      // ---------------------------------------------------------------
      if ( doStack && inputPaths.size() > 1 )
      {
         std::cout << "--- Step 2: Smart Crop Stacking Edges ---\n";
         SmartCropStackingEdges( image, g_cropOffsetX, g_cropOffsetY );
         if ( g_cropOffsetX > 0 || g_cropOffsetY > 0 )
         {
            std::cout << "  WCS offset adjusted: dx=" << g_cropOffsetX
                      << " dy=" << g_cropOffsetY << "\n";
         }
         w = image.Width();
         h = image.Height();
         std::cout << "\n";
      }

      // ---------------------------------------------------------------
      // Pipeline Step 3: Background Neutralization (on cropped image)
      // ---------------------------------------------------------------
      if ( doBgNeutralize )
      {
         std::cout << "--- Step 3: Background Neutralization ---\n";
         BackgroundNeutralize( image );
         std::cout << "\n";
      }
      else
         std::cout << "--- Step 3: Background Neutralization (SKIPPED) ---\n\n";

      // ---------------------------------------------------------------
      // Pipeline Step 4: Color Calibration (SDSS or Gaia SPCC)
      // ---------------------------------------------------------------
      if ( doSDSS )
      {
         std::cout << "--- Step 4: SDSS Flux Calibration ---\n";
         SDSSColorCalibrate( image, keywords );
         std::cout << "\n";
      }
      else if ( doSPCC )
      {
         std::cout << "--- Step 4: Spectrophotometric Color Calibration (Gaia) ---\n";
         SpectrophotometricColorCalibrate( image, keywords, properties, gaiaDbPaths );
         std::cout << "\n";
      }
      else
         std::cout << "--- Step 4: Color Calibration (SKIPPED) ---\n\n";

      // ---------------------------------------------------------------
      // Save calibrated linear result before stretching
      // ---------------------------------------------------------------
      std::cout << "--- Saving Result (linear) ---\n";
      {
         String pclOutputPath = String::UTF8ToUTF16( outputPath.c_str() );

         XISFWriter writer;
         writer.Create( pclOutputPath, 1 );

         ImageOptions xisfOptions;
         xisfOptions.bitsPerSample = 32;
         xisfOptions.ieeefpSampleFormat = true;
         writer.SetImageOptions( xisfOptions );

         writer.WriteFITSKeywords( keywords );
         for ( const Property& p : properties )
            writer.WriteImageProperty( p.Id(), p.Value() );

         writer.WriteImageProperty( "Processing:GalaxyProcessor:Version", Variant( IsoString( "1.0.0" ) ) );
         if ( doBgNeutralize )
            writer.WriteImageProperty( "Processing:GalaxyProcessor:BackgroundNeutralization", Variant( true ) );
         if ( doSPCC )
            writer.WriteImageProperty( "Processing:GalaxyProcessor:SPCC", Variant( true ) );

         writer.WriteImage( image );
         writer.Close();

         std::cout << "Output saved to: " << outputPath << "\n\n";
      }

      // ---------------------------------------------------------------
      // Pipeline Step 5: VeraLux HyperMetric Stretch + Wavelet Enhancement
      // (applied after saving linear data — stretched result saved separately)
      // ---------------------------------------------------------------
      if ( doStretch )
      {
         // Re-neutralize background after SPCC: the correction factors shift
         // per-channel background levels, and the non-linear stretch would
         // amplify any residual colour imbalance in the background.
         std::cout << "--- Step 5a: Re-neutralize Background (pre-stretch) ---\n";
         BackgroundNeutralize( image );
         std::cout << "\n";

         if ( doOptimize )
         {
            // Compute field center of the cropped image for survey fetch.
            // g_solvedWCS uses original (uncropped) pixel coords, so add crop offset.
            double optCenterRA = tanWcs.crval1;
            double optCenterDec = tanWcs.crval2;
            double optPixscale = tanWcs.valid ? tanWcs.Resolution() * 3600.0 : 0;

            if ( g_solvedWCS )
            {
               double cx = ( w / 2.0 + g_cropOffsetX ) / 2.0 + 1.0;
               double cy = ( h / 2.0 + g_cropOffsetY ) / 2.0 + 1.0;
               QPointF cp( cx, cy );
               FITSImage::wcs_point sky;
               if ( g_solvedWCS->pixelToWCS( cp, sky ) )
               {
                  optCenterRA = sky.ra;
                  optCenterDec = sky.dec;
               }
            }

            std::cout << "--- Step 5+6: Optimize Stretch + Wavelet ---\n";
            RunOptimization( image, optIterations, tanWcs,
                             optCenterRA, optCenterDec, optPixscale );
            std::cout << "\n";
         }
         else
         {
            std::cout << "--- Step 5: VeraLux HyperMetric Stretch ---\n";
            VeraLuxParams vlParams;
            VeraLuxStretch( image, vlParams );
            std::cout << "\n";

            std::cout << "--- Step 6: Starlet Wavelet Enhancement ---\n";
            WaveletEnhance( image, vlParams );
            std::cout << "\n";
         }

         // Save stretched result — replace _stacked with _processed,
         // or append _stretched if no _stacked suffix
         std::string stretchedPath = outputPath;
         size_t stk = stretchedPath.rfind( "_stacked" );
         if ( stk != std::string::npos )
            stretchedPath.replace( stk, 8, "_processed" );
         else
         {
            size_t dot = stretchedPath.rfind( '.' );
            if ( dot != std::string::npos )
               stretchedPath.insert( dot, "_stretched" );
            else
               stretchedPath += "_stretched";
         }

         std::cout << "--- Saving Stretched Result ---\n";
         {
            String pclStretchedPath = String::UTF8ToUTF16( stretchedPath.c_str() );
            XISFWriter writer;
            writer.Create( pclStretchedPath, 1 );

            ImageOptions xisfOptions;
            xisfOptions.bitsPerSample = 32;
            xisfOptions.ieeefpSampleFormat = true;
            writer.SetImageOptions( xisfOptions );

            writer.WriteFITSKeywords( keywords );
            for ( const Property& p : properties )
               writer.WriteImageProperty( p.Id(), p.Value() );

            writer.WriteImageProperty( "Processing:GalaxyProcessor:Version", Variant( IsoString( "1.0.0" ) ) );
            writer.WriteImageProperty( "Processing:GalaxyProcessor:Stretched", Variant( true ) );

            writer.WriteImage( image );
            writer.Close();

            std::cout << "Stretched output saved to: " << stretchedPath << "\n";
         }
      }

      std::cout << "\nDone.\n";
   }
   catch ( const Exception& e )
   {
      std::cerr << "ERROR: " << e.Message().ToUTF8() << "\n";
      return 1;
   }
   catch ( const std::exception& e )
   {
      std::cerr << "ERROR: " << e.what() << "\n";
      return 1;
   }

   return 0;
}
