#include "gp_stacking.h"

bool EndsWithCI( const std::string& s, const char* suffix )
{
   size_t slen = s.size(), xlen = strlen( suffix );
   if ( slen < xlen ) return false;
   for ( size_t i = 0; i < xlen; ++i )
      if ( tolower( s[slen - xlen + i] ) != tolower( suffix[i] ) )
         return false;
   return true;
}

// ============================================================================
// Stacking front-end (requires STACK=1 build with OpenCV + cfitsio)
// ============================================================================

// Convert cv::Mat (float32, 1 or 3 channels) to pcl::Image
Image CvMatToPCLImage( const cv::Mat& mat )
{
   int w = mat.cols, h = mat.rows, nc = mat.channels();
   Image img( w, h, nc >= 3 ? ColorSpace::RGB : ColorSpace::Gray );
   if ( nc == 1 )
   {
      for ( int y = 0; y < h; y++ )
      {
         const float* row = mat.ptr<float>( y );
         for ( int x = 0; x < w; x++ )
            img( x, y, 0 ) = row[x];
      }
   }
   else
   {
      std::vector<cv::Mat> channels;
      cv::split( mat, channels );
      for ( int c = 0; c < std::min( nc, 3 ); c++ )
         for ( int y = 0; y < h; y++ )
         {
            const float* row = channels[c].ptr<float>( y );
            for ( int x = 0; x < w; x++ )
               img( x, y, c ) = row[x];
         }
   }
   return img;
}

// Convert pcl::Image to cv::Mat (float32)
cv::Mat PCLImageToCvMat( const Image& img )
{
   int w = img.Width(), h = img.Height(), nc = img.NumberOfChannels();
   if ( nc >= 3 )
   {
      std::vector<cv::Mat> channels( 3 );
      for ( int c = 0; c < 3; c++ )
      {
         channels[c] = cv::Mat( h, w, CV_32FC1 );
         for ( int y = 0; y < h; y++ )
         {
            float* row = channels[c].ptr<float>( y );
            for ( int x = 0; x < w; x++ )
               row[x] = img( x, y, c );
         }
      }
      cv::Mat result;
      cv::merge( channels, result );
      return result;
   }
   else
   {
      cv::Mat result( h, w, CV_32FC1 );
      for ( int y = 0; y < h; y++ )
      {
         float* row = result.ptr<float>( y );
         for ( int x = 0; x < w; x++ )
            row[x] = img( x, y, 0 );
      }
      return result;
   }
}

// Stack a list of single-channel FITS files, return result as cv::Mat.
cv::Mat StackChannel(
   const QStringList& paths,
   const StackParams& params,
   const char* label )
{
   std::cout << "  Stacking " << label << " (" << paths.size() << " frames)...\n";

   StackEngine engine;
   engine.preselectFrames( paths, params );
   engine.analyzeFrames( params );

   cv::Mat result = engine.stackFrames( params );
   if ( result.empty() )
      std::cerr << "    WARNING: " << label << " stack produced empty result\n";
   else
   {
      int aligned = 0;
      for ( auto& f : engine.frames() )
         if ( f.enabled && f.aligned ) aligned++;
      std::cout << "    " << label << ": " << aligned << "/" << paths.size()
                << " frames aligned, result " << result.cols << "x" << result.rows << "\n";
   }
   return result;
}

// LRGB stacking pipeline:
//  1. Extract luminance from each frame, align and sigma-clip stack for deep L
//  2. Drizzle (register-average) the RGB colour from each frame using the
//     luminance-derived homographies — luminance has the best star detection
//     so alignment is sharpest; colour is low-frequency so a simple mean suffices
//  3. Combine: L for detail, drizzled RGB for colour
Image StackFramesLRGB(
   const QStringList& inputPaths,
   const StackParams& params )
{
   std::cout << "\n--- LRGB Stacking Pipeline ---\n";

   // Phase 1: Build luminance frames and run full alignment + stack
   std::cout << "  Phase 1: Luminance alignment + stack...\n";
   QString tmpDir = QDir::currentPath() + "/galaxy_proc_lrgb";
   QDir dir( tmpDir );
   if ( !dir.exists() ) dir.mkpath( "." );

   // Keep a map from luminance path → original RGB path
   QStringList lumPaths;
   QStringList origPaths; // parallel to lumPaths
   for ( int i = 0; i < inputPaths.size(); i++ )
   {
      cv::Mat img = StackEngine::loadFits( inputPaths[i] );
      if ( img.empty() || img.channels() < 3 )
      {
         std::cerr << "    Skipping " << inputPaths[i].toStdString() << " (not RGB)\n";
         continue;
      }
      std::vector<cv::Mat> ch;
      cv::split( img, ch );
      cv::Mat lum = 0.2126f * ch[0] + 0.7152f * ch[1] + 0.0722f * ch[2];
      QString base = QFileInfo( inputPaths[i] ).completeBaseName();
      QString lumPath = dir.filePath( base + "_L.fits" );
      StackEngine::saveFits( lumPath, lum );
      lumPaths.append( lumPath );
      origPaths.append( inputPaths[i] );
   }

   if ( lumPaths.isEmpty() )
   {
      std::cerr << "  ERROR: No valid RGB frames\n";
      QDir( tmpDir ).removeRecursively();
      return Image();
   }

   // Align and stack luminance with full sigma-clipping
   StackEngine lumEngine;
   lumEngine.preselectFrames( lumPaths, params );
   lumEngine.analyzeFrames( params );
   cv::Mat stackL = lumEngine.stackFrames( params );

   if ( stackL.empty() )
   {
      std::cerr << "  ERROR: Luminance stack failed\n";
      QDir( tmpDir ).removeRecursively();
      return Image();
   }

   int aligned = 0;
   for ( auto& f : lumEngine.frames() )
      if ( f.enabled && f.aligned ) aligned++;
   int refIdx = lumEngine.referenceIndex();
   int w = stackL.cols, h = stackL.rows;
   std::cout << "    Luminance: " << aligned << "/" << lumPaths.size()
             << " frames aligned, result " << w << "x" << h << "\n";

   // Phase 2: Sigma-clipped RGB drizzle using luminance-derived homographies.
   // Two-pass: first pass computes mean+variance, second rejects outliers
   // (satellite trails, planes, cosmic rays) and recomputes the mean.
   std::cout << "  Phase 2: Drizzle RGB colour (sigma-clipped)...\n";
   const auto& lumFrames = lumEngine.frames();
   const float clipSigma = float( params.sigmaHigh );

   // Collect indices of usable frames
   std::vector<int> usableIdx;
   for ( int i = 0; i < (int)lumFrames.size(); i++ )
      if ( lumFrames[i].enabled && lumFrames[i].aligned )
         usableIdx.push_back( i );

   // Helper: warp a single frame's RGB, apply per-frame normalization
   // (gradient removal + scale/offset matching the luminance stack), and build
   // its validity mask.  Without this the RGB drizzle uses raw un-normalized
   // values while the L stack has had backgrounds subtracted, causing
   // L < colourL → dimming in the LRGB synthesis.
   auto warpFrame = [&]( int idx, cv::Mat& warped, cv::Mat& mask ) -> bool {
      cv::Mat rgb = StackEngine::loadFits( origPaths[idx] );
      if ( rgb.empty() || rgb.channels() < 3 )
         return false;

      const FrameInfo& fi = lumFrames[idx];

      // Apply per-channel gradient removal and scale/offset (same as prepareFrame)
      std::vector<cv::Mat> ch;
      cv::split( rgb, ch );
      int nc = std::min( (int)ch.size(), 3 );
      for ( int c = 0; c < nc; c++ )
      {
         if ( !fi.gradientCoeffs[c].empty() )
         {
            int deg = (int)std::round( ( -1 + std::sqrt( 1 + 8.0 * fi.gradientCoeffs[c].size() ) ) / 2.0 );
            cv::Mat grad = StackEngine::evalGradient( rgb.rows, rgb.cols, fi.gradientCoeffs[c], deg );
            ch[c] = fi.scale[c] * ( ch[c] - grad ) + fi.offset[c];
         }
         else
         {
            ch[c] = fi.scale[c] * ch[c] + fi.offset[c];
         }
      }
      cv::Mat normalized;
      cv::merge( ch, normalized );

      cv::warpPerspective( normalized, warped, fi.homography, cv::Size( w, h ),
                           cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar( 0, 0, 0 ) );
      cv::Mat ones = cv::Mat::ones( rgb.size(), CV_8UC1 );
      cv::warpPerspective( ones, mask, fi.homography, cv::Size( w, h ),
                           cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar( 0 ) );
      return true;
   };

   // Pass 1: accumulate sum and sum-of-squares for mean + variance
   cv::Mat sumR = cv::Mat::zeros( h, w, CV_64FC1 );
   cv::Mat sumG = cv::Mat::zeros( h, w, CV_64FC1 );
   cv::Mat sumB = cv::Mat::zeros( h, w, CV_64FC1 );
   cv::Mat sqR  = cv::Mat::zeros( h, w, CV_64FC1 );
   cv::Mat sqG  = cv::Mat::zeros( h, w, CV_64FC1 );
   cv::Mat sqB  = cv::Mat::zeros( h, w, CV_64FC1 );
   cv::Mat countMat = cv::Mat::zeros( h, w, CV_32SC1 );

   int drizzled = 0;
   for ( int idx : usableIdx )
   {
      cv::Mat warped, mask;
      if ( !warpFrame( idx, warped, mask ) )
         continue;

      std::vector<cv::Mat> ch;
      cv::split( warped, ch );

      cv::parallel_for_( cv::Range( 0, h ), [&]( const cv::Range& range ) {
         for ( int y = range.start; y < range.end; y++ )
         {
            const uchar* pm = mask.ptr<uchar>( y );
            const float* pR = ch[0].ptr<float>( y );
            const float* pG = ch[1].ptr<float>( y );
            const float* pB = ch[2].ptr<float>( y );
            double* sR = sumR.ptr<double>( y );
            double* sG = sumG.ptr<double>( y );
            double* sB = sumB.ptr<double>( y );
            double* qR = sqR.ptr<double>( y );
            double* qG = sqG.ptr<double>( y );
            double* qB = sqB.ptr<double>( y );
            int* cnt = countMat.ptr<int>( y );
            for ( int x = 0; x < w; x++ )
            {
               if ( pm[x] == 0 ) continue;
               double r = pR[x], g = pG[x], b = pB[x];
               sR[x] += r;  sG[x] += g;  sB[x] += b;
               qR[x] += r*r; qG[x] += g*g; qB[x] += b*b;
               cnt[x]++;
            }
         }
      });
      drizzled++;
   }

   std::cout << "    Pass 1: " << drizzled << " frames, computing statistics...\n";

   // Compute per-pixel mean and sigma
   cv::Mat meanR( h, w, CV_32FC1 ), meanG( h, w, CV_32FC1 ), meanB( h, w, CV_32FC1 );
   cv::Mat sigR( h, w, CV_32FC1 ),  sigG( h, w, CV_32FC1 ),  sigB( h, w, CV_32FC1 );

   cv::parallel_for_( cv::Range( 0, h ), [&]( const cv::Range& range ) {
      for ( int y = range.start; y < range.end; y++ )
      {
         const double* sR_ = sumR.ptr<double>( y );
         const double* sG_ = sumG.ptr<double>( y );
         const double* sB_ = sumB.ptr<double>( y );
         const double* qR_ = sqR.ptr<double>( y );
         const double* qG_ = sqG.ptr<double>( y );
         const double* qB_ = sqB.ptr<double>( y );
         const int* cnt = countMat.ptr<int>( y );
         float* mR = meanR.ptr<float>( y );
         float* mG = meanG.ptr<float>( y );
         float* mB = meanB.ptr<float>( y );
         float* dR = sigR.ptr<float>( y );
         float* dG = sigG.ptr<float>( y );
         float* dB = sigB.ptr<float>( y );
         for ( int x = 0; x < w; x++ )
         {
            if ( cnt[x] > 0 )
            {
               double n = cnt[x];
               mR[x] = float( sR_[x] / n );
               mG[x] = float( sG_[x] / n );
               mB[x] = float( sB_[x] / n );
               dR[x] = float( std::sqrt( std::max( 0.0, qR_[x] / n - double(mR[x]) * mR[x] ) ) );
               dG[x] = float( std::sqrt( std::max( 0.0, qG_[x] / n - double(mG[x]) * mG[x] ) ) );
               dB[x] = float( std::sqrt( std::max( 0.0, qB_[x] / n - double(mB[x]) * mB[x] ) ) );
            }
            else
            {
               mR[x] = mG[x] = mB[x] = 0;
               dR[x] = dG[x] = dB[x] = 0;
            }
         }
      }
   });

   // Pass 2: re-accumulate, rejecting pixels outside mean ± clipSigma*sigma
   sumR.setTo( 0 ); sumG.setTo( 0 ); sumB.setTo( 0 );
   countMat.setTo( 0 );
   long totalRejected = 0;

   for ( int idx : usableIdx )
   {
      cv::Mat warped, mask;
      if ( !warpFrame( idx, warped, mask ) )
         continue;

      std::vector<cv::Mat> ch;
      cv::split( warped, ch );

      std::atomic<long> frameRejected( 0 );
      cv::parallel_for_( cv::Range( 0, h ), [&]( const cv::Range& range ) {
         long localRej = 0;
         for ( int y = range.start; y < range.end; y++ )
         {
            const uchar* pm = mask.ptr<uchar>( y );
            const float* pR = ch[0].ptr<float>( y );
            const float* pG = ch[1].ptr<float>( y );
            const float* pB = ch[2].ptr<float>( y );
            const float* mR = meanR.ptr<float>( y );
            const float* mG = meanG.ptr<float>( y );
            const float* mB = meanB.ptr<float>( y );
            const float* dR = sigR.ptr<float>( y );
            const float* dG = sigG.ptr<float>( y );
            const float* dB = sigB.ptr<float>( y );
            double* sR_ = sumR.ptr<double>( y );
            double* sG_ = sumG.ptr<double>( y );
            double* sB_ = sumB.ptr<double>( y );
            int* cnt = countMat.ptr<int>( y );
            for ( int x = 0; x < w; x++ )
            {
               if ( pm[x] == 0 ) continue;
               float r = pR[x], g = pG[x], b = pB[x];
               // Reject if ANY channel is an outlier
               if ( std::abs( r - mR[x] ) > clipSigma * dR[x] ||
                    std::abs( g - mG[x] ) > clipSigma * dG[x] ||
                    std::abs( b - mB[x] ) > clipSigma * dB[x] )
               {
                  localRej++;
                  continue;
               }
               sR_[x] += r;
               sG_[x] += g;
               sB_[x] += b;
               cnt[x]++;
            }
         }
         frameRejected += localRej;
      });
      totalRejected += frameRejected.load();
   }

   double totalPixels = double( drizzled ) * w * h;
   std::cout << "    Pass 2: " << totalRejected << " pixels rejected ("
             << std::setprecision( 2 ) << std::fixed
             << ( 100.0 * totalRejected / totalPixels ) << "%)\n" << std::defaultfloat;

   // Clean up temp files
   QDir( tmpDir ).removeRecursively();

   // Phase 3: LRGB synthesis — deep L for detail, averaged RGB for colour
   // Zero out pixels with low coverage (rotation edges) so the downstream
   // smart-crop can detect and trim them.
   int minCoverage = std::max( 1, drizzled / 2 );

   Image output( w, h, ColorSpace::RGB );

   for ( int y = 0; y < h; y++ )
   {
      const float* pL = stackL.ptr<float>( y );
      const double* sR = sumR.ptr<double>( y );
      const double* sG = sumG.ptr<double>( y );
      const double* sB = sumB.ptr<double>( y );
      const int* cnt = countMat.ptr<int>( y );
      for ( int x = 0; x < w; x++ )
      {
         if ( cnt[x] < minCoverage )
         {
            output( x, y, 0 ) = 0;
            output( x, y, 1 ) = 0;
            output( x, y, 2 ) = 0;
            continue;
         }
         float L = pL[x];
         float n = float( cnt[x] );
         float R = float( sR[x] / n );
         float G = float( sG[x] / n );
         float B = float( sB[x] / n );
         // Scale colour so luminance matches the deep L stack
         float colL = 0.2126f * R + 0.7152f * G + 0.0722f * B;
         if ( colL > 1e-6f )
         {
            float s = L / colL;
            R *= s; G *= s; B *= s;
         }
         output( x, y, 0 ) = std::max( 0.0f, R );
         output( x, y, 1 ) = std::max( 0.0f, G );
         output( x, y, 2 ) = std::max( 0.0f, B );
      }
   }

   std::cout << "  LRGB combine: " << w << "x" << h << " output\n";
   return output;
}

// Simple RGB stacking (no LRGB decomposition)
Image StackFramesRGB(
   const QStringList& inputPaths,
   const StackParams& params )
{
   std::cout << "\n--- RGB Stacking ---\n";

   StackEngine engine;
   engine.preselectFrames( inputPaths, params );
   engine.analyzeFrames( params );

   cv::Mat result = engine.stackFrames( params );
   if ( result.empty() )
   {
      std::cerr << "  ERROR: Stacking failed\n";
      return Image();
   }

   int aligned = 0;
   for ( auto& f : engine.frames() )
      if ( f.enabled && f.aligned ) aligned++;
   std::cout << "  " << aligned << "/" << inputPaths.size()
             << " frames aligned, result " << result.cols << "x" << result.rows << "\n";

   return CvMatToPCLImage( result );
}
