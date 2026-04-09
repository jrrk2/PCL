#include "gp_color_calibration.h"
#include "gp_plate_solver.h"

// Red channel: IMX178 Red Bayer
const FilterPoint s_filterR[] = {
   { 400, 0.09 }, { 410, 0.07 }, { 420, 0.04 }, { 430, 0.03 },
   { 440, 0.02 }, { 450, 0.02 }, { 460, 0.02 }, { 470, 0.03 },
   { 480, 0.03 }, { 490, 0.04 }, { 500, 0.05 }, { 510, 0.06 },
   { 520, 0.07 }, { 530, 0.07 }, { 540, 0.06 }, { 550, 0.05 },
   { 560, 0.06 }, { 570, 0.20 }, { 580, 0.55 }, { 590, 0.85 },
   { 600, 0.93 }, { 610, 0.92 }, { 620, 0.90 }, { 630, 0.87 },
   { 640, 0.83 }, { 650, 0.78 }, { 660, 0.72 }, { 670, 0.68 },
   { 680, 0.64 }, { 690, 0.64 }, { 700, 0.65 }
};

#if 0 // Sony Color Sensor UVIRcut from PixInsight filters.xspd (default SPCC curves)
static const FilterPoint s_filterR_sony[] = {
   { 400, 0.088 }, { 420, 0.052 }, { 440, 0.030 }, { 460, 0.021 },
   { 480, 0.023 }, { 500, 0.036 }, { 520, 0.057 }, { 540, 0.050 },
   { 560, 0.038 }, { 580, 0.433 }, { 600, 0.892 }, { 620, 0.870 },
   { 640, 0.846 }, { 660, 0.799 }, { 680, 0.704 }, { 700, 0.649 }
};
#endif

// Green channel: IMX178 Green Bayer
const FilterPoint s_filterG[] = {
   { 400, 0.07 }, { 410, 0.05 }, { 420, 0.03 }, { 430, 0.03 },
   { 440, 0.04 }, { 450, 0.05 }, { 460, 0.08 }, { 470, 0.20 },
   { 480, 0.45 }, { 490, 0.75 }, { 500, 0.90 }, { 510, 0.98 },
   { 520, 1.00 }, { 530, 1.00 }, { 540, 0.97 }, { 550, 0.92 },
   { 560, 0.85 }, { 570, 0.75 }, { 580, 0.65 }, { 590, 0.50 },
   { 600, 0.35 }, { 610, 0.25 }, { 620, 0.20 }, { 630, 0.18 },
   { 640, 0.16 }, { 650, 0.15 }, { 660, 0.15 }, { 670, 0.18 },
   { 680, 0.22 }, { 690, 0.26 }, { 700, 0.29 }
};

// Blue channel: IMX178 Blue Bayer
const FilterPoint s_filterB[] = {
   { 400, 0.45 }, { 410, 0.55 }, { 420, 0.62 }, { 430, 0.68 },
   { 440, 0.74 }, { 450, 0.78 }, { 460, 0.80 }, { 470, 0.78 },
   { 480, 0.70 }, { 490, 0.60 }, { 500, 0.48 }, { 510, 0.35 },
   { 520, 0.25 }, { 530, 0.18 }, { 540, 0.12 }, { 550, 0.08 },
   { 560, 0.05 }, { 570, 0.04 }, { 580, 0.03 }, { 590, 0.03 },
   { 600, 0.03 }, { 610, 0.02 }, { 620, 0.02 }, { 630, 0.03 },
   { 640, 0.04 }, { 650, 0.05 }, { 660, 0.06 }, { 670, 0.07 },
   { 680, 0.07 }, { 690, 0.07 }, { 700, 0.07 }
};

const int s_filterRCount = sizeof( s_filterR ) / sizeof( s_filterR[0] );
const int s_filterGCount = sizeof( s_filterG ) / sizeof( s_filterG[0] );
const int s_filterBCount = sizeof( s_filterB ) / sizeof( s_filterB[0] );

// ============================================================================
// Interpolate filter transmission at a given wavelength (linear interp)
// ============================================================================

double InterpolateFilter( const FilterPoint* filter, int count, double wavelength )
{
   if ( wavelength <= filter[0].wavelength )
      return 0.0;
   if ( wavelength >= filter[count - 1].wavelength )
      return 0.0;

   for ( int i = 0; i < count - 1; ++i )
   {
      if ( wavelength >= filter[i].wavelength && wavelength <= filter[i + 1].wavelength )
      {
         double t = ( wavelength - filter[i].wavelength ) / ( filter[i + 1].wavelength - filter[i].wavelength );
         return filter[i].transmission + t * ( filter[i + 1].transmission - filter[i].transmission );
      }
   }
   return 0.0;
}

// ============================================================================
// Convolve a Gaia BP/RP spectrum with a filter curve using trapezoidal integration
// Returns the integrated flux through the filter
// ============================================================================

double ConvolveSpectrumWithFilter(
   const FVector& spectrum, float spectrumStart, float spectrumStep, int spectrumCount,
   const FilterPoint* filter, int filterCount )
{
   double integral = 0.0;
   double normalization = 0.0;

   for ( int i = 0; i < spectrumCount - 1; ++i )
   {
      double wl0 = spectrumStart + i * spectrumStep;
      double wl1 = spectrumStart + ( i + 1 ) * spectrumStep;
      double f0 = InterpolateFilter( filter, filterCount, wl0 );
      double f1 = InterpolateFilter( filter, filterCount, wl1 );
      double s0 = spectrum[i];
      double s1 = spectrum[i + 1];
      double dw = wl1 - wl0;

      // Trapezoidal rule: integral of spectrum * filter
      integral += 0.5 * ( s0 * f0 + s1 * f1 ) * dw;
      normalization += 0.5 * ( f0 + f1 ) * dw;
   }

   if ( normalization > 0 )
      return integral / normalization;
   return 0.0;
}

// ============================================================================
// Robust median using std::nth_element
// ============================================================================

double Median( std::vector<double>& v )
{
   if ( v.empty() )
      return 0;
   size_t n = v.size();
   size_t mid = n / 2;
   std::nth_element( v.begin(), v.begin() + mid, v.end() );
   if ( n % 2 == 0 )
   {
      double a = v[mid];
      std::nth_element( v.begin(), v.begin() + mid - 1, v.end() );
      return ( a + v[mid - 1] ) * 0.5;
   }
   return v[mid];
}

// ============================================================================
// Median Absolute Deviation
// ============================================================================

double MAD( std::vector<double>& v, double median )
{
   std::vector<double> dev( v.size() );
   for ( size_t i = 0; i < v.size(); ++i )
      dev[i] = std::abs( v[i] - median );
   return Median( dev );
}

// ============================================================================
// Smart crop of stacking edges
// Field-rotation stacking creates irregular boundaries where valid data
// transitions abruptly to zero. We find the largest inscribed rectangle
// where every row and column is fully populated with valid data.
// ============================================================================

void SmartCropStackingEdges( Image& image, int& cropX0, int& cropY0 )
{
   int w = image.Width();
   int h = image.Height();
   int ch = std::min( image.NumberOfChannels(), 3 );
   const double threshold = 1e-6;

   // Build a validity mask: true where pixel has signal
   std::vector<bool> valid( w * h );
   int totalValid = 0;
   for ( int y = 0; y < h; ++y )
      for ( int x = 0; x < w; ++x )
      {
         double lum = 0;
         for ( int c = 0; c < ch; ++c )
            lum += image.Pixel( x, y, c );
         bool v = lum > threshold;
         valid[y * w + x] = v;
         if ( v ) ++totalValid;
      }

   double validFraction = double( totalValid ) / ( w * h );
   std::cout << "  Valid pixel fraction: " << std::setprecision( 3 )
             << ( validFraction * 100 ) << "%\n";

   if ( validFraction > 0.99 )
   {
      std::cout << "  Image is fully populated - no crop needed\n";
      cropX0 = 0;
      cropY0 = 0;
      return;
   }

   // Find the largest inscribed circle using a Chamfer distance transform.
   // For alt-az field rotation, the valid region is roughly circular, so the
   // largest inscribed circle maximizes retained area. We then crop to the
   // bounding square of that circle.

   // Chamfer 3-4 distance transform (approximates Euclidean distance * 3)
   // All image border pixels are treated as invalid (distance 0) to ensure
   // the transform doesn't produce infinite distances at edges.
   std::vector<int> dist( w * h, 0 );
   for ( int y = 0; y < h; y++ )
      for ( int x = 0; x < w; x++ )
      {
         if ( y == 0 || y == h - 1 || x == 0 || x == w - 1 )
            dist[y * w + x] = 0;  // border always 0
         else
            dist[y * w + x] = valid[y * w + x] ? 30000 : 0;
      }

   // Forward pass (top-left to bottom-right)
   for ( int y = 1; y < h - 1; y++ )
      for ( int x = 1; x < w - 1; x++ )
      {
         int d = dist[y * w + x];
         if ( d == 0 ) continue;
         d = std::min( d, dist[( y - 1 ) * w + x - 1] + 4 );
         d = std::min( d, dist[( y - 1 ) * w + x    ] + 3 );
         d = std::min( d, dist[( y - 1 ) * w + x + 1] + 4 );
         d = std::min( d, dist[y * w + x - 1] + 3 );
         dist[y * w + x] = d;
      }

   // Backward pass (bottom-right to top-left)
   for ( int y = h - 2; y >= 1; y-- )
      for ( int x = w - 2; x >= 1; x-- )
      {
         int d = dist[y * w + x];
         if ( d == 0 ) continue;
         d = std::min( d, dist[( y + 1 ) * w + x + 1] + 4 );
         d = std::min( d, dist[( y + 1 ) * w + x    ] + 3 );
         d = std::min( d, dist[( y + 1 ) * w + x - 1] + 4 );
         d = std::min( d, dist[y * w + x + 1] + 3 );
         dist[y * w + x] = d;
      }

   // Find the pixel with maximum distance — center of largest inscribed circle
   int bestX = w / 2, bestY = h / 2;
   int maxDist = 0;
   for ( int y = 0; y < h; y++ )
      for ( int x = 0; x < w; x++ )
         if ( dist[y * w + x] > maxDist )
         {
            maxDist = dist[y * w + x];
            bestX = x;
            bestY = y;
         }

   // Chamfer 3-4 distance: divide by 3 to get approximate pixel radius
   int radius = maxDist / 3;

   std::cout << "  Largest inscribed circle: center=(" << bestX << "," << bestY
             << ") radius=" << radius << " px\n";

   if ( radius < 10 )
   {
      std::cout << "  Inscribed circle too small — no crop applied\n";
      cropX0 = 0;
      cropY0 = 0;
      return;
   }

   // Force crop centre to image centre — the telescope tracks the target so
   // the centre pixel is always valid and is the point of interest.
   bestX = w / 2;
   bestY = h / 2;

   // Limit crop to 1/sqrt(2) of original dimensions — the worst case for
   // 360° alt-az field rotation inscribing a square in a rotated rectangle.
   int minW = int( std::ceil( w / std::sqrt( 2.0 ) ) );
   int minH = int( std::ceil( h / std::sqrt( 2.0 ) ) );
   int maxRadius = std::min( { bestX, w - 1 - bestX, bestY, h - 1 - bestY, radius } );
   // Ensure the cropped square meets the minimum dimension
   int side = 2 * maxRadius + 1;
   if ( side < minW || side < minH )
   {
      int needR = std::max( ( minW - 1 ) / 2, ( minH - 1 ) / 2 );
      maxRadius = std::min( needR, std::min( { bestX, w - 1 - bestX, bestY, h - 1 - bestY } ) );
   }
   else
      maxRadius = radius;

   // Crop to the bounding square centred on image centre
   int left   = std::max( 0, bestX - maxRadius );
   int right  = std::min( w - 1, bestX + maxRadius );
   int top    = std::max( 0, bestY - maxRadius );
   int bottom = std::min( h - 1, bestY + maxRadius );

   int newW = right - left + 1;
   int newH = bottom - top + 1;

   if ( newW >= w - 2 && newH >= h - 2 )
   {
      std::cout << "  Crop would remove < 2 pixels per side - no crop needed\n";
      cropX0 = 0;
      cropY0 = 0;
      return;
   }

   double retained = double( newW ) * newH / ( double( w ) * h ) * 100;
   std::cout << "  Crop: [" << left << "," << top << "] to [" << right << "," << bottom << "]"
             << " (" << w << "x" << h << " → " << newW << "x" << newH
             << ", " << std::setprecision( 1 ) << std::fixed << retained << "% retained)\n"
             << std::defaultfloat;

   // Create cropped image
   Image cropped( newW, newH, image.ColorSpace() );
   cropped.AllocateData( newW, newH, image.NumberOfChannels(), image.ColorSpace() );
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
      for ( int y = 0; y < newH; ++y )
         for ( int x = 0; x < newW; ++x )
            cropped.Pixel( x, y, c ) = image.Pixel( x + left, y + top, c );

   image = std::move( cropped );
   cropX0 = left;
   cropY0 = top;
}

// ============================================================================
// Background Neutralization
// ============================================================================

void BackgroundNeutralize( Image& image )
{
   int w = image.Width();
   int h = image.Height();
   int ch = image.NumberOfChannels();

   if ( ch < 3 )
   {
      std::cout << "  Skipping background neutralization (image has < 3 channels)\n";
      return;
   }

   std::cout << "  Background Neutralization:\n";

   // Step 1: Compute luminance for each pixel, excluding zero/near-zero pixels (black borders)
   int totalPixels = w * h;
   const double minLuminance = 1e-6; // Threshold to exclude black border pixels
   std::vector<double> luminance( totalPixels );
   std::vector<int> validPixels; // Indices of non-border pixels
   validPixels.reserve( totalPixels );
   for ( int p = 0; p < totalPixels; ++p )
   {
      double sum = 0;
      for ( int c = 0; c < ch && c < 3; ++c )
         sum += image.Pixel( p % w, p / w, c );
      luminance[p] = sum / std::min( ch, 3 );
      if ( luminance[p] > minLuminance )
         validPixels.push_back( p );
   }

   std::cout << "    Valid (non-border) pixels: " << validPixels.size()
             << " / " << totalPixels << "\n";

   if ( validPixels.size() < 100 )
   {
      std::cout << "    Too few valid pixels - skipping background neutralization\n";
      return;
   }

   // Step 2: Find threshold for bottom 5% of valid pixels by luminance
   std::vector<double> validLum( validPixels.size() );
   for ( size_t i = 0; i < validPixels.size(); ++i )
      validLum[i] = luminance[validPixels[i]];
   size_t threshold_idx = static_cast<size_t>( validPixels.size() * 0.05 );
   if ( threshold_idx < 1 )
      threshold_idx = 1;
   std::nth_element( validLum.begin(), validLum.begin() + threshold_idx, validLum.end() );
   double lumThreshold = validLum[threshold_idx];

   std::cout << "    Luminance threshold (5th percentile of valid): " << std::setprecision( 6 ) << lumThreshold << "\n";

   // Step 3: Collect background pixels per channel (from valid pixels only)
   std::vector<std::vector<double>> bgPixels( 3 );
   for ( int idx : validPixels )
   {
      if ( luminance[idx] <= lumThreshold )
      {
         int x = idx % w;
         int y = idx / w;
         for ( int c = 0; c < 3; ++c )
            bgPixels[c].push_back( image.Pixel( x, y, c ) );
      }
   }

   std::cout << "    Background sample pixels: " << bgPixels[0].size() << "\n";

   // Step 4: Compute per-channel median of background
   double bgMedian[3];
   double bgMAD[3];
   for ( int c = 0; c < 3; ++c )
   {
      bgMedian[c] = Median( bgPixels[c] );
      bgMAD[c] = MAD( bgPixels[c], bgMedian[c] );
   }

   const char* chanNames[3] = { "R", "G", "B" };
   for ( int c = 0; c < 3; ++c )
      std::cout << "    Channel " << chanNames[c] << " background: median=" << std::setprecision( 6 )
                << bgMedian[c] << "  MAD=" << bgMAD[c] << "\n";

   // Step 5: Compute per-channel offsets to equalize backgrounds
   // PixInsight's approach: subtract each channel's background, then add a common target
   // This preserves relative intensities (no multiplicative scaling)
   double refMedian = bgMedian[1]; // Use green channel as reference (like PixInsight)
   double offset[3];
   for ( int c = 0; c < 3; ++c )
      offset[c] = refMedian - bgMedian[c];

   std::cout << "    Reference (green) bg: " << std::setprecision( 6 ) << refMedian << "\n";
   std::cout << "    Offsets: R=" << std::setprecision( 6 ) << offset[0]
             << "  G=" << offset[1] << "  B=" << offset[2] << "\n";

   // Step 6: Apply additive correction to entire image
   for ( int c = 0; c < 3; ++c )
   {
      if ( std::abs( offset[c] ) < 1e-12 )
         continue; // Green channel, no change needed
      for ( int y = 0; y < h; ++y )
         for ( int x = 0; x < w; ++x )
         {
            double v = image.Pixel( x, y, c ) + offset[c];
            image.Pixel( x, y, c ) = std::max( 0.0, std::min( 1.0, v ) );
         }
   }

   std::cout << "    Background neutralization applied.\n";
}

// ============================================================================
// Spectrophotometric Color Calibration (SPCC)
// ============================================================================

void SpectrophotometricColorCalibrate(
   Image& image,
   const FITSKeywordArray& keywords,
   const PropertyArray& properties,
   const std::vector<std::string>& gaiaDbPaths )
{
   int w = image.Width();
   int h = image.Height();
   int ch = image.NumberOfChannels();

   if ( ch < 3 )
   {
      std::cout << "  Skipping SPCC (image has < 3 channels)\n";
      return;
   }

   std::cout << "  Spectrophotometric Color Calibration:\n";

   // -----------------------------------------------------------------
   // Step 1: Build astrometric metadata
   // Try PCL AstrometricMetadata first, fall back to SimpleTANWCS
   // -----------------------------------------------------------------
   AstrometricMetadata amd;
   SimpleTANWCS tanWcs;
   bool useSimpleTAN = false;

   try
   {
      amd.Build( properties, keywords, w, h );
   }
   catch ( const Exception& e )
   {
      std::cout << "    PCL WCS build failed: " << e.Message().ToUTF8() << "\n";
      std::cout << "    Falling back to SimpleTANWCS from FITS keywords...\n";
   }

   if ( g_solvedWCS )
   {
      // Already plate-solved in Step 1 - reuse existing WCS
      std::cout << "    Using pre-existing plate-solved WCS\n";
   }
   else if ( amd.IsValid() )
   {
      std::cout << "    WCS plate solution: valid (resolution "
                << std::setprecision( 4 ) << amd.Resolution() * 3600 << " arcsec/px)\n";
   }
   else if ( BuildWCSFromMetadata( tanWcs, properties, keywords ) )
   {
      useSimpleTAN = true;
      std::cout << "    SimpleTAN WCS: valid (resolution "
                << std::setprecision( 4 ) << tanWcs.Resolution() * 3600 << " arcsec/px)\n";
      std::cout << "    CRVAL=(" << tanWcs.crval1 << ", " << tanWcs.crval2 << ")"
                << " CRPIX=(" << tanWcs.crpix1 << ", " << tanWcs.crpix2 << ")\n";
   }
   else if ( PlateSolve( image, tanWcs, keywords ) )
   {
      useSimpleTAN = true;
      std::cout << "    Plate-solved WCS: valid (resolution "
                << std::setprecision( 4 ) << tanWcs.Resolution() * 3600 << " arcsec/px)\n";
   }
   else
   {
      std::cerr << "  ERROR: No valid WCS plate solution - cannot perform SPCC\n";
      return;
   }

   // Unified coordinate transform: image pixel -> (ra, dec) in degrees
   // Priority: StellarSolver WCS > SimpleTAN from metadata > PCL AstrometricMetadata
   auto imageToCelestial = [&]( double px, double py, double& ra, double& dec ) -> bool
   {
      if ( g_solvedWCS )
      {
         // Use StellarSolver's native WCS (most accurate for plate-solved images)
         // StellarSolver uses 1-indexed FITS pixel coordinates
         // Image was 2x downsampled before solving, so divide coords by 2
         // Apply crop offset to convert cropped-image coords back to original-image coords
         double origPx = px + g_cropOffsetX;
         double origPy = py + g_cropOffsetY;
         QPointF pixel( origPx / 2.0 + 1.0, origPy / 2.0 + 1.0 );
         FITSImage::wcs_point sky;
         if ( g_solvedWCS->pixelToWCS( pixel, sky ) )
         {
            ra = sky.ra;
            dec = sky.dec;
            return true;
         }
         return false;
      }
      if ( useSimpleTAN )
         return tanWcs.PixelToWorld( px + 1.0, py + 1.0, ra, dec );
      DPoint rd;
      if ( amd.ImageToCelestial( rd, DPoint( px, py ) ) )
      {
         ra = rd.x;
         dec = rd.y;
         return true;
      }
      return false;
   };

   // -----------------------------------------------------------------
   // Step 2: Detect stars in the green channel (or luminance)
   // -----------------------------------------------------------------
   // Create a luminance image for star detection
   Image lumiImage( w, h );
   for ( int y = 0; y < h; ++y )
      for ( int x = 0; x < w; ++x )
      {
         double lum = 0;
         for ( int c = 0; c < std::min( ch, 3 ); ++c )
            lum += image.Pixel( x, y, c );
         lumiImage.Pixel( x, y ) = lum / std::min( ch, 3 );
      }

   ImageVariant lumiVariant( &lumiImage );
   lumiVariant.SetOwnership( false );

   StarDetector detector;
   detector.SetStructureLayers( 5 );
   detector.SetSensitivity( 0.5F );
   detector.SetPeakResponse( 0.5F );
   detector.SetUpperLimit( 0.9F ); // reject saturated stars

   StarDetector::star_list stars = detector.DetectStars( lumiVariant );
   std::cout << "    Detected " << stars.Length() << " stars\n";

   if ( stars.Length() < 10 )
   {
      std::cerr << "  WARNING: Too few stars detected for reliable SPCC\n";
      if ( stars.IsEmpty() )
         return;
   }

   // Limit to brightest 2000 stars for efficiency
   int maxStars = std::min( int( stars.Length() ), 2000 );

   // -----------------------------------------------------------------
   // Step 3: Map star positions to sky coordinates
   // -----------------------------------------------------------------
   struct StarWithCoords
   {
      double ra, dec;     // sky coords in degrees
      double imgX, imgY;  // image coords
      float flux;         // detected flux
   };
   std::vector<StarWithCoords> starCoords;
   starCoords.reserve( maxStars );

   for ( int i = 0; i < maxStars; ++i )
   {
      double ra, dec;
      if ( imageToCelestial( stars[i].pos.x, stars[i].pos.y, ra, dec ) )
      {
         StarWithCoords sc;
         sc.ra = ra;
         sc.dec = dec;
         sc.imgX = stars[i].pos.x;
         sc.imgY = stars[i].pos.y;
         sc.flux = stars[i].flux;
         starCoords.push_back( sc );
      }
   }
   std::cout << "    Stars with valid sky coords: " << starCoords.size() << "\n";

   // Print first few star coords for diagnostics
   for ( size_t i = 0; i < std::min( size_t( 3 ), starCoords.size() ); ++i )
      std::cout << "    Star " << i << ": px=(" << std::setprecision( 1 )
                << starCoords[i].imgX << "," << starCoords[i].imgY
                << ") → RA=" << std::setprecision( 6 ) << starCoords[i].ra
                << " Dec=" << starCoords[i].dec << "\n";

   // -----------------------------------------------------------------
   // Step 4: Open Gaia DR3 SP databases and search for matching stars
   // -----------------------------------------------------------------
   if ( gaiaDbPaths.empty() )
   {
      std::cerr << "  ERROR: No Gaia database files specified - cannot perform SPCC\n";
      return;
   }

   // Open all database files
   std::vector<std::unique_ptr<GaiaDatabaseFile>> gaiaDBs;
   float spectrumStart = 0, spectrumStep = 0;
   int spectrumCount = 0;
   bool hasSpectrumData = false;

   for ( const auto& path : gaiaDbPaths )
   {
      try
      {
         auto db = std::make_unique<GaiaDatabaseFile>( String::UTF8ToUTF16( path.c_str() ) );
         if ( db->HasMeanSpectrumData() )
         {
            if ( !hasSpectrumData )
            {
               spectrumStart = db->SpectrumStart();
               spectrumStep = db->SpectrumStep();
               spectrumCount = db->SpectrumCount();
               hasSpectrumData = true;
            }
            std::cout << "    Opened Gaia DB: " << path
                      << " (DR" << db->DataRelease().c_str() << "/SP"
                      << ", spectrum " << spectrumStart << "-"
                      << ( spectrumStart + ( spectrumCount - 1 ) * spectrumStep ) << " nm"
                      << ", " << spectrumCount << " samples)\n";
         }
         else
         {
            std::cout << "    Opened Gaia DB: " << path
                      << " (DR" << db->DataRelease().c_str() << ", no spectrum data)\n";
         }
         gaiaDBs.push_back( std::move( db ) );
      }
      catch ( const Exception& e )
      {
         std::cerr << "  WARNING: Failed to open Gaia DB " << path << ": " << e.Message().ToUTF8() << "\n";
      }
   }

   if ( !hasSpectrumData )
   {
      std::cerr << "  ERROR: No Gaia database with BP/RP spectrum data found\n";
      return;
   }

   // Compute field center and radius from image corners
   double centerRA, centerDec;
   imageToCelestial( w / 2.0, h / 2.0, centerRA, centerDec );

   double cornerRA, cornerDec;
   imageToCelestial( 0, 0, cornerRA, cornerDec );
   // Angular distance from center to corner
   double dra = ( cornerRA - centerRA ) * std::cos( Rad( centerDec ) );
   double ddec = cornerDec - centerDec;
   double fieldRadius = std::sqrt( dra * dra + ddec * ddec ) * 1.1; // 10% margin

   std::cout << "    Field center: RA=" << std::setprecision( 6 ) << centerRA
             << " Dec=" << centerDec << " radius=" << fieldRadius << " deg\n";

   // Search all databases
   std::vector<GaiaStarData> gaiaStars;
   for ( auto& db : gaiaDBs )
   {
      GaiaSearchData search;
      search.centerRA = centerRA;
      search.centerDec = centerDec;
      search.radius = fieldRadius;
      search.magnitudeLow = -1.5;
      search.magnitudeHigh = 18.0;
      search.sourceLimit = 100000;
      search.normalizeSpectrum = false;
      search.photonFluxUnits = false; // use energy flux (W/nm/m²) from Gaia

      db->Search( search );

      for ( const auto& s : search.stars )
         if ( s.flux.Length() > 0 )
            gaiaStars.push_back( s );
   }

   std::cout << "    Gaia stars with spectra found: " << gaiaStars.size() << "\n";

   // Print first few Gaia star coords for diagnostics
   for ( size_t i = 0; i < std::min( size_t( 3 ), gaiaStars.size() ); ++i )
      std::cout << "    Gaia " << i << ": RA=" << std::setprecision( 6 ) << gaiaStars[i].ra
                << " Dec=" << gaiaStars[i].dec << "\n";

   if ( gaiaStars.size() < 5 )
   {
      std::cerr << "  WARNING: Too few Gaia stars with spectra for reliable calibration\n";
      if ( gaiaStars.empty() )
         return;
   }

   // -----------------------------------------------------------------
   // Step 5: Cross-match detected stars with Gaia stars
   // -----------------------------------------------------------------
   // Use adaptive match radius: at least 3 arcsec, or 2 pixels worth
   double pixscaleArcsec = useSimpleTAN ? tanWcs.Resolution() * 3600.0
                                        : amd.Resolution() * 3600.0;
   double matchRadiusArcsec = std::max( 3.0, pixscaleArcsec * 2.0 );
   double matchRadiusDeg = matchRadiusArcsec / 3600.0;
   std::cout << "    Match radius: " << std::setprecision( 2 ) << matchRadiusArcsec << " arcsec ("
             << std::setprecision( 4 ) << matchRadiusDeg << " deg)\n";

   // Diagnostic: find nearest Gaia star to first few detected stars
   for ( size_t i = 0; i < std::min( size_t( 3 ), starCoords.size() ); ++i )
   {
      double bestDist = 1e30;
      for ( const auto& gaia : gaiaStars )
      {
         double dra2 = ( gaia.ra - starCoords[i].ra ) * std::cos( Rad( starCoords[i].dec ) );
         double ddec2 = gaia.dec - starCoords[i].dec;
         double dist = std::sqrt( dra2 * dra2 + ddec2 * ddec2 );
         if ( dist < bestDist ) bestDist = dist;
      }
      std::cout << "    Nearest Gaia to star " << i << ": " << std::setprecision( 4 )
                << bestDist * 3600 << " arcsec\n";
   }

   struct MatchedStar
   {
      double imgX, imgY;
      double measuredR, measuredG, measuredB;
      double predictedR, predictedG, predictedB;
   };
   std::vector<MatchedStar> matches;

   int aperture = 5; // pixel radius for photometry aperture

   for ( const auto& imgStar : starCoords )
   {
      // Find closest Gaia star
      double bestDist = 1e30;
      const GaiaStarData* bestGaia = nullptr;

      for ( const auto& gaia : gaiaStars )
      {
         double dra2 = ( gaia.ra - imgStar.ra ) * std::cos( Rad( imgStar.dec ) );
         double ddec2 = gaia.dec - imgStar.dec;
         double dist = std::sqrt( dra2 * dra2 + ddec2 * ddec2 );
         if ( dist < bestDist )
         {
            bestDist = dist;
            bestGaia = &gaia;
         }
      }

      if ( bestGaia == nullptr || bestDist > matchRadiusDeg )
         continue;
      if ( bestGaia->flux.Length() < spectrumCount )
         continue;

      // Convolve Gaia spectrum with filter curves to get predicted R/G/B
      double predR = ConvolveSpectrumWithFilter( bestGaia->flux, spectrumStart, spectrumStep, spectrumCount,
                                                  s_filterR, s_filterRCount );
      double predG = ConvolveSpectrumWithFilter( bestGaia->flux, spectrumStart, spectrumStep, spectrumCount,
                                                  s_filterG, s_filterGCount );
      double predB = ConvolveSpectrumWithFilter( bestGaia->flux, spectrumStart, spectrumStep, spectrumCount,
                                                  s_filterB, s_filterBCount );

      if ( predR <= 0 || predG <= 0 || predB <= 0 )
         continue;

      // Measure actual R/G/B fluxes via background-subtracted aperture photometry
      int cx = int( imgStar.imgX + 0.5 );
      int cy = int( imgStar.imgY + 0.5 );
      int annulusInner = aperture + 2; // background annulus starts 2px outside aperture
      int annulusOuter = aperture + 6;

      // Star aperture flux
      double sumR = 0, sumG = 0, sumB = 0;
      int count = 0;
      // Background annulus flux
      double bgSumR = 0, bgSumG = 0, bgSumB = 0;
      int bgCount = 0;

      for ( int dy = -annulusOuter; dy <= annulusOuter; ++dy )
         for ( int dx = -annulusOuter; dx <= annulusOuter; ++dx )
         {
            int r2 = dx * dx + dy * dy;
            int px = cx + dx;
            int py = cy + dy;
            if ( px < 0 || px >= w || py < 0 || py >= h )
               continue;
            double vR = image.Pixel( px, py, 0 );
            double vG = image.Pixel( px, py, 1 );
            double vB = image.Pixel( px, py, 2 );

            if ( r2 <= aperture * aperture )
            {
               sumR += vR; sumG += vG; sumB += vB;
               ++count;
            }
            else if ( r2 >= annulusInner * annulusInner && r2 <= annulusOuter * annulusOuter )
            {
               bgSumR += vR; bgSumG += vG; bgSumB += vB;
               ++bgCount;
            }
         }

      if ( count == 0 || bgCount < 10 )
         continue;

      // Subtract local background
      double bgR = bgSumR / bgCount;
      double bgG = bgSumG / bgCount;
      double bgB = bgSumB / bgCount;
      double measR = sumR / count - bgR;
      double measG = sumG / count - bgG;
      double measB = sumB / count - bgB;

      // Reject very faint (after bg subtraction) or saturated stars
      double maxMeas = std::max( { measR, measG, measB } );
      double minMeas = std::min( { measR, measG, measB } );
      if ( minMeas < 0.0005 || maxMeas > 0.85 )
         continue;

      MatchedStar m;
      m.imgX = imgStar.imgX;
      m.imgY = imgStar.imgY;
      m.measuredR = measR;
      m.measuredG = measG;
      m.measuredB = measB;
      m.predictedR = predR;
      m.predictedG = predG;
      m.predictedB = predB;
      matches.push_back( m );
   }

   std::cout << "    Cross-matched stars: " << matches.size() << "\n";

   if ( matches.size() < 3 )
   {
      std::cerr << "  WARNING: Too few matched stars for reliable SPCC (" << matches.size() << ")\n";
      if ( matches.empty() )
         return;
   }

   // -----------------------------------------------------------------
   // Step 6: Compute per-channel correction factors using median ratios
   // -----------------------------------------------------------------
   // For each star, compute ratio = (predicted_c / predicted_G) / (measured_c / measured_G)
   // This normalizes relative to Green channel
   std::vector<double> ratioR, ratioB;
   ratioR.reserve( matches.size() );
   ratioB.reserve( matches.size() );

   for ( const auto& m : matches )
   {
      // Predicted color ratios (relative to green)
      double predRatioR = m.predictedR / m.predictedG;
      double predRatioB = m.predictedB / m.predictedG;

      // Measured color ratios (relative to green)
      double measRatioR = m.measuredR / m.measuredG;
      double measRatioB = m.measuredB / m.measuredG;

      if ( measRatioR > 0 && measRatioB > 0 )
      {
         ratioR.push_back( predRatioR / measRatioR );
         ratioB.push_back( predRatioB / measRatioB );
      }
   }

   // Diagnostics: show median predicted and measured ratios separately
   {
      std::vector<double> predRG, predBG, measRG, measBG;
      for ( const auto& m : matches )
      {
         predRG.push_back( m.predictedR / m.predictedG );
         predBG.push_back( m.predictedB / m.predictedG );
         measRG.push_back( m.measuredR / m.measuredG );
         measBG.push_back( m.measuredB / m.measuredG );
      }
      std::cout << "    Median predicted R/G=" << std::setprecision(4) << Median( predRG )
                << "  B/G=" << Median( predBG ) << "\n";
      std::cout << "    Median measured  R/G=" << Median( measRG )
                << "  B/G=" << Median( measBG ) << "\n";
   }

   // Use median of ratios as robust correction factor
   double corrR = Median( ratioR );
   double corrG = 1.0; // Green is reference
   double corrB = Median( ratioB );

   // Normalize so that corrections average to 1 (preserve overall brightness)
   double corrMean = ( corrR + corrG + corrB ) / 3.0;
   if ( corrMean > 0 )
   {
      corrR /= corrMean;
      corrG /= corrMean;
      corrB /= corrMean;
   }

   std::cout << "    SPCC correction factors: R=" << std::setprecision( 6 ) << corrR
             << "  G=" << corrG << "  B=" << corrB << "\n";

   // Report MAD of the ratios for quality assessment
   if ( ratioR.size() >= 3 )
   {
      double convergenceR = MAD( ratioR, Median( ratioR ) );
      double convergenceB = MAD( ratioB, Median( ratioB ) );
      std::cout << "    Convergence (MAD): R=" << std::setprecision( 4 ) << convergenceR
                << "  B=" << convergenceB << "\n";
   }

   // -----------------------------------------------------------------
   // Step 7: Apply correction to entire image
   // -----------------------------------------------------------------
   double corr[3] = { corrR, corrG, corrB };
   for ( int c = 0; c < 3; ++c )
   {
      for ( int y = 0; y < h; ++y )
         for ( int x = 0; x < w; ++x )
         {
            double v = image.Pixel( x, y, c ) * corr[c];
            image.Pixel( x, y, c ) = std::max( 0.0, std::min( 1.0, v ) );
         }
   }

   std::cout << "    SPCC correction applied.\n";
}

// ============================================================================
// SDSS Flux Calibration
// Queries SDSS SkyServer for calibrated photometry and uses it as a reference
// ============================================================================

// Fetch SDSS star photometry via SkyServer REST API + curl
std::vector<SDSSstar> FetchSDSSPhotometry( double centerRA, double centerDec, double radiusDeg )
{
   std::vector<SDSSstar> result;

   // Build SQL query for SDSS DR18
   // Use RA/Dec box (slightly wider than cone to be safe)
   double raMargin = radiusDeg / std::cos( centerDec * M_PI / 180.0 );
   double ra1 = centerRA - raMargin;
   double ra2 = centerRA + raMargin;
   double dec1 = centerDec - radiusDeg;
   double dec2 = centerDec + radiusDeg;

   char sql[1024];
   snprintf( sql, sizeof( sql ),
      "SELECT TOP 2000 "
      "p.ra,p.dec,p.psfMag_u,p.psfMag_g,p.psfMag_r,p.psfMag_i,"
      "p.psfMagErr_g,p.psfMagErr_r,p.psfMagErr_i "
      "FROM PhotoObj AS p "
      "WHERE p.type=6 "
      "AND p.ra BETWEEN %.4f AND %.4f "
      "AND p.dec BETWEEN %.4f AND %.4f "
      "AND p.psfMagErr_g<0.05 "
      "AND p.psfMagErr_r<0.05 "
      "AND p.psfMagErr_i<0.05 "
      "AND p.psfMag_r BETWEEN 10 AND 20 "
      "AND p.clean=1",
      ra1, ra2, dec1, dec2 );

   // URL-encode the SQL (simple: replace spaces with +)
   std::string encodedSQL;
   for ( const char* p = sql; *p; ++p )
   {
      if ( *p == ' ' )
         encodedSQL += '+';
      else if ( *p == '<' )
         encodedSQL += "%3C";
      else
         encodedSQL += *p;
   }

   std::string url = "https://skyserver.sdss.org/dr18/SkyServerWS/SearchTools/SqlSearch?cmd="
                     + encodedSQL + "&format=csv";

   // Fetch via curl
   std::string curlCmd = "curl -s --max-time 30 '" + url + "' 2>/dev/null";
   FILE* pipe = popen( curlCmd.c_str(), "r" );
   if ( !pipe )
   {
      std::cerr << "    Failed to execute curl\n";
      return result;
   }

   std::string response;
   char buf[4096];
   while ( fgets( buf, sizeof( buf ), pipe ) )
      response += buf;
   int exitCode = pclose( pipe );

   if ( exitCode != 0 || response.empty() )
   {
      std::cerr << "    SDSS query failed (exit code " << exitCode << ")\n";
      return result;
   }

   // Check for error in response
   if ( response.find( "error" ) != std::string::npos && response.find( "ra" ) == std::string::npos )
   {
      std::cerr << "    SDSS query returned error: " << response.substr( 0, 200 ) << "\n";
      return result;
   }

   // Parse CSV: skip header line, then parse data lines
   std::istringstream iss( response );
   std::string line;
   bool headerSeen = false;
   while ( std::getline( iss, line ) )
   {
      if ( line.empty() || line[0] == '#' )
         continue;
      if ( !headerSeen )
      {
         if ( line.find( "ra" ) != std::string::npos )
            headerSeen = true;
         continue;
      }

      // Parse: ra,dec,psfMag_u,psfMag_g,psfMag_r,psfMag_i,...
      SDSSstar star;
      if ( sscanf( line.c_str(), "%lf,%lf,%lf,%lf,%lf,%lf",
                   &star.ra, &star.dec, &star.psfMag_u, &star.psfMag_g,
                   &star.psfMag_r, &star.psfMag_i ) == 6 )
      {
         // Reject bad magnitudes
         if ( star.psfMag_g > 0 && star.psfMag_g < 25 &&
              star.psfMag_r > 0 && star.psfMag_r < 25 &&
              star.psfMag_i > 0 && star.psfMag_i < 25 &&
              star.psfMag_u > 0 && star.psfMag_u < 25 )
            result.push_back( star );
      }
   }

   return result;
}

// No filter-curve prediction needed for empirical SDSS calibration

void SDSSColorCalibrate(
   Image& image,
   const FITSKeywordArray& keywords )
{
   int w = image.Width();
   int h = image.Height();
   int ch = image.NumberOfChannels();

   if ( ch < 3 )
   {
      std::cout << "  Skipping SDSS calibration (image has < 3 channels)\n";
      return;
   }

   std::cout << "  SDSS Flux Calibration:\n";

   // Need WCS to convert pixel → sky coords
   if ( !g_solvedWCS )
   {
      std::cerr << "    ERROR: No plate-solved WCS available for SDSS calibration\n";
      return;
   }

   // Compute field center and radius from image corners
   auto pixToSky = [&]( double px, double py, double& ra, double& dec ) -> bool
   {
      double origPx = px + g_cropOffsetX;
      double origPy = py + g_cropOffsetY;
      QPointF pixel( origPx / 2.0 + 1.0, origPy / 2.0 + 1.0 );
      FITSImage::wcs_point sky;
      if ( g_solvedWCS->pixelToWCS( pixel, sky ) )
      {
         ra = sky.ra;
         dec = sky.dec;
         return true;
      }
      return false;
   };

   double centerRA, centerDec;
   if ( !pixToSky( w / 2.0, h / 2.0, centerRA, centerDec ) )
   {
      std::cerr << "    ERROR: Cannot determine field center\n";
      return;
   }

   double cornerRA, cornerDec;
   pixToSky( 0, 0, cornerRA, cornerDec );
   double dra = ( cornerRA - centerRA ) * std::cos( centerDec * M_PI / 180.0 );
   double ddec = cornerDec - centerDec;
   double fieldRadius = std::sqrt( dra * dra + ddec * ddec );

   std::cout << "    Field: RA=" << std::setprecision( 5 ) << centerRA
             << " Dec=" << centerDec << " radius=" << std::setprecision( 3 )
             << fieldRadius << " deg\n";

   // Fetch SDSS photometry
   std::cout << "    Querying SDSS DR18 SkyServer...\n";
   std::vector<SDSSstar> sdssStars = FetchSDSSPhotometry( centerRA, centerDec, fieldRadius * 1.1 );
   std::cout << "    SDSS stars with good photometry: " << sdssStars.size() << "\n";

   if ( sdssStars.size() < 5 )
   {
      std::cerr << "    ERROR: Too few SDSS stars for calibration (need >= 5, got "
                << sdssStars.size() << ")\n";
      std::cerr << "    (This field may not be in SDSS footprint)\n";
      return;
   }

   // Detect stars in the image
   Image lumiImage( w, h );
   for ( int y = 0; y < h; ++y )
      for ( int x = 0; x < w; ++x )
      {
         double lum = 0;
         for ( int c = 0; c < std::min( ch, 3 ); ++c )
            lum += image.Pixel( x, y, c );
         lumiImage.Pixel( x, y ) = lum / std::min( ch, 3 );
      }
   ImageVariant lumiVariant( &lumiImage );
   lumiVariant.SetOwnership( false );

   StarDetector detector;
   detector.SetStructureLayers( 5 );
   detector.SetSensitivity( 0.5F );
   detector.SetPeakResponse( 0.5F );
   detector.SetUpperLimit( 0.9F );
   StarDetector::star_list stars = detector.DetectStars( lumiVariant );
   std::cout << "    Detected " << stars.Length() << " stars in image\n";

   // Map star positions to sky coordinates
   struct ImgStar { double ra, dec, imgX, imgY; };
   std::vector<ImgStar> imgStars;
   int maxStars = std::min( int( stars.Length() ), 2000 );
   for ( int i = 0; i < maxStars; ++i )
   {
      double ra, dec;
      if ( pixToSky( stars[i].pos.x, stars[i].pos.y, ra, dec ) )
         imgStars.push_back( { ra, dec, stars[i].pos.x, stars[i].pos.y } );
   }
   std::cout << "    Stars with sky coords: " << imgStars.size() << "\n";

   // Cross-match image stars with SDSS stars
   // Empirical approach: for each matched star, we measure camera R, G, B fluxes
   // and compare against SDSS calibrated fluxes F_r, F_g, F_i.
   // We solve for the per-channel scale factor that best maps SDSS flux to camera flux.
   double matchRadiusDeg = 3.0 / 3600.0; // 3 arcsec
   int aperture = 5;

   struct MatchedStar
   {
      double measR, measG, measB;    // Camera aperture photometry
      double sdssR, sdssG, sdssI;    // SDSS flux (arbitrary units)
      double sdss_gr;                // SDSS g-r color
   };
   std::vector<MatchedStar> matches;

   for ( const auto& imgStar : imgStars )
   {
      // Find closest SDSS star
      double bestDist = 1e30;
      const SDSSstar* bestSDSS = nullptr;
      for ( const auto& sdss : sdssStars )
      {
         double dra2 = ( sdss.ra - imgStar.ra ) * std::cos( imgStar.dec * M_PI / 180.0 );
         double ddec2 = sdss.dec - imgStar.dec;
         double dist = std::sqrt( dra2 * dra2 + ddec2 * ddec2 );
         if ( dist < bestDist )
         {
            bestDist = dist;
            bestSDSS = &sdss;
         }
      }

      if ( !bestSDSS || bestDist > matchRadiusDeg )
         continue;

      // SDSS fluxes (10^(-0.4*mag))
      double sdssR = std::pow( 10.0, -0.4 * bestSDSS->psfMag_r );
      double sdssG = std::pow( 10.0, -0.4 * bestSDSS->psfMag_g );
      double sdssI = std::pow( 10.0, -0.4 * bestSDSS->psfMag_i );

      // Measure actual RGB from image (background-subtracted aperture photometry)
      int cx = int( imgStar.imgX + 0.5 );
      int cy = int( imgStar.imgY + 0.5 );
      int annulusInner = aperture + 2;
      int annulusOuter = aperture + 6;
      double sumR = 0, sumG = 0, sumB = 0;
      int count = 0;
      double bgSumR = 0, bgSumG = 0, bgSumB = 0;
      int bgCount = 0;

      for ( int dy = -annulusOuter; dy <= annulusOuter; ++dy )
         for ( int dx = -annulusOuter; dx <= annulusOuter; ++dx )
         {
            int r2 = dx * dx + dy * dy;
            int px = cx + dx, py = cy + dy;
            if ( px < 0 || px >= w || py < 0 || py >= h )
               continue;
            double vR = image.Pixel( px, py, 0 );
            double vG = image.Pixel( px, py, 1 );
            double vB = image.Pixel( px, py, 2 );
            if ( r2 <= aperture * aperture )
            { sumR += vR; sumG += vG; sumB += vB; ++count; }
            else if ( r2 >= annulusInner * annulusInner && r2 <= annulusOuter * annulusOuter )
            { bgSumR += vR; bgSumG += vG; bgSumB += vB; ++bgCount; }
         }
      if ( count == 0 || bgCount < 10 ) continue;

      double measR = sumR / count - bgSumR / bgCount;
      double measG = sumG / count - bgSumG / bgCount;
      double measB = sumB / count - bgSumB / bgCount;

      double maxMeas = std::max( { measR, measG, measB } );
      double minMeas = std::min( { measR, measG, measB } );
      if ( minMeas < 0.0005 || maxMeas > 0.85 )
         continue;

      matches.push_back( { measR, measG, measB, sdssR, sdssG, sdssI,
                           bestSDSS->psfMag_g - bestSDSS->psfMag_r } );
   }

   std::cout << "    Cross-matched stars: " << matches.size() << "\n";

   if ( matches.size() < 3 )
   {
      std::cerr << "    WARNING: Too few matched stars (" << matches.size() << ")\n";
      if ( matches.empty() ) return;
   }

   // Empirical color correction using SDSS reference:
   // For each matched star, solve for the scale factor per channel such that
   // cam_c * k_c = sdss_flux_c (mapping camera to calibrated SDSS fluxes).
   //
   // The per-star scale: k_c = sdss_flux_c / cam_c
   // Color correction = k_R/k_G, k_B/k_G (relative to green channel)
   //
   // SDSS band → camera channel mapping:
   //   SDSS r (628nm) → Camera R (peak 630nm) — excellent match
   //   SDSS g (476nm) → Camera G (peak 520nm) — moderate match
   //   SDSS g (476nm) → Camera B (peak 440nm) — approximate
   //
   // Since camera G is redder than SDSS g, we interpolate: for camera G,
   // use sdss_flux at ~520nm estimated from g and r.

   std::vector<double> scaleR_over_G, scaleB_over_G;
   std::vector<double> allColors;
   for ( const auto& m : matches )
   {
      // Estimate SDSS flux at camera wavelengths via log-linear interp
      // SDSS g at 476nm, r at 628nm
      double logFg = std::log( m.sdssG );
      double logFr = std::log( m.sdssR );

      // Camera R peak 630nm ≈ SDSS r (628nm)
      double sdss_camR = m.sdssR;
      // Camera G peak 520nm: interpolate between g(476) and r(628)
      double t_G = ( 520.0 - 476.0 ) / ( 628.0 - 476.0 ); // = 0.29
      double sdss_camG = std::exp( logFg + t_G * ( logFr - logFg ) );
      // Camera B peak 440nm: extrapolate blueward from g
      double slope = ( logFr - logFg ) / ( 628.0 - 476.0 );
      double sdss_camB = std::exp( logFg + slope * ( 440.0 - 476.0 ) );

      // Per-channel scale: k_c = sdss_cam_c / measured_c
      double kR = sdss_camR / m.measR;
      double kG = sdss_camG / m.measG;
      double kB = sdss_camB / m.measB;

      // Color correction relative to green
      if ( kG > 0 )
      {
         scaleR_over_G.push_back( kR / kG );
         scaleB_over_G.push_back( kB / kG );
      }
      allColors.push_back( m.sdss_gr );
   }

   double corrR = Median( scaleR_over_G );
   double corrG = 1.0;
   double corrB = Median( scaleB_over_G );

   // Normalize to preserve overall brightness
   double corrMean = ( corrR + corrG + corrB ) / 3.0;
   if ( corrMean > 0 )
   {
      corrR /= corrMean;
      corrG /= corrMean;
      corrB /= corrMean;
   }

   std::cout << "    SDSS correction factors: R=" << std::setprecision( 6 ) << corrR
             << "  G=" << corrG << "  B=" << corrB << "\n";

   if ( scaleR_over_G.size() >= 3 )
   {
      double convergenceR = MAD( scaleR_over_G, Median( scaleR_over_G ) );
      double convergenceB = MAD( scaleB_over_G, Median( scaleB_over_G ) );
      std::cout << "    Convergence (MAD): R=" << std::setprecision( 4 ) << convergenceR
                << "  B=" << convergenceB << "\n";
   }

   // Report SDSS color distribution
   if ( !allColors.empty() )
   {
      std::sort( allColors.begin(), allColors.end() );
      std::cout << "    SDSS g-r color range: " << std::setprecision( 3 )
                << allColors.front() << " to " << allColors.back()
                << " (median " << allColors[allColors.size() / 2] << ")\n";
   }

   // Apply correction
   double corr[3] = { corrR, corrG, corrB };
   for ( int c = 0; c < 3; ++c )
      for ( int y = 0; y < h; ++y )
         for ( int x = 0; x < w; ++x )
         {
            double v = image.Pixel( x, y, c ) * corr[c];
            image.Pixel( x, y, c ) = std::max( 0.0, std::min( 1.0, v ) );
         }

   std::cout << "    SDSS calibration applied.\n";
}
