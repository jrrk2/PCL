#include "gp_survey.h"

float opt_mtf( float v, float m )
{
   if ( v <= 0 ) return 0;
   if ( v >= 1 ) return 1;
   float num = ( m - 1 ) * v;
   float den = ( 2 * m - 1 ) * v - m;
   if ( std::abs( den ) < 1e-9f ) return v;
   return std::max( 0.0f, std::min( 1.0f, num / den ) );
}

bool FetchSurveyReference(
   Image& refImage,
   const SimpleTANWCS& wcs,
   int inputW, int inputH,
   double centerRA, double centerDec,
   double pixscaleArcsec,
   double targetBg,
   const char* layer )
{
   if ( !wcs.valid || pixscaleArcsec <= 0 )
   {
      std::cerr << "  ERROR: No valid WCS for survey fetch\n";
      return false;
   }

   // Use a fixed-size survey tile so the same download can be reused across
   // different crop/trim passes.  The 3000px tile at the input pixscale gives
   // ample margin for any reasonable crop of the input field.
   int surveySize = 3000;  // fixed tile — Legacy Survey max

   // Build URL — fetch FITS cutout (uncompressed linear flux in nanomaggies)
   char url[1024];
   snprintf( url, sizeof( url ),
      "https://www.legacysurvey.org/viewer/cutout.fits?ra=%.6f&dec=%.6f&size=%d&layer=%s&pixscale=%.3f",
      centerRA, centerDec, surveySize, layer, pixscaleArcsec );

   std::cout << "  Fetching survey FITS: " << layer << " " << surveySize << "px at "
             << std::setprecision( 3 ) << pixscaleArcsec << "\"/px\n";

   // Cache survey tiles — round center to nearest 100" to encourage reuse
   // across runs with slightly different plate solutions or crops.
   // 100" = 0.02778°, well within the 3000px tile margin.
   double cacheRA  = std::round( centerRA  * 36.0 ) / 36.0;  // 100" = 1/36 deg
   double cacheDec = std::round( centerDec * 36.0 ) / 36.0;
   char cacheFile[512];
   snprintf( cacheFile, sizeof( cacheFile ),
      "galaxy_proc_survey_%s_%.4f_%+.4f_%.3f.fits",
      layer, cacheRA, cacheDec, pixscaleArcsec );
   std::string tmpPath = cacheFile;

   // Check cache
   QFileInfo cacheInfo( QString::fromStdString( tmpPath ) );
   if ( cacheInfo.exists() && cacheInfo.size() > 2880 )
   {
      std::cout << "  Using cached survey: " << tmpPath << "\n";
   }
   else
   {
      char curlCmd[2048];
      snprintf( curlCmd, sizeof( curlCmd ),
         "curl -s --max-time 120 -o '%s' '%s' 2>/dev/null", tmpPath.c_str(), url );

      int exitCode = system( curlCmd );
      if ( exitCode != 0 )
      {
         std::cerr << "  ERROR: curl failed (exit " << exitCode << ")\n";
         return false;
      }
      std::cout << "  Downloaded survey: " << tmpPath << "\n";
   }

   // Read FITS cutout — Legacy Survey returns multi-HDU: one image per band (g, r, z)
   fitsfile* fptr = nullptr;
   int fitsStatus = 0;
   fits_open_file( &fptr, tmpPath.c_str(), READONLY, &fitsStatus );
   if ( fitsStatus )
   {
      std::cerr << "  ERROR: Failed to open survey FITS\n";
      return false;
   }

   // Get dimensions from primary HDU
   int naxis = 0;
   fits_get_img_dim( fptr, &naxis, &fitsStatus );
   long naxes[3] = { 1, 1, 1 };
   fits_get_img_size( fptr, 3, naxes, &fitsStatus );
   int survW = int( naxes[0] ), survH = int( naxes[1] );
   int nBands = ( naxis >= 3 ) ? int( naxes[2] ) : 1;

   std::cout << "  Survey FITS: " << survW << "x" << survH << " (" << nBands << " bands)\n";

   // Read all bands into float arrays.
   // Legacy Survey FITS cutout: 3D cube [W x H x bands] where bands = g, r, z.
   // We map g→B, r→G, z→R for approximate visual colour.
   int npix = survW * survH;
   std::vector<std::vector<float>> bands( nBands, std::vector<float>( npix ) );
   if ( naxis >= 3 )
   {
      // 3D cube — read each band plane
      for ( int b = 0; b < nBands; b++ )
      {
         long fpixel[3] = { 1, 1, b + 1 };
         fits_read_pix( fptr, TFLOAT, fpixel, npix, nullptr, bands[b].data(), nullptr, &fitsStatus );
      }
   }
   else
   {
      // Single 2D image
      long fpixel[2] = { 1, 1 };
      fits_read_pix( fptr, TFLOAT, fpixel, npix, nullptr, bands[0].data(), nullptr, &fitsStatus );
   }
   fits_close_file( fptr, &fitsStatus );

   // Map bands to RGB: g→B, r→G, z→R (Legacy Survey band order)
   std::vector<float>& survR = ( nBands >= 3 ) ? bands[2] : bands[0];
   std::vector<float>& survG = ( nBands >= 2 ) ? bands[1] : bands[0];
   std::vector<float>& survB = bands[0];

   // Normalize flux to [0,1] — find a robust max (99.9th percentile)
   {
      std::vector<float> allFlux;
      allFlux.reserve( npix * 3 );
      for ( int i = 0; i < npix; i++ )
      {
         float r = survR[i], g = survG[i], b = survB[i];
         if ( r == r ) allFlux.push_back( r );
         if ( g == g ) allFlux.push_back( g );
         if ( b == b ) allFlux.push_back( b );
      }
      std::sort( allFlux.begin(), allFlux.end() );
      float fluxMax = allFlux[int( allFlux.size() * 0.999 )];
      float fluxMin = allFlux[int( allFlux.size() * 0.001 )];
      if ( fluxMax <= fluxMin ) fluxMax = fluxMin + 1;
      float scale = 1.0f / ( fluxMax - fluxMin );
      std::cout << "  Survey flux range: " << fluxMin << " to " << fluxMax << " nanomaggies\n";
      for ( int i = 0; i < npix; i++ )
      {
         survR[i] = std::max( 0.0f, std::min( 1.0f, ( survR[i] - fluxMin ) * scale ) );
         survG[i] = std::max( 0.0f, std::min( 1.0f, ( survG[i] - fluxMin ) * scale ) );
         survB[i] = std::max( 0.0f, std::min( 1.0f, ( survB[i] - fluxMin ) * scale ) );
      }
   }

   // Read survey WCS from FITS header
   double survCrpix1 = 0, survCrpix2 = 0;
   double survCrval1 = 0, survCrval2 = 0;
   double survCd11 = 0, survCd12 = 0, survCd21 = 0, survCd22 = 0;
   {
      fitsfile* wfptr = nullptr;
      int ws = 0;
      fits_open_file( &wfptr, tmpPath.c_str(), READONLY, &ws );
      if ( !ws )
      {
         fits_read_key( wfptr, TDOUBLE, "CRPIX1", &survCrpix1, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CRPIX2", &survCrpix2, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CRVAL1", &survCrval1, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CRVAL2", &survCrval2, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CD1_1", &survCd11, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CD1_2", &survCd12, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CD2_1", &survCd21, nullptr, &ws ); ws = 0;
         fits_read_key( wfptr, TDOUBLE, "CD2_2", &survCd22, nullptr, &ws ); ws = 0;
         fits_close_file( wfptr, &ws );
      }
   }

   // Reproject: for each output pixel, map input_pixel → sky → survey_pixel
   // using proper TAN gnomonic projection via SimpleTANWCS methods.
   // Build a SimpleTANWCS for the survey tile
   SimpleTANWCS survWcs;
   survWcs.crval1 = survCrval1; survWcs.crval2 = survCrval2;
   survWcs.crpix1 = survCrpix1; survWcs.crpix2 = survCrpix2;
   survWcs.cd11 = survCd11; survWcs.cd12 = survCd12;
   survWcs.cd21 = survCd21; survWcs.cd22 = survCd22;
   survWcs.valid = true;

   int outW = inputW, outH = inputH;
   refImage = Image( outW, outH, ColorSpace::RGB );
   refImage.Zero();

   std::cout << "  Reprojecting survey to match input WCS...\n";

   for ( int oy = 0; oy < outH; oy++ )
   {
      for ( int ox = 0; ox < outW; ox++ )
      {
         // Input pixel → sky using StellarSolver's WCS if available (most accurate),
         // otherwise fall back to SimpleTANWCS (CRPIX in PCL convention: 0-indexed, y=0 at top)
         double ra, dec;
         if ( g_solvedWCS )
         {
            // Convert cropped pixel coords back to original frame for g_solvedWCS
            QPointF pixel( ( double( ox ) + g_cropOffsetX ) / 2.0 + 1.0,
                           ( double( oy ) + g_cropOffsetY ) / 2.0 + 1.0 );
            FITSImage::wcs_point sky;
            if ( !g_solvedWCS->pixelToWCS( pixel, sky ) )
               continue;
            ra = sky.ra;
            dec = sky.dec;
         }
         else if ( !wcs.PixelToWorld( double( ox ), double( oy ), ra, dec ) )
            continue;

         // Sky → survey pixel via proper TAN projection
         double survPx, survPy;
         if ( !survWcs.WorldToPixel( ra, dec, survPx, survPy ) )
            continue;

         // cfitsio array: index 0 = FITS row 1 (bottom), so array coords = fitsCoord - 1
         double sx = survPx - 1.0;
         double sy = survPy - 1.0;

         int x0 = int( std::floor( sx ) );
         int y0 = int( std::floor( sy ) );
         if ( x0 >= 0 && y0 >= 0 && x0 < survW - 1 && y0 < survH - 1 )
         {
            float fx = float( sx - x0 ), fy = float( sy - y0 );
            float w00 = ( 1 - fx ) * ( 1 - fy ), w10 = fx * ( 1 - fy );
            float w01 = ( 1 - fx ) * fy, w11 = fx * fy;
            int i00 = y0 * survW + x0;
            refImage( ox, oy, 0 ) = w00 * survR[i00] + w10 * survR[i00 + 1]
                                  + w01 * survR[i00 + survW] + w11 * survR[i00 + survW + 1];
            refImage( ox, oy, 1 ) = w00 * survG[i00] + w10 * survG[i00 + 1]
                                  + w01 * survG[i00 + survW] + w11 * survG[i00 + survW + 1];
            refImage( ox, oy, 2 ) = w00 * survB[i00] + w10 * survB[i00 + 1]
                                  + w01 * survB[i00 + survW] + w11 * survB[i00 + survW + 1];
         }
      }
   }

   // Save reprojected reference for visual verification
   {
      String convergent = String::UTF8ToUTF16( "galaxy_proc_survey_reprojected.xisf" );
      XISFWriter convergentWriter;
      convergentWriter.Create( convergent, 1 );
      ImageOptions convergentOptions;
      convergentOptions.bitsPerSample = 32;
      convergentOptions.ieeefpSampleFormat = true;
      convergentWriter.SetImageOptions( convergentOptions );
      convergentWriter.WriteImage( refImage );
      convergentWriter.Close();
      std::cout << "  Saved reprojected survey: galaxy_proc_survey_reprojected.xisf\n";
   }

   // Self-check: verify reprojection via coverage and cross-correlation sharpness.
   // Avoids plate-solving which can hang on FWHM-distorted survey stars.
   {
      std::cout << "  Verifying reprojection alignment...\n";

      // 1. Coverage: fraction of output pixels that received survey data
      int nonZero = 0;
      int totalPix = outW * outH;
      for ( int y = 0; y < outH; y++ )
         for ( int x = 0; x < outW; x++ )
            if ( refImage( x, y, 0 ) > 0 || refImage( x, y, 1 ) > 0 || refImage( x, y, 2 ) > 0 )
               nonZero++;
      double coverage = 100.0 * nonZero / totalPix;

      // 2. Downsample both images to luminance at 1/4 resolution for cross-correlation
      int dsW = outW / 4, dsH = outH / 4;
      std::vector<float> refLum( dsW * dsH, 0 ), inpLum( dsW * dsH, 0 );
      for ( int y = 0; y < dsH; y++ )
         for ( int x = 0; x < dsW; x++ )
         {
            int sx = x * 4, sy = y * 4;
            float r = refImage( sx, sy, 0 ), g = refImage( sx, sy, 1 ), b = refImage( sx, sy, 2 );
            // -ffast-math safe: use v != v instead of std::isnan
            r = ( r != r ) ? 0 : r;
            g = ( g != g ) ? 0 : g;
            b = ( b != b ) ? 0 : b;
            refLum[y * dsW + x] = 0.2126f * r + 0.7152f * g + 0.0722f * b;
         }

      // 3. Compute mean and stddev of reprojected luminance (non-zero pixels only)
      double refSum = 0, refSum2 = 0;
      int refN = 0;
      for ( int i = 0; i < dsW * dsH; i++ )
      {
         float v = refLum[i];
         if ( v != v || v <= 0 || v > 1e6f ) continue; // skip NaN/zero/outlier
         refSum += v;
         refSum2 += double( v ) * v;
         refN++;
      }
      double refMean = refN > 0 ? refSum / refN : 0;
      double refStd = refN > 1 ? std::sqrt( refSum2 / refN - refMean * refMean ) : 0;

      // 4. Sharpness: ratio of high-frequency energy to total energy via Laplacian
      //    A blurry (misaligned) reprojection will have low sharpness.
      double lapSum = 0, pixSum = 0;
      for ( int y = 1; y < dsH - 1; y++ )
         for ( int x = 1; x < dsW - 1; x++ )
         {
            float c = refLum[y * dsW + x];
            if ( c <= 0 ) continue;
            float lap = 4 * c - refLum[( y - 1 ) * dsW + x] - refLum[( y + 1 ) * dsW + x]
                              - refLum[y * dsW + x - 1] - refLum[y * dsW + x + 1];
            lapSum += std::abs( lap );
            pixSum += c;
         }
      double sharpness = pixSum > 0 ? lapSum / pixSum : 0;

      // 5. Print WCS info
      double inputDet = wcs.cd11 * wcs.cd22 - wcs.cd12 * wcs.cd21;
      double inputRot = ( inputDet < 0 )
         ? std::atan2( wcs.cd21, wcs.cd22 ) * 180.0 / M_PI
         : std::atan2( -wcs.cd12, wcs.cd22 ) * 180.0 / M_PI;
      double inputScale = wcs.Resolution() * 3600.0;

      std::cout << "    Input  WCS: RA=" << std::setprecision( 4 ) << std::fixed << wcs.crval1
                << " Dec=" << wcs.crval2 << " rot=" << inputRot << "° scale=" << inputScale << "\"/px"
                << " CD=[" << std::setprecision( 6 ) << std::scientific
                << wcs.cd11 << ", " << wcs.cd12 << "; " << wcs.cd21 << ", " << wcs.cd22 << "]\n";
      std::cout << std::fixed
                << "    Coverage: " << std::setprecision( 1 ) << coverage << "%"
                << "  mean=" << std::setprecision( 4 ) << refMean
                << "  std=" << refStd
                << "  sharpness=" << std::setprecision( 3 ) << sharpness << "\n"
                << std::defaultfloat;

      if ( coverage < 50.0 )
         std::cerr << "  WARNING: Low coverage (" << std::setprecision( 1 ) << coverage
                   << "%) — survey may not overlap input field\n";
      if ( sharpness < 0.01 )
         std::cerr << "  WARNING: Low sharpness — reprojection may be misaligned or blurred\n";
      if ( coverage >= 50.0 && sharpness >= 0.01 )
         std::cout << "    Reprojection verified OK\n";
   }

   // Normalize reference luminosity to match VeraLux target background via MTF
   int N = outW * outH;
   int step = std::max( 1, N / 500000 );
   std::vector<float> refLum;
   refLum.reserve( N / step + 1 );
   for ( int i = 0; i < N; i += step )
   {
      int x = i % outW, y = i / outW;
      refLum.push_back( 0.2126f * refImage( x, y, 0 )
                       + 0.7152f * refImage( x, y, 1 )
                       + 0.0722f * refImage( x, y, 2 ) );
   }
   std::sort( refLum.begin(), refLum.end() );
   float medLum = refLum[refLum.size() / 2];

   if ( medLum > 0.001f && medLum < 0.999f && std::abs( medLum - float( targetBg ) ) >= 0.01f )
   {
      float tb = float( targetBg );
      float m = ( medLum * ( tb - 1 ) ) / ( medLum * ( 2 * tb - 1 ) - tb );
      if ( std::isfinite( m ) && m > 0 )
      {
         std::cout << "  Normalizing reference: median " << std::setprecision( 3 ) << medLum
                   << " → " << tb << " (MTF m=" << std::setprecision( 4 ) << m << ")\n";
         for ( int y = 0; y < outH; y++ )
            for ( int x = 0; x < outW; x++ )
               for ( int c = 0; c < 3; c++ )
                  refImage( x, y, c ) = opt_mtf( refImage( x, y, c ), m );
      }
   }

   std::cout << "  Survey reference ready: " << outW << "x" << outH << "\n";
   return true;
}

// ============================================================================
// Optimization loop: iterate VeraLux/wavelet params against survey reference
// ============================================================================

void RunOptimization( Image& image, int maxIterations, const SimpleTANWCS& wcs,
                      double centerRA, double centerDec, double pixscaleArcsec )
{
   std::cout << "\n=== OPTIMIZATION MODE (" << maxIterations << " iterations) ===\n\n";

   // Fetch survey reference at default target_bg
   VeraLuxParams defaultParams;
   Image refImage;
   if ( !FetchSurveyReference( refImage, wcs,
         image.Width(), image.Height(),
         centerRA, centerDec, pixscaleArcsec,
         defaultParams.target_bg ) )
   {
      std::cerr << "  ERROR: Cannot fetch survey reference — optimization aborted\n";
      return;
   }

   ImageHistogram refHist = compute_histogram( refImage );

   // Keep a copy of the calibrated (pre-stretch) image to re-stretch each iteration
   Image calibrated( image );

   // Parameter ranges
   struct ParamRange { double* ptr; const char* name; double lo; double hi; };
   VeraLuxParams vlParams;
   ParamRange ranges[] = {
      { &vlParams.target_bg,        "vl.target",      0.04,  0.15 },
      { &vlParams.protect_b,        "vl.b",           1.0,  12.0  },
      { &vlParams.convergence,      "vl.conv",        1.5,   6.0  },
      { &vlParams.wavelet_gain[1],  "wavelet.gain1",  0.0,   2.5  },
      { &vlParams.wavelet_gain[2],  "wavelet.gain2",  0.0,   3.0  },
      { &vlParams.wavelet_gain[3],  "wavelet.gain3",  0.0,   3.5  },
      { &vlParams.wavelet_gain[4],  "wavelet.gain4",  0.0,   2.5  },
      { &vlParams.wavelet_residual, "wavelet.resid",  0.9,   1.8  },
      { &vlParams.wavelet_mask_lo_mad, "wavelet.maskLo", 0.5, 3.0 },
      { &vlParams.wavelet_mask_hi_mad, "wavelet.maskHi", 1.5, 8.0 },
   };
   int numParams = sizeof( ranges ) / sizeof( ranges[0] );

   // Save defaults
   std::vector<double> defaults( numParams );
   for ( int p = 0; p < numParams; p++ )
      defaults[p] = *ranges[p].ptr;

   float bestScore = 1e6f;
   std::vector<double> bestParams = defaults;
   int bestIter = -1;

   std::mt19937 rng( 42 );
   std::uniform_real_distribution<double> uni( 0.0, 1.0 );

   for ( int iter = 0; iter < maxIterations; iter++ )
   {
      std::cout << "\n========== Iteration " << iter + 1 << "/" << maxIterations << " ==========\n";

      // Generate parameters
      if ( iter == 0 )
      {
         for ( int p = 0; p < numParams; p++ )
            *ranges[p].ptr = defaults[p];
      }
      else if ( iter <= numParams )
      {
         // Systematic: perturb one parameter from best
         for ( int p = 0; p < numParams; p++ )
            *ranges[p].ptr = bestParams[p];
         int pidx = ( iter - 1 ) % numParams;
         double range = ranges[pidx].hi - ranges[pidx].lo;
         double offset = ( iter % 2 == 0 ) ? range * 0.2 : -range * 0.2;
         *ranges[pidx].ptr = std::min( ranges[pidx].hi,
            std::max( ranges[pidx].lo, bestParams[pidx] + offset ) );
      }
      else if ( iter <= numParams * 2 )
      {
         // Systematic: opposite direction
         for ( int p = 0; p < numParams; p++ )
            *ranges[p].ptr = bestParams[p];
         int pidx = ( iter - numParams - 1 ) % numParams;
         double range = ranges[pidx].hi - ranges[pidx].lo;
         double offset = ( iter % 2 == 0 ) ? -range * 0.3 : range * 0.3;
         *ranges[pidx].ptr = std::min( ranges[pidx].hi,
            std::max( ranges[pidx].lo, bestParams[pidx] + offset ) );
      }
      else
      {
         // Random perturbation around best — shrinking radius
         double temperature = std::max( 0.1, 1.0 - double( iter ) / double( maxIterations ) );
         for ( int p = 0; p < numParams; p++ )
         {
            double range = ranges[p].hi - ranges[p].lo;
            double noise = ( uni( rng ) * 2.0 - 1.0 ) * range * 0.3 * temperature;
            *ranges[p].ptr = std::min( ranges[p].hi,
               std::max( ranges[p].lo, bestParams[p] + noise ) );
         }
      }

      // Print current parameters
      std::cout << "  Params:";
      for ( int p = 0; p < numParams; p++ )
         std::cout << " " << ranges[p].name << "=" << std::setprecision( 3 ) << *ranges[p].ptr;
      std::cout << "\n";

      // Run stretch + wavelet on a fresh copy of the calibrated image
      Image trial( calibrated );
      VeraLuxStretch( trial, vlParams );
      WaveletEnhance( trial, vlParams );

      // Score against reference
      ImageHistogram candHist = compute_histogram( trial );
      float score = histogram_distance( candHist, refHist );

      std::cout << "  Score: " << std::setprecision( 4 ) << std::fixed << score
                << " (best: " << bestScore << " at iter " << bestIter + 1 << ")\n";
      std::cout << std::defaultfloat;

      if ( score < bestScore )
      {
         bestScore = score;
         bestIter = iter;
         for ( int p = 0; p < numParams; p++ )
            bestParams[p] = *ranges[p].ptr;
         image = trial; // Keep best result
         std::cout << "  >>> NEW BEST! <<<\n";
      }
   }

   // Final report
   std::cout << "\n=== OPTIMIZATION COMPLETE ===\n";
   std::cout << "Best score: " << std::setprecision( 4 ) << std::fixed << bestScore
             << " (iteration " << bestIter + 1 << ")\n" << std::defaultfloat;
   std::cout << "Best parameters:\n";
   for ( int p = 0; p < numParams; p++ )
      std::cout << "  --" << ranges[p].name << "=" << std::setprecision( 4 ) << bestParams[p] << "\n";

   // Apply best params to the output image (already done via image = trial above)
}
