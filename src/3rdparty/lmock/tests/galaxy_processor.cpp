// galaxy_processor.cpp -- Galaxy preprocessing pipeline using PCL
//
// Implements: Background Neutralization + SPCC + VeraLux HyperMetric Stretch
//
// Usage: galaxy_processor <input.xisf> [-o output.xisf] [--gaia-db /path/to/gdr3sp*.xpsd]
//                         [--no-spcc] [--no-bgneutralize] [--sdss] [--stretch]
//                         [--optimize[=N]]
//
// Build (from macosx/g++):
//   make -f makefile-arm64 PCLSRCDIR=... PCLINCDIR=... PCLLIBDIR64=... \
//        $(OBJ_DIR)/galaxy_processor

// OpenCV must be included before PCL headers. PCL's APIDefs.h has
// 'using namespace pcl;' which leaks pcl::int64/uint64 into the global
// namespace, conflicting with OpenCV's global typedefs of the same names.
#include "stackengine_cli.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <pcl/XISF.h>
#include <pcl/WCSKeywords.h>
#include <pcl/AstrometricMetadata.h>
#include <pcl/LinearTransformation.h>
#include <pcl/MetaModule.h>
#include <pcl/StarDetector.h>
#include <pcl/GaiaDatabaseFile.h>
#include <pcl/ImageVariant.h>

// FITS module reader -- exported from libFITS-pxm
#include "FITS/FITS.h"

// Plate solving via StellarSolver
#include <fitsio.h>
#include <stellarsolver.h>

#include "../PCLMockAPI.h"
#include <pcl/api/APIInterface.h>

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <numeric>
#include <memory>
#include <sstream>
#include <sys/statvfs.h>
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <random>

// OpenCV and PCL both define int64/uint64 — OpenCV as global typedefs,
// PCL as 'using' in namespace pcl. APIDefs.h does 'using namespace pcl;'
// which leaks pcl types into the global namespace. Temporarily force
// int64/uint64 to resolve to the same underlying type as both PCL and
// OpenCV use, suppressing the ambiguity in OpenCV headers.
using namespace pcl;

// ============================================================================
// MetaModule stub
// ============================================================================

class GalaxyProcessorModule : public MetaModule
{
public:
   GalaxyProcessorModule() : MetaModule() {}
   const char* Version() const override { return "1.0.0"; }
   IsoString Name() const override { return "galaxy_processor"; }
   String Description() const override { return "Galaxy Preprocessing Pipeline"; }
   String Company() const override { return String(); }
   String Author() const override { return "test"; }
   String Copyright() const override { return String(); }
   String TradeMarks() const override { return String(); }
   String OriginalFileName() const override { return "galaxy_processor"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2026; m=4; d=3; }
};

extern "C" void* IdentifyPixInsightModule()
{
   static GalaxyProcessorModule module;
   return &module;
}

// ============================================================================
// Simple TAN projection WCS (from stellina_processor SimpleTANWCS)
// Used as fallback when PCL AstrometricMetadata fails (e.g. spline WCS)
// ============================================================================

struct SimpleTANWCS
{
   double crval1, crval2;   // Reference RA, Dec (degrees)
   double crpix1, crpix2;   // Reference pixels (1-indexed)
   double cd11, cd12, cd21, cd22;  // CD matrix (degrees/pixel)
   bool valid;

   SimpleTANWCS() : crval1( 0 ), crval2( 0 ), crpix1( 0 ), crpix2( 0 ),
                    cd11( 0 ), cd12( 0 ), cd21( 0 ), cd22( 0 ), valid( false ) {}

   bool PixelToWorld( double px, double py, double& ra, double& dec ) const
   {
      if ( !valid ) return false;
      double dx = px - crpix1;
      double dy = py - crpix2;
      double xi = cd11 * dx + cd12 * dy;
      double eta = cd21 * dx + cd22 * dy;
      double ra0 = crval1 * M_PI / 180.0;
      double dec0 = crval2 * M_PI / 180.0;
      double xi_r = xi * M_PI / 180.0;
      double eta_r = eta * M_PI / 180.0;
      double cos_dec0 = cos( dec0 );
      double sin_dec0 = sin( dec0 );
      double denom = cos_dec0 - eta_r * sin_dec0;
      if ( std::abs( denom ) < 1e-12 ) return false;
      ra = ( ra0 + atan2( xi_r, denom ) ) * 180.0 / M_PI;
      dec = atan( ( sin_dec0 + eta_r * cos_dec0 ) / sqrt( xi_r * xi_r + denom * denom ) ) * 180.0 / M_PI;
      while ( ra < 0 ) ra += 360.0;
      while ( ra >= 360.0 ) ra -= 360.0;
      return true;
   }

   bool WorldToPixel( double ra, double dec, double& px, double& py ) const
   {
      if ( !valid ) return false;
      double ra0 = crval1 * M_PI / 180.0;
      double dec0 = crval2 * M_PI / 180.0;
      double ra_r = ra * M_PI / 180.0;
      double dec_r = dec * M_PI / 180.0;
      double cos_dec = cos( dec_r );
      double sin_dec = sin( dec_r );
      double cos_dec0 = cos( dec0 );
      double sin_dec0 = sin( dec0 );
      double dra = ra_r - ra0;
      double cos_dra = cos( dra );
      double denom = sin_dec * sin_dec0 + cos_dec * cos_dec0 * cos_dra;
      if ( denom <= 0 ) return false;  // behind tangent point
      double xi  = ( cos_dec * sin( dra ) ) / denom;
      double eta = ( sin_dec * cos_dec0 - cos_dec * sin_dec0 * cos_dra ) / denom;
      // xi, eta are in radians; CD matrix expects degrees
      xi  *= 180.0 / M_PI;
      eta *= 180.0 / M_PI;
      // Invert CD matrix: [dx, dy] = CD^-1 * [xi, eta]
      double det = cd11 * cd22 - cd12 * cd21;
      if ( std::abs( det ) < 1e-20 ) return false;
      double dx = ( cd22 * xi - cd12 * eta ) / det;
      double dy = ( -cd21 * xi + cd11 * eta ) / det;
      px = crpix1 + dx;
      py = crpix2 + dy;
      return true;
   }

   double Resolution() const
   {
      return sqrt( std::abs( cd11 * cd22 - cd12 * cd21 ) );
   }

   // Build from XISF properties and/or FITS keywords using PCL WCSKeywords
   bool BuildFromMetadata( const PropertyArray& properties, const FITSKeywordArray& keywords )
   {
      valid = false;

      WCSKeywords wcs( properties, keywords );

      if ( !wcs.crval1.IsDefined() || !wcs.crval2.IsDefined() ||
           !wcs.crpix1.IsDefined() || !wcs.crpix2.IsDefined() ||
           !wcs.cd1_1.IsDefined() || !wcs.cd1_2.IsDefined() ||
           !wcs.cd2_1.IsDefined() || !wcs.cd2_2.IsDefined() )
         return false;

      crval1 = wcs.crval1();
      crval2 = wcs.crval2();
      crpix1 = wcs.crpix1();
      crpix2 = wcs.crpix2();
      cd11 = wcs.cd1_1();
      cd12 = wcs.cd1_2();
      cd21 = wcs.cd2_1();
      cd22 = wcs.cd2_2();

      // Validate: need non-zero CD matrix
      double det = cd11 * cd22 - cd12 * cd21;
      if ( std::abs( det ) < 1e-20 )
         return false;

      valid = true;
      return true;
   }
};

// ============================================================================
// Plate solve an image using StellarSolver (astrometry.net)
// Returns WCSData for accurate coordinate transforms, also populates SimpleTANWCS
// ============================================================================

static std::unique_ptr<WCSData> g_solvedWCS; // Holds plate-solve WCS for coordinate transforms
static int g_cropOffsetX = 0; // Crop offset to adjust WCS pixel coords after cropping
static int g_cropOffsetY = 0;

static bool PlateSolve( const Image& image, SimpleTANWCS& wcs,
                        const FITSKeywordArray& keywords = FITSKeywordArray() )
{
   int w = image.Width();
   int h = image.Height();
   int ch = image.NumberOfChannels();

   std::cout << "  Plate solving via StellarSolver...\n";

   // Create luminance grayscale and convert to uint8
   std::vector<float> luminance( w * h );
   float minVal = 1e30f, maxVal = -1e30f;
   for ( int y = 0; y < h; ++y )
      for ( int x = 0; x < w; ++x )
      {
         float lum = 0;
         for ( int c = 0; c < std::min( ch, 3 ); ++c )
            lum += image.Pixel( x, y, c );
         lum /= std::min( ch, 3 );
         luminance[y * w + x] = lum;
         if ( lum < minVal ) minVal = lum;
         if ( lum > maxVal ) maxVal = lum;
      }

   // Downsample 2x and convert to uint8 for solver
   int dw = w / 2, dh = h / 2;
   std::vector<uint8_t> buffer( dw * dh );
   float range = maxVal - minVal;
   if ( range > 0 )
   {
      for ( int y = 0; y < dh; ++y )
         for ( int x = 0; x < dw; ++x )
         {
            int sx = x * 2, sy = y * 2;
            float avg = ( luminance[sy * w + sx] + luminance[sy * w + sx + 1] +
                          luminance[( sy + 1 ) * w + sx] + luminance[( sy + 1 ) * w + sx + 1] ) * 0.25f;
            float norm = ( avg - minVal ) / range;
            buffer[y * dw + x] = uint8_t( std::max( 0.0f, std::min( 255.0f, norm * 255.0f ) ) );
         }
   }

   // Configure statistics
   FITSImage::Statistic stats{};
   stats.width = dw;
   stats.height = dh;
   stats.channels = 1;
   stats.dataType = TBYTE;
   stats.bytesPerPixel = 1;
   stats.min[0] = minVal;
   stats.max[0] = maxVal;
   stats.mean[0] = ( minVal + maxVal ) / 2.0;
   stats.SNR = 1.0;

   // Create solver
   StellarSolver solver( stats, buffer.data() );

   // Configure parameters
   QList<Parameters> profiles = StellarSolver::getBuiltInProfiles();
   if ( profiles.isEmpty() )
   {
      std::cerr << "  ERROR: No StellarSolver parameter profiles available\n";
      return false;
   }
   Parameters params = profiles.at( 0 );
   params.search_radius = 10.0;
   params.minwidth = 0.1;
   params.maxwidth = 10.0;
   params.resort = true;
   params.autoDownsample = false;
   params.downsample = 1;
   params.inParallel = true;
   params.solverTimeLimit = 120;
   params.initialKeep = 2000;
   params.keepNum = 500;
   solver.setParameters( params );

   // Set position hint from FITS keywords if available
   double hintRA = -1, hintDec = -91;
   for ( const auto& kw : keywords )
   {
      IsoString name = kw.name.Trimmed();
      if ( name == "RA" && hintRA < 0 )
         hintRA = kw.value.Trimmed().ToDouble();
      else if ( name == "DEC" && hintDec < -90 )
         hintDec = kw.value.Trimmed().ToDouble();
      else if ( name == "OBJCTRA" && hintRA < 0 )
      {
         // Parse "hh mm ss" format
         IsoString v = kw.value.Trimmed();
         v.DeleteChar( '\'' );
         StringList parts;
         v.Break( parts, ' ', true );
         if ( parts.Length() >= 3 )
            hintRA = ( parts[0].ToDouble() + parts[1].ToDouble() / 60 + parts[2].ToDouble() / 3600 ) * 15.0;
      }
      else if ( name == "OBJCTDEC" && hintDec < -90 )
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
   if ( hintRA >= 0 && hintRA <= 360 && hintDec >= -90 && hintDec <= 90 )
   {
      solver.setSearchPositionInDegrees( hintRA, hintDec );
      std::cout << "    Position hint: RA=" << std::setprecision( 4 ) << hintRA
                << " Dec=" << hintDec << "\n";
   }

   // Find index files — search standard locations on macOS and Linux
   QStringList indexPaths;
   QStringList searchDirs = {
      "/Users/jonathan/kstars/astrometry",
      "/opt/homebrew/share/astrometry",
      "/usr/local/share/astrometry",
      "/usr/share/astrometry",
      QDir::homePath() + "/.local/share/kstars/astrometry",
      QDir::homePath() + "/kstars/astrometry",
      QDir::homePath() + "/.local/share/astrometry"
   };
   for ( const auto& dir : searchDirs )
   {
      QDir d( dir );
      if ( d.exists() )
         indexPaths << dir;
   }
   if ( indexPaths.isEmpty() )
   {
      std::cerr << "  ERROR: No astrometry index file directories found\n"
                << "  Searched:\n";
      for ( const auto& dir : searchDirs )
         std::cerr << "    " << dir.toStdString() << "\n";
      return false;
   }
   solver.setIndexFolderPaths( indexPaths );

   // Set solver mode
   solver.setProperty( "ProcessType", SSolver::SOLVE );
   solver.setProperty( "ExtractorType", SSolver::EXTRACTOR_INTERNAL );
   solver.setProperty( "SolverType", SSolver::SOLVER_STELLARSOLVER );

   // Synchronous solve
   solver.solve();

   if ( !solver.solvingDone() || !solver.hasWCSData() )
   {
      std::cerr << "  ERROR: Plate solve failed\n";
      return false;
   }

   FITSImage::Solution solution = solver.getSolution();
   double pixscale = solution.pixscale / 2.0; // correct for our 2x manual downsample

   // Store WCSData for accurate pixel-to-sky transforms
   g_solvedWCS = std::make_unique<WCSData>( solver.getWCSData() );

   // Populate SimpleTANWCS with proper rotation from plate solve.
   // StellarSolver orientation = position angle of "up" measured East of North.
   // StellarSolver pixel coords are 0-indexed top-down (same as PCL).
   // We store CRPIX in PCL convention (0-indexed, y=0 at top) since PixelToWorld
   // and WorldToPixel will be called with those coordinates.
   wcs.crval1 = solution.ra;
   wcs.crval2 = solution.dec;
   wcs.crpix1 = w / 2.0;
   wcs.crpix2 = h / 2.0;
   double degPerPix = pixscale / 3600.0;
   double theta = solution.orientation * M_PI / 180.0;
   // Negative parity (standard CCD): RA increases to the left.
   // For top-down image (y increases downward), the CD matrix is:
   wcs.cd11 = -degPerPix * cos( theta );
   wcs.cd12 = degPerPix * sin( theta );
   wcs.cd21 = degPerPix * sin( theta );
   wcs.cd22 = degPerPix * cos( theta );
   wcs.valid = true;

   std::cout << "    Plate solve SUCCESS\n";
   std::cout << "    RA=" << std::setprecision( 6 ) << solution.ra
             << " Dec=" << solution.dec
             << " pixscale=" << std::setprecision( 4 ) << pixscale << " arcsec/px"
             << " orient=" << solution.orientation << " deg\n";

   return true;
}

// ============================================================================
// Sony IMX178 Bayer filter spectral sensitivity curves
// Source: User-provided IMX178 datasheet approximation
// QE curve is common to all channels and cancels in color ratios
// ============================================================================

struct FilterPoint
{
   double wavelength; // nm
   double transmission;
};

// Red channel: IMX178 Red Bayer
static const FilterPoint s_filterR[] = {
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
static const FilterPoint s_filterG[] = {
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
static const FilterPoint s_filterB[] = {
   { 400, 0.45 }, { 410, 0.55 }, { 420, 0.62 }, { 430, 0.68 },
   { 440, 0.74 }, { 450, 0.78 }, { 460, 0.80 }, { 470, 0.78 },
   { 480, 0.70 }, { 490, 0.60 }, { 500, 0.48 }, { 510, 0.35 },
   { 520, 0.25 }, { 530, 0.18 }, { 540, 0.12 }, { 550, 0.08 },
   { 560, 0.05 }, { 570, 0.04 }, { 580, 0.03 }, { 590, 0.03 },
   { 600, 0.03 }, { 610, 0.02 }, { 620, 0.02 }, { 630, 0.03 },
   { 640, 0.04 }, { 650, 0.05 }, { 660, 0.06 }, { 670, 0.07 },
   { 680, 0.07 }, { 690, 0.07 }, { 700, 0.07 }
};

static const int s_filterRCount = sizeof( s_filterR ) / sizeof( s_filterR[0] );
static const int s_filterGCount = sizeof( s_filterG ) / sizeof( s_filterG[0] );
static const int s_filterBCount = sizeof( s_filterB ) / sizeof( s_filterB[0] );

// ============================================================================
// Interpolate filter transmission at a given wavelength (linear interp)
// ============================================================================

static double InterpolateFilter( const FilterPoint* filter, int count, double wavelength )
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

static double ConvolveSpectrumWithFilter(
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

static double Median( std::vector<double>& v )
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

static double MAD( std::vector<double>& v, double median )
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

static void SmartCropStackingEdges( Image& image, int& cropX0, int& cropY0 )
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

static void BackgroundNeutralize( Image& image )
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

static void SpectrophotometricColorCalibrate(
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
   else if ( tanWcs.BuildFromMetadata( properties, keywords ) )
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

struct SDSSstar
{
   double ra, dec;
   double psfMag_u, psfMag_g, psfMag_r, psfMag_i;
};

// Fetch SDSS star photometry via SkyServer REST API + curl
static std::vector<SDSSstar> FetchSDSSPhotometry( double centerRA, double centerDec, double radiusDeg )
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

static void SDSSColorCalibrate(
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

// ============================================================================
// VeraLux HyperMetric Stretch (ported from VeraLux Suite v2.0.7 PJSR)
// Algorithm by Riccardo Paterniti. MIT License.
// ============================================================================

struct VeraLuxParams
{
   double weights[3]    = { 0.2126, 0.7152, 0.0722 }; // Rec.709 luminance
   double log_d         = 0.0;    // 0 = auto-solve to target_bg
   double protect_b     = 5.0;    // highlight protection
   double convergence   = 3.5;    // colour → white blend speed
   double color_grip    = 1.0;    // 1.0 = luminance-preserving
   double shadow_conv   = 0.0;    // shadow convergence fade
   double target_bg     = 0.10;   // target background level
   bool   adaptive      = false;  // adaptive histogram anchor
   bool   add_pedestal  = true;   // 0.5% pedestal
   int    wavelet_scales    = 5;
   double wavelet_gain[5]  = { 0.0, 0.7, 1.0, 1.3, 0.5 };
   double wavelet_residual = 1.3;
   double wavelet_mask_lo_mad = 1.0;
   double wavelet_mask_hi_mad = 3.0;
};

static float vl_sorted_percentile( const std::vector<float>& sorted, float p )
{
   if ( sorted.empty() ) return 0.0f;
   float idx = ( p / 100.0f ) * float( sorted.size() - 1 );
   int lo = int( idx );
   int hi = std::min( lo + 1, int( sorted.size() ) - 1 );
   float w = idx - float( lo );
   return sorted[lo] * ( 1.0f - w ) + sorted[hi] * w;
}

static inline float vl_hyperbolic( float v, float D, float b )
{
   float denom = std::asinh( D + b ) - std::asinh( b );
   if ( denom < 1e-9f ) return v;
   return ( std::asinh( D * v + b ) - std::asinh( b ) ) / denom;
}

static inline float vl_mtf( float v, float m )
{
   if ( v <= 0.0f ) return 0.0f;
   if ( v >= 1.0f ) return 1.0f;
   float num = ( m - 1.0f ) * v;
   float den = ( 2.0f * m - 1.0f ) * v - m;
   if ( std::abs( den ) < 1e-9f ) return v;
   return std::max( 0.0f, std::min( 1.0f, num / den ) );
}

static inline float vl_soft_clip( float v, float threshold = 0.98f, float rolloff = 2.0f )
{
   if ( v <= threshold ) return v;
   float t = ( v - threshold ) / ( 1.0f - threshold );
   t = std::max( 0.0f, std::min( 1.0f, t ) );
   return threshold + ( 1.0f - threshold ) * ( 1.0f - std::pow( 1.0f - t, rolloff ) );
}

static float vl_anchor_stats( const Image& img, const double weights[3] )
{
   int W = img.Width(), H = img.Height();
   int total = W * H;
   int step = std::max( 1, total / 500000 );

   std::vector<float> samples;
   samples.reserve( total / step + 1 );
   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      float lum = float( weights[0] ) * img( x, y, 0 )
                + float( weights[1] ) * img( x, y, 1 )
                + float( weights[2] ) * img( x, y, 2 );
      samples.push_back( lum );
   }
   std::sort( samples.begin(), samples.end() );
   float p05 = vl_sorted_percentile( samples, 0.5f );
   return std::max( 0.0f, p05 - 0.00025f );
}

static float vl_anchor_adaptive( const Image& img, const double weights[3] )
{
   int W = img.Width(), H = img.Height();
   int total = W * H;
   int step = std::max( 1, total / 2000000 );

   const int BINS = 65536;
   std::vector<float> hist( BINS, 0.0f );
   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      float lum = float( weights[0] ) * img( x, y, 0 )
                + float( weights[1] ) * img( x, y, 1 )
                + float( weights[2] ) * img( x, y, 2 );
      int bin = int( lum * ( BINS - 1 ) );
      if ( bin >= 0 && bin < BINS )
         hist[bin] += 1.0f;
   }

   const int WIN = 50;
   std::vector<float> smoothed( BINS, 0.0f );
   for ( int k = 0; k < BINS; k++ )
   {
      float sum = 0; int cnt = 0;
      for ( int w = -WIN; w <= WIN; w++ )
      {
         int kk = k + w;
         if ( kk >= 0 && kk < BINS ) { sum += hist[kk]; cnt++; }
      }
      smoothed[k] = sum / float( cnt );
   }

   int peakIdx = 100; float peakVal = smoothed[100];
   for ( int k = 101; k < BINS; k++ )
      if ( smoothed[k] > peakVal ) { peakVal = smoothed[k]; peakIdx = k; }

   float target = peakVal * 0.06f;
   int anchorIdx = 0;
   for ( int k = peakIdx; k >= 0; k-- )
      if ( smoothed[k] < target ) { anchorIdx = k; break; }

   float anchor = float( anchorIdx ) / float( BINS - 1 );
   if ( anchor <= 0.0f || std::isnan( anchor ) )
      return vl_anchor_stats( img, weights );
   return std::max( 0.0f, anchor );
}

static double vl_solve_log_d( float medianIn, float targetMedian, float protectB )
{
   if ( medianIn < 1e-9f ) return 2.0;
   double lo = 0.0, hi = 7.0, best = 2.0;
   for ( int i = 0; i < 40; i++ )
   {
      double mid = ( lo + hi ) * 0.5;
      float D = float( std::pow( 10.0, mid ) );
      float test = vl_hyperbolic( medianIn, D, protectB );
      if ( std::abs( test - targetMedian ) < 0.0001f ) { best = mid; break; }
      if ( test < targetMedian ) lo = mid; else hi = mid;
      best = mid;
   }
   return best;
}

static void vl_adaptive_scaling( Image& img, const double weights[3], float targetBg )
{
   int W = img.Width(), H = img.Height();
   int total = W * H;
   int step = std::max( 1, total / 500000 );

   std::vector<float> luma;
   luma.reserve( total / step + 1 );
   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      luma.push_back( float( weights[0] ) * img( x, y, 0 )
                    + float( weights[1] ) * img( x, y, 1 )
                    + float( weights[2] ) * img( x, y, 2 ) );
   }

   float mean = 0;
   for ( float v : luma ) mean += v;
   mean /= float( luma.size() );
   float var = 0;
   for ( float v : luma ) var += ( v - mean ) * ( v - mean );
   float stdDev = std::sqrt( var / float( luma.size() ) );

   std::vector<float> sorted = luma;
   std::sort( sorted.begin(), sorted.end() );
   float med = sorted[sorted.size() / 2];
   float minVal = sorted.front();

   float floor_ = std::max( minVal, med - 2.7f * stdDev );
   float softCeil = vl_sorted_percentile( sorted, 99.0f );
   float hardCeil = vl_sorted_percentile( sorted, 99.99f );

   if ( softCeil <= floor_ ) softCeil = floor_ + 1e-6f;
   if ( hardCeil <= softCeil ) hardCeil = softCeil + 1e-6f;

   const float PEDESTAL = 0.001f;
   float scaleA = ( 0.98f - PEDESTAL ) / ( softCeil - floor_ + 1e-9f );
   float scaleB = ( 1.00f - PEDESTAL ) / ( hardCeil - floor_ + 1e-9f );
   float scale = std::min( scaleA, scaleB );

   for ( int c = 0; c < 3; c++ )
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
            img( x, y, c ) = std::max( 0.0f, std::min( 1.0f,
               ( img( x, y, c ) - floor_ ) * scale + PEDESTAL ) );

   // MTF shift to push median to targetBg
   luma.clear();
   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      luma.push_back( float( weights[0] ) * img( x, y, 0 )
                    + float( weights[1] ) * img( x, y, 1 )
                    + float( weights[2] ) * img( x, y, 2 ) );
   }
   std::sort( luma.begin(), luma.end() );
   float currentBg = luma[luma.size() / 2];

   if ( currentBg > 0 && currentBg < 1 && std::abs( currentBg - targetBg ) > 0.001f )
   {
      float m = ( currentBg * ( targetBg - 1.0f ) )
              / ( currentBg * ( 2.0f * targetBg - 1.0f ) - targetBg );
      for ( int c = 0; c < 3; c++ )
         for ( int y = 0; y < H; y++ )
            for ( int x = 0; x < W; x++ )
               img( x, y, c ) = vl_mtf( img( x, y, c ), m );
   }
}

// Main VeraLux HyperMetric Stretch
static void VeraLuxStretch( Image& img, const VeraLuxParams& p )
{
   int W = img.Width(), H = img.Height();
   const double* weights = p.weights;

   // Anchor
   float anchor;
   if ( p.adaptive )
      anchor = vl_anchor_adaptive( img, weights );
   else
      anchor = vl_anchor_stats( img, weights );
   std::cout << "    Anchor: " << std::setprecision( 6 ) << anchor << "\n";

   // Solve logD
   double logD = p.log_d;
   if ( logD <= 0.0 )
   {
      int total = W * H;
      int step = std::max( 1, total / 500000 );
      std::vector<float> lumaSamples;
      lumaSamples.reserve( total / step + 1 );
      for ( int i = 0; i < total; i += step )
      {
         int x = i % W, y = i / W;
         float r = std::max( 0.0f, img( x, y, 0 ) - anchor );
         float g = std::max( 0.0f, img( x, y, 1 ) - anchor );
         float b = std::max( 0.0f, img( x, y, 2 ) - anchor );
         lumaSamples.push_back( float( weights[0] ) * r + float( weights[1] ) * g + float( weights[2] ) * b );
      }
      std::sort( lumaSamples.begin(), lumaSamples.end() );
      float medLuma = lumaSamples[lumaSamples.size() / 2];
      logD = vl_solve_log_d( medLuma, float( p.target_bg ), float( p.protect_b ) );
      std::cout << "    Auto-solved logD=" << std::setprecision( 4 ) << logD
                << " (median " << medLuma << " → target " << p.target_bg << ")\n";
   }
   else
      std::cout << "    Fixed logD=" << std::setprecision( 4 ) << logD << "\n";

   const float D      = float( std::pow( 10.0, logD ) );
   const float b      = float( std::max( 0.1, p.protect_b ) );
   const float conv   = float( p.convergence );
   const float grip   = float( p.color_grip );
   const float shConv = float( p.shadow_conv );
   const float pedestal = 0.005f;
   const bool  addPed = p.add_pedestal;
   const float eps    = 1e-9f;

   // Main pixel loop
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float r  = std::max( 0.0f, img( x, y, 0 ) - anchor );
         float g  = std::max( 0.0f, img( x, y, 1 ) - anchor );
         float bv = std::max( 0.0f, img( x, y, 2 ) - anchor );

         float L     = float( weights[0] ) * r + float( weights[1] ) * g + float( weights[2] ) * bv;
         float Lsafe = L + eps;

         float Ls = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic( L, D, b ) ) );
         float k  = std::pow( Ls, conv );

         float rF = Ls * ( ( r  / Lsafe ) * ( 1.0f - k ) + k );
         float gF = Ls * ( ( g  / Lsafe ) * ( 1.0f - k ) + k );
         float bF = Ls * ( ( bv / Lsafe ) * ( 1.0f - k ) + k );

         if ( grip < 1.0f || shConv > 0.01f )
         {
            float rS = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic( r, D, b ) ) );
            float gS = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic( g, D, b ) ) );
            float bS = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic( bv, D, b ) ) );

            float cg = grip;
            if ( shConv > 0.01f ) cg *= std::pow( Ls, shConv );
            float omg = 1.0f - cg;

            rF = rF * cg + rS * omg;
            gF = gF * cg + gS * omg;
            bF = bF * cg + bS * omg;
         }

         if ( addPed )
         {
            rF = rF * ( 1.0f - pedestal ) + pedestal;
            gF = gF * ( 1.0f - pedestal ) + pedestal;
            bF = bF * ( 1.0f - pedestal ) + pedestal;
         }

         img( x, y, 0 ) = std::max( 0.0f, std::min( 1.0f, rF ) );
         img( x, y, 1 ) = std::max( 0.0f, std::min( 1.0f, gF ) );
         img( x, y, 2 ) = std::max( 0.0f, std::min( 1.0f, bF ) );
      }

   // Adaptive scaling post-processing
   std::cout << "    Adaptive scaling (target bg=" << p.target_bg << ")...\n";
   vl_adaptive_scaling( img, weights, float( p.target_bg ) );

   // Soft clip polish
   for ( int c = 0; c < 3; c++ )
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
            img( x, y, c ) = vl_soft_clip( img( x, y, c ) );

   std::cout << "    VeraLux stretch applied.\n";
}

// ============================================================================
// Starlet (à trous) wavelet — luminance-only detail enhancement
// ============================================================================

static void convolve_atrous_1d(
   const std::vector<float>& in,
   std::vector<float>& out,
   int W, int H, int scale )
{
   static const float k[5] = { 1.0f/16, 4.0f/16, 6.0f/16, 4.0f/16, 1.0f/16 };
   int step = 1 << scale;
   std::vector<float> tmp( W * H );

   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float acc = 0;
         for ( int i = -2; i <= 2; i++ )
         {
            int xx = std::min( std::max( x + i * step, 0 ), W - 1 );
            acc += in[y * W + xx] * k[i + 2];
         }
         tmp[y * W + x] = acc;
      }

   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float acc = 0;
         for ( int i = -2; i <= 2; i++ )
         {
            int yy = std::min( std::max( y + i * step, 0 ), H - 1 );
            acc += tmp[yy * W + x] * k[i + 2];
         }
         out[y * W + x] = acc;
      }
}

struct WaveletLayers
{
   std::vector<std::vector<float>> detail;
   std::vector<float> residual;
};

static WaveletLayers starlet_decompose(
   const std::vector<float>& input, int W, int H, int nScales )
{
   WaveletLayers result;
   result.detail.resize( nScales );

   std::vector<float> current = input;
   std::vector<float> smoothed( W * H );

   for ( int s = 0; s < nScales; s++ )
   {
      convolve_atrous_1d( current, smoothed, W, H, s );
      result.detail[s].resize( W * H );
      for ( int i = 0; i < W * H; i++ )
         result.detail[s][i] = current[i] - smoothed[i];
      current = smoothed;
   }
   result.residual = current;
   return result;
}

static std::vector<float> starlet_reconstruct(
   const WaveletLayers& w,
   const std::vector<float>& gain,
   float residualScale )
{
   std::vector<float> out = w.residual;
   int N = int( out.size() );
   for ( int i = 0; i < N; i++ )
      out[i] *= residualScale;

   int nScales = int( w.detail.size() );
   for ( int s = 0; s < nScales; s++ )
   {
      float g = ( s < int( gain.size() ) ) ? gain[s] : 1.0f;
      for ( int i = 0; i < N; i++ )
         out[i] += g * w.detail[s][i];
   }
   return out;
}

static void WaveletEnhance( Image& img, const VeraLuxParams& p )
{
   int W = img.Width(), H = img.Height();
   const double* weights = p.weights;

   std::cout << "    Starlet wavelet (" << p.wavelet_scales << " scales, luminance)...\n";

   // Extract luminance
   std::vector<float> lum( W * H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         lum[y * W + x] = float( weights[0] ) * img( x, y, 0 )
                         + float( weights[1] ) * img( x, y, 1 )
                         + float( weights[2] ) * img( x, y, 2 );

   std::vector<float> gain( p.wavelet_scales );
   for ( int s = 0; s < p.wavelet_scales && s < 5; s++ )
      gain[s] = float( p.wavelet_gain[s] );
   for ( int s = 5; s < p.wavelet_scales; s++ )
      gain[s] = 1.0f;

   auto wl = starlet_decompose( lum, W, H, p.wavelet_scales );

   // Spatial mask: suppress detail in background, boost in signal
   std::vector<float> sortedLum = lum;
   std::sort( sortedLum.begin(), sortedLum.end() );
   float wMed = vl_sorted_percentile( sortedLum, 50.0f );
   std::vector<float> devs( W * H );
   for ( int i = 0; i < W * H; i++ )
      devs[i] = std::abs( lum[i] - wMed );
   std::sort( devs.begin(), devs.end() );
   float wMAD = vl_sorted_percentile( devs, 50.0f );

   float wMaskLo = wMed + float( p.wavelet_mask_lo_mad ) * wMAD;
   float wMaskHi = wMed + float( p.wavelet_mask_hi_mad ) * wMAD;
   std::cout << "    Mask: med=" << std::setprecision( 4 ) << wMed
             << " MAD=" << wMAD << " transition=[" << wMaskLo << ", " << wMaskHi << "]\n";

   for ( int s = 0; s < p.wavelet_scales; s++ )
   {
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
         {
            float L = lum[y * W + x];
            float ts = ( wMaskHi > wMaskLo )
                     ? std::min( 1.0f, std::max( 0.0f, ( L - wMaskLo ) / ( wMaskHi - wMaskLo ) ) )
                     : 0.0f;
            ts = ts * ts * ( 3.0f - 2.0f * ts ); // smoothstep

            // Star suppression via local contrast
            float localMean = 0;
            int cnt = 0;
            for ( int ky = -1; ky <= 1; ky++ )
               for ( int kx = -1; kx <= 1; kx++ )
               {
                  int yy = std::min( std::max( y + ky, 0 ), H - 1 );
                  int xx = std::min( std::max( x + kx, 0 ), W - 1 );
                  localMean += lum[yy * W + xx];
                  cnt++;
               }
            localMean /= cnt;
            float peakness = L - localMean;
            float starSuppress = std::exp( -peakness * 20.0f );

            wl.detail[s][y * W + x] *= ts * starSuppress;
         }
      std::cout << "    Scale " << s << ": gain=" << std::setprecision( 2 ) << gain[s] << " (masked)\n";
   }

   std::vector<float> newLum = starlet_reconstruct( wl, gain, float( p.wavelet_residual ) );

   // Rescale RGB by pow(newL/oldL, 0.6) to preserve colour ratios
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float oldL = lum[y * W + x];
         float newL = newLum[y * W + x];
         if ( oldL > 1e-6f )
         {
            float scale = std::pow( newL / oldL, 0.6f );
            for ( int c = 0; c < 3; c++ )
               img( x, y, c ) = std::min( 1.0f,
                  std::max( 0.0f, img( x, y, c ) * scale ) );
         }
      }

   std::cout << "    Wavelet enhancement applied.\n";
}

// ============================================================================
// Optimization: histogram scoring against survey reference image
// ============================================================================

static constexpr int HIST_BINS = 256;

struct ImageHistogram
{
   float lum[HIST_BINS];      // luminance histogram (normalized)
   float ch[3][HIST_BINS];    // per-channel histograms (normalized)
   float percentiles[11];     // luminance p0,p10,p20,...,p100
};

static ImageHistogram compute_histogram( const Image& image )
{
   ImageHistogram h = {};
   int W = image.Width(), H = image.Height();
   int N = W * H;
   if ( N == 0 ) return h;

   std::vector<float> lum( N );
   int valid = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float r = image( x, y, 0 ), g = image( x, y, 1 ), b = image( x, y, 2 );
         // Clamp to [0,1] and skip non-finite values
         r = ( r != r || r < 0 ) ? 0 : ( r > 1 ? 1 : r );
         g = ( g != g || g < 0 ) ? 0 : ( g > 1 ? 1 : g );
         b = ( b != b || b < 0 ) ? 0 : ( b > 1 ? 1 : b );
         float L = 0.2126f * r + 0.7152f * g + 0.0722f * b;
         lum[valid] = L;
         valid++;
         int bin = int( L * ( HIST_BINS - 1 ) );
         bin = ( bin < 0 ) ? 0 : ( bin >= HIST_BINS ? HIST_BINS - 1 : bin );
         h.lum[bin] += 1.0f;
         float cv[3] = { r, g, b };
         for ( int c = 0; c < 3; c++ )
         {
            int cb = int( cv[c] * ( HIST_BINS - 1 ) );
            cb = ( cb < 0 ) ? 0 : ( cb >= HIST_BINS ? HIST_BINS - 1 : cb );
            h.ch[c][cb] += 1.0f;
         }
      }
   N = valid;

   // Normalize
   float invN = 1.0f / N;
   for ( int i = 0; i < HIST_BINS; i++ )
   {
      h.lum[i] *= invN;
      for ( int c = 0; c < 3; c++ )
         h.ch[c][i] *= invN;
   }

   // Luminance percentiles
   lum.resize( N );
   std::sort( lum.begin(), lum.end() );
   for ( int p = 0; p <= 10; p++ )
   {
      int idx = std::min( N - 1, int( float( p ) / 10.0f * ( N - 1 ) ) );
      h.percentiles[p] = lum[idx];
   }
   return h;
}

// Score: lower = better match to reference (range ~0-10)
static float histogram_distance( const ImageHistogram& a, const ImageHistogram& b )
{
   // Earth-mover distance on luminance CDF, normalized to [0,1]
   float lumEMD = 0;
   float cumA = 0, cumB = 0;
   for ( int i = 0; i < HIST_BINS; i++ )
   {
      cumA += a.lum[i];
      cumB += b.lum[i];
      lumEMD += std::abs( cumA - cumB );
   }
   lumEMD /= HIST_BINS;

   // Per-channel EMD, normalized
   float chEMD = 0;
   for ( int c = 0; c < 3; c++ )
   {
      cumA = cumB = 0;
      for ( int i = 0; i < HIST_BINS; i++ )
      {
         cumA += a.ch[c][i];
         cumB += b.ch[c][i];
         chEMD += std::abs( cumA - cumB );
      }
   }
   chEMD /= HIST_BINS * 3;

   // Percentile distance — weight upper percentiles more heavily
   // Percentiles are [0,1] so pDist is naturally bounded
   float pDist = 0;
   for ( int p = 0; p <= 10; p++ )
   {
      float w = ( p >= 7 ) ? 3.0f : 1.0f;
      pDist += w * std::abs( a.percentiles[p] - b.percentiles[p] );
   }

   // Signal brightness penalty: prevent "dark and flat" false minima.
   // Penalizes candidate being dimmer OR brighter than reference in upper percentiles,
   // with dimness weighted more heavily since it loses unrecoverable detail.
   float signalPenalty = 0;
   for ( int p = 7; p <= 10; p++ )
   {
      float diff = a.percentiles[p] - b.percentiles[p]; // candidate - reference
      if ( diff < 0 )
         signalPenalty += 2.0f * diff * diff;  // too dim
      else
         signalPenalty += diff * diff;          // too bright
   }

   // Dynamic range penalty
   float refRange = b.percentiles[9] - b.percentiles[2];   // p90 - p20
   float candRange = a.percentiles[9] - a.percentiles[2];
   float rangePenalty = 0;
   if ( candRange < refRange * 0.5f )
      rangePenalty = ( refRange - candRange ) * ( refRange - candRange );

   return lumEMD * 2.0f + chEMD * 0.5f + pDist * 50.0f
        + signalPenalty * 500.0f + rangePenalty * 200.0f;
}

// ============================================================================
// Fetch Legacy Survey reference image and orient to match input WCS
// ============================================================================

// MTF transfer for luminosity normalization
static inline float opt_mtf( float v, float m )
{
   if ( v <= 0 ) return 0;
   if ( v >= 1 ) return 1;
   float num = ( m - 1 ) * v;
   float den = ( 2 * m - 1 ) * v - m;
   if ( std::abs( den ) < 1e-9f ) return v;
   return std::max( 0.0f, std::min( 1.0f, num / den ) );
}

static bool FetchSurveyReference(
   Image& refImage,
   const SimpleTANWCS& wcs,
   int inputW, int inputH,
   double centerRA, double centerDec,
   double pixscaleArcsec,
   double targetBg,
   const char* layer = "sdss" )
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

static void RunOptimization( Image& image, int maxIterations, const SimpleTANWCS& wcs,
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

// ============================================================================
// Utility: case-insensitive suffix match
// ============================================================================

static bool EndsWithCI( const std::string& s, const char* suffix )
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
static Image CvMatToPCLImage( const cv::Mat& mat )
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
static cv::Mat PCLImageToCvMat( const Image& img )
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
static cv::Mat StackChannel(
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
static Image StackFramesLRGB(
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
static Image StackFramesRGB(
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


// ============================================================================
// Main
// ============================================================================

int main( int argc, char** argv )
{
   // QCoreApplication needed for StellarSolver's event loop
   QCoreApplication app( argc, argv );

   if ( argc < 2 )
   {
      std::cerr << "Usage: galaxy_processor <input.xisf|fits ...> [-o output.xisf]\n"
                << "       [--gaia-db /path/to/gdr3sp*.xpsd] [--no-spcc] [--no-bgneutralize]\n"
                << "       [--sdss] [--stretch] [--optimize[=N]]\n"
                << "       [--stack] [--lrgb] [--gradient=0|1|2|3] [--global-gradient] [--survey-mask]\n"
                << "       [--sigma=N] [--clip-iters=N] [--max-frames=N]\n"
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
   bool doSPCC = true;
   bool doBgNeutralize = true;
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
      else if ( arg == "--no-spcc" )
         doSPCC = false;
      else if ( arg == "--no-bgneutralize" )
         doBgNeutralize = false;
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
         std::cerr << "  Use --gaia-db <path> to specify, or --no-spcc to skip SPCC\n";
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
            reader.ReadImage( image );
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
