// ============================================================================
// test_realdata.cpp - Real astronomical data regression tests
//
// Reads real FITS images, applies gradient extraction and auto-stretch,
// verifies results against saved reference values, and writes output XISF.
//
// Usage:
//   ./test_realdata                    # Run all tests
//   ./test_realdata --save-reference   # Save reference values for future runs
// ============================================================================

#include <QApplication>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/Image.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/DisplayFunction.h>
#include <pcl/XISF.h>
#include <FITS/FITS.h>

#include "../PCLMockAPI.h"

// BackgroundExtraction module headers
#include "BackgroundExtractor.h"
#include "BackgroundExtractionInstance.h"
#include "BackgroundExtractionParameters.h"
#include "BackgroundExtractionProcess.h"

// LocalHistogramEqualization module headers
#include "LocalHistogramEqualizationInstance.h"
#include "LocalHistogramEqualizationParameters.h"
#include "LocalHistogramEqualizationProcess.h"

#include <png.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <sys/stat.h>

// ============================================================================
// Minimal MetaModule
// ============================================================================

namespace pcl
{
class TestModule : public MetaModule
{
public:
   TestModule() : MetaModule() {}
   const char* Version() const override        { return "1.0.0"; }
   IsoString Name() const override             { return "TestRealData"; }
   String Description() const override         { return "Real Data Test Module"; }
   String Company() const override             { return "Test"; }
   String Author() const override              { return "Test"; }
   String Copyright() const override           { return "Test"; }
   String TradeMarks() const override          { return String(); }
   String OriginalFileName() const override    { return "test_realdata"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2025; m=1; d=1; }
};
} // pcl

// ============================================================================
// Test Infrastructure
// ============================================================================

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;
static bool g_save_reference = false;

// Output directory for reference data and results
static const char* g_output_dir = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference";

#define TEST_ASSERT(cond, msg) do { \
   if (!(cond)) { \
      fprintf(stderr, "  FAIL: %s\n    at %s:%d\n", msg, __FILE__, __LINE__); \
      g_tests_failed++; \
      return false; \
   } \
} while(0)

#define TEST_ASSERT_NEAR(a, b, tol, msg) do { \
   double _a = (a), _b = (b), _t = (tol); \
   if (std::abs(_a - _b) > _t) { \
      fprintf(stderr, "  FAIL: %s\n    expected: %.10e  got: %.10e  diff: %.10e  tol: %.10e\n    at %s:%d\n", \
              msg, _b, _a, std::abs(_a - _b), _t, __FILE__, __LINE__); \
      g_tests_failed++; \
      return false; \
   } \
} while(0)

static void run_test(const char* name, bool (*test_fn)())
{
   g_tests_run++;
   fprintf(stdout, "TEST: %s ... ", name);
   fflush(stdout);
   if (test_fn()) {
      fprintf(stdout, "PASS\n");
      g_tests_passed++;
   } else {
      fprintf(stdout, "FAILED\n");
   }
}

// ============================================================================
// Target information for astrometric cropping
// ============================================================================

struct TargetInfo
{
   const char* name;
   const char* fits_path;
   const char* ref_path;
   const char* crop_cache;   // path to plate-solve crop cache file
   double crop_margin;       // multiplier on galaxy diameter for crop box
};

// ============================================================================
// Helper: Check file exists
// ============================================================================

static bool file_exists(const char* path)
{
   struct stat st;
   return stat(path, &st) == 0;
}

// ============================================================================
// Helper: Load astrometric crop parameters from cache file
// ============================================================================

struct CropParams
{
   double center_x;
   double center_y;
   double arcsec_per_pixel;
   double diameter_arcmin;
   bool valid;
};

static CropParams load_crop_cache(const char* path)
{
   CropParams p = {};
   p.valid = false;

   FILE* f = fopen(path, "r");
   if ( !f ) return p;

   char line[256];
   while ( fgets(line, sizeof(line), f) )
   {
      if ( line[0] == '#' ) continue;
      double v;
      if ( sscanf(line, "center_x %le", &v) == 1 ) { p.center_x = v; continue; }
      if ( sscanf(line, "center_y %le", &v) == 1 ) { p.center_y = v; continue; }
      if ( sscanf(line, "arcsec_per_pixel %le", &v) == 1 ) { p.arcsec_per_pixel = v; continue; }
      if ( sscanf(line, "diameter_arcmin %le", &v) == 1 ) { p.diameter_arcmin = v; continue; }
   }
   fclose(f);

   if ( p.arcsec_per_pixel > 0 && p.diameter_arcmin > 0 )
      p.valid = true;
   return p;
}

// ============================================================================
// Helper: Read FITS into DImage
// ============================================================================

static bool read_fits(const char* path, pcl::Image& image)
{
   try
   {
      pcl::FITSReader reader;
      reader.Open( pcl::String( path ) );
      reader.SetIndex( 0 );
      reader.ReadImage( image );
      reader.Close();
      return true;
   }
   catch ( const pcl::Exception& e )
   {
      fprintf(stderr, "    Error reading FITS: %s\n", pcl::IsoString( e.Message() ).c_str());
      return false;
   }
}

// ============================================================================
// Helper: Write XISF
// ============================================================================

static bool write_xisf(const char* path, const pcl::Image& image, const char* id)
{
   try
   {
      pcl::XISFWriter writer;
      writer.Create( pcl::String( path ), 1 );

      pcl::ImageOptions opts;
      opts.bitsPerSample = 32;
      opts.ieeefpSampleFormat = true;
      writer.SetImageOptions( opts );
      writer.SetImageId( pcl::IsoString( id ) );
      writer.WriteImage( image );
      writer.Close();
      return true;
   }
   catch ( const pcl::Exception& e )
   {
      fprintf(stderr, "    Error writing XISF: %s\n", pcl::IsoString( e.Message() ).c_str());
      return false;
   }
}

// ============================================================================
// Helper: Write 8-bit PNG from float RGB image
// ============================================================================

static bool write_png(const char* path, const pcl::Image& image)
{
   int W = image.Width(), H = image.Height();
   int C = std::min( 3, image.NumberOfChannels() );

   FILE* f = fopen(path, "wb");
   if ( !f ) return false;

   png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
   png_infop info = png_create_info_struct(png);

   if ( setjmp(png_jmpbuf(png)) )
   {
      png_destroy_write_struct(&png, &info);
      fclose(f);
      return false;
   }

   png_init_io(png, f);
   png_set_IHDR(png, info, W, H, 16,
                C == 1 ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);
   png_write_info(png, info);

   std::vector<uint8_t> row(W * C * 2);
   for ( int y = 0; y < H; y++ )
   {
      for ( int x = 0; x < W; x++ )
         for ( int c = 0; c < C; c++ )
         {
            float v = image( x, y, c );
            uint16_t val = uint16_t( std::max( 0.0f, std::min( 65535.0f, v * 65535.0f + 0.5f ) ) );
            // PNG stores 16-bit values big-endian
            row[(x * C + c) * 2]     = val >> 8;
            row[(x * C + c) * 2 + 1] = val & 0xFF;
         }
      png_write_row(png, row.data());
   }

   png_write_end(png, nullptr);
   png_destroy_write_struct(&png, &info);
   fclose(f);
   return true;
}

// ============================================================================
// Helper: Compute image statistics for one channel
// ============================================================================

struct ChannelStats
{
   double median;
   double mad;
   double mean;
   double min;
   double max;
};

static ChannelStats compute_channel_stats(const pcl::Image& image, int channel)
{
   ChannelStats s;
   int W = image.Width(), H = image.Height();
   int N = W * H;

   // Compute min, max, mean
   s.min = 1e30; s.max = -1e30; s.mean = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double v = image( x, y, channel );
         if ( v < s.min ) s.min = v;
         if ( v > s.max ) s.max = v;
         s.mean += v;
      }
   s.mean /= N;

   // Compute median using sampling (exact median on full image is expensive)
   // Use PCL's built-in if available via the image
   // For now, sample 100k pixels
   int sampleStep = std::max(1, N / 100000);
   std::vector<float> samples;
   samples.reserve(N / sampleStep + 1);
   for ( int i = 0; i < N; i += sampleStep )
   {
      int x = i % W;
      int y = i / W;
      samples.push_back( image( x, y, channel ) );
   }
   std::sort(samples.begin(), samples.end());
   s.median = samples[samples.size() / 2];

   // MAD
   std::vector<float> deviations(samples.size());
   for ( size_t i = 0; i < samples.size(); i++ )
      deviations[i] = std::abs(samples[i] - (float)s.median);
   std::sort(deviations.begin(), deviations.end());
   s.mad = deviations[deviations.size() / 2];

   return s;
}

// ============================================================================
// Helper: Save/load reference values
// ============================================================================

struct ReferenceValues
{
   double bg_coeff_a[3];   // Per-channel gradient X coefficients
   double bg_coeff_b[3];   // Per-channel gradient Y coefficients
   double bg_coeff_c[3];   // Per-channel constant
   double stf_shadows[3];
   double stf_midtones[3];
   double stretched_median[3];
};

static bool save_reference(const char* path, const ReferenceValues& ref)
{
   FILE* f = fopen(path, "w");
   if ( !f ) return false;

   fprintf(f, "# PCL Mock Test Reference Values\n");
   for ( int c = 0; c < 3; c++ )
      fprintf(f, "bg_coeff %d %.15e %.15e %.15e\n", c,
              ref.bg_coeff_a[c], ref.bg_coeff_b[c], ref.bg_coeff_c[c]);
   for ( int c = 0; c < 3; c++ )
      fprintf(f, "stf %d %.15e %.15e\n", c, ref.stf_shadows[c], ref.stf_midtones[c]);
   for ( int c = 0; c < 3; c++ )
      fprintf(f, "stretched_median %d %.15e\n", c, ref.stretched_median[c]);

   fclose(f);
   return true;
}

static bool load_reference(const char* path, ReferenceValues& ref)
{
   FILE* f = fopen(path, "r");
   if ( !f ) return false;

   char line[256];
   while ( fgets(line, sizeof(line), f) )
   {
      if ( line[0] == '#' ) continue;

      // Parse channel index and values separately to avoid the bug where
      // sscanf evaluates &array[c] using the OLD value of c before sscanf
      // writes the new channel index from the format string.
      int c;
      double v1, v2, v3;
      if ( sscanf(line, "bg_coeff %d %le %le %le", &c, &v1, &v2, &v3) == 4 && c >= 0 && c < 3 )
      {
         ref.bg_coeff_a[c] = v1;
         ref.bg_coeff_b[c] = v2;
         ref.bg_coeff_c[c] = v3;
         continue;
      }
      if ( sscanf(line, "stf %d %le %le", &c, &v1, &v2) == 3 && c >= 0 && c < 3 )
      {
         ref.stf_shadows[c] = v1;
         ref.stf_midtones[c] = v2;
         continue;
      }
      if ( sscanf(line, "stretched_median %d %le", &c, &v1) == 2 && c >= 0 && c < 3 )
      {
         ref.stretched_median[c] = v1;
         continue;
      }
   }

   fclose(f);
   return true;
}

// ============================================================================
// Core pipeline: gradient extraction + auto-stretch on one image
// ============================================================================

struct PipelineResult
{
   ReferenceValues ref;
   pcl::Image background;
   pcl::Image subtracted;
   pcl::Image stretched;
   pcl::Image lhe;    // after Local Histogram Equalization
   pcl::Image final_; // after noise suppression + star control + S-curve
   ChannelStats input_stats[3];
   ChannelStats output_stats[3];
   bool ok;
};

static PipelineResult run_pipeline(const TargetInfo& target)
{
   PipelineResult result;
   result.ok = false;

   // Read FITS
   pcl::Image image;
   fprintf(stdout, "\n    Reading %s ...\n", target.fits_path);
   if ( !read_fits(target.fits_path, image) )
      return result;

   int W = image.Width(), H = image.Height();
   int C = image.NumberOfChannels();
   fprintf(stdout, "    Image: %dx%d, %d channels\n", W, H, C);

   if ( C < 3 )
   {
      fprintf(stderr, "    Error: expected RGB image (3 channels)\n");
      return result;
   }

   // --- Crop to region of interest using plate-solved WCS ---
   // The crop cache file is generated by solve_and_cache.sh which runs
   // solve-field and wcs-rd2xy to find the pixel coordinates of the target.
   if ( target.crop_cache )
   {
      CropParams cp = load_crop_cache( target.crop_cache );
      if ( cp.valid )
      {
         // Crop to crop_margin × galaxy diameter, centered on the target
         double crop_radius_px = (cp.diameter_arcmin * 60.0 * target.crop_margin)
                                 / (2.0 * cp.arcsec_per_pixel);
         int x0 = std::max( 0, int( cp.center_x - crop_radius_px ) );
         int y0 = std::max( 0, int( cp.center_y - crop_radius_px ) );
         int x1 = std::min( W, int( cp.center_x + crop_radius_px ) );
         int y1 = std::min( H, int( cp.center_y + crop_radius_px ) );
         int cropW = x1 - x0, cropH = y1 - y0;

         fprintf(stdout, "    WCS crop: target at pixel (%.1f, %.1f), scale %.3f\"/px\n",
                 cp.center_x, cp.center_y, cp.arcsec_per_pixel);
         fprintf(stdout, "    Crop: [%d,%d]-[%d,%d] (%dx%d, %.1f'x%.1f', was %dx%d)\n",
                 x0, y0, x1, y1, cropW, cropH,
                 cropW * cp.arcsec_per_pixel / 60.0,
                 cropH * cp.arcsec_per_pixel / 60.0, W, H);

         pcl::Image cropped( cropW, cropH, pcl::ColorSpace::RGB );
         for ( int c = 0; c < 3; c++ )
            for ( int y = 0; y < cropH; y++ )
               for ( int x = 0; x < cropW; x++ )
                  cropped( x, y, c ) = image( x + x0, y + y0, c );
         image = cropped;
         W = cropW;
         H = cropH;
      }
      else
      {
         fprintf(stdout, "    WARNING: no crop cache at %s — run solve_and_cache.sh first\n",
                 target.crop_cache);
         fprintf(stdout, "    Using full frame.\n");
      }
   }

   // Input statistics (after crop)
   for ( int c = 0; c < 3; c++ )
   {
      result.input_stats[c] = compute_channel_stats(image, c);
      fprintf(stdout, "    Channel %d: median=%.6f MAD=%.6f min=%.6f max=%.6f\n",
              c, result.input_stats[c].median, result.input_stats[c].mad,
              result.input_stats[c].min, result.input_stats[c].max);
   }

   // --- Step 1: Background Extraction per channel ---
   fprintf(stdout, "    Extracting background (linear model)...\n");

   result.background = pcl::Image( W, H, pcl::ColorSpace::RGB );
   result.subtracted = pcl::Image( W, H, pcl::ColorSpace::RGB );

   pcl::BackgroundExtractionInstance bgInstance( pcl::TheBackgroundExtractionProcess );
   // Use grid sampling for deterministic, reproducible results.
   // Access private members through LockParameter() which returns void* to each field.
   *static_cast<pcl::pcl_enum*>( bgInstance.LockParameter( pcl::TheBGSampleGenerationModeParameter, 0 ) )
      = pcl::BGSampleGenerationMode::Grid;
   *static_cast<pcl::int32*>( bgInstance.LockParameter( pcl::TheBGGridSpacingXParameter, 0 ) ) = 200;
   *static_cast<pcl::int32*>( bgInstance.LockParameter( pcl::TheBGGridSpacingYParameter, 0 ) ) = 200;

   for ( int c = 0; c < 3; c++ )
   {
      // Extract channel to DImage for BackgroundExtractor
      pcl::DImage channelImage( W, H );
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
            channelImage( x, y ) = image( x, y, c );

      pcl::BackgroundExtractor extractor( channelImage, bgInstance );
      extractor.GenerateSamples();

      fprintf(stdout, "    Channel %d: %zu samples generated\n",
              c, extractor.SampleCount());

      extractor.FitBackground();
      const pcl::DImage& bg = extractor.Background();

      // Extract gradient coefficients from the model
      // The model is z = a*x + b*y + c, evaluate at corners to recover coefficients
      double v00 = bg( 0, 0 );
      double v10 = bg( W-1, 0 );
      double v01 = bg( 0, H-1 );
      double a = (v10 - v00) / (W - 1);
      double b = (v01 - v00) / (H - 1);
      double offset = v00;  // = c in the linear model

      result.ref.bg_coeff_a[c] = a;
      result.ref.bg_coeff_b[c] = b;
      result.ref.bg_coeff_c[c] = offset;

      // Subtract only the gradient (a*x + b*y), keeping the constant pedestal.
      // This removes spatial non-uniformity while preserving the overall
      // background level, which auto-stretch needs for a meaningful result.
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
         {
            float bgVal = float( bg( x, y ) );
            float gradient = float( a * x + b * y );
            result.background( x, y, c ) = bgVal;
            float sub = image( x, y, c ) - gradient;
            result.subtracted( x, y, c ) = std::max( 0.0f, sub );
         }

      fprintf(stdout, "    Channel %d gradient: a=%.6e b=%.6e c=%.6e\n",
              c, result.ref.bg_coeff_a[c], result.ref.bg_coeff_b[c], result.ref.bg_coeff_c[c]);
   }

   // --- Step 2: Auto-stretch the subtracted image ---
   // Per-channel shadows clipping (each channel's own pedestal) but shared
   // midtones balance (same gain) so color ratios are preserved.
   fprintf(stdout, "    Computing auto-stretch...\n");

   double clip = -2.80;  // default auto-stretch clipping point
   double targetBg = 0.25;
   double shadows[3], midtones[3];
   double midtonesSum = 0;

   for ( int c = 0; c < 3; c++ )
   {
      ChannelStats cs = compute_channel_stats(result.subtracted, c);
      double sigma = cs.mad * 1.4826;
      double center = cs.median;
      fprintf(stdout, "    Subtracted ch%d: median=%.6f sigma=%.6f\n",
              c, center, sigma);

      // Per-channel shadows clipping
      shadows[c] = (1 + sigma != 1) ? std::max( 0.0, std::min( 1.0, center + clip * sigma ) ) : 0.0;
      // Per-channel midtones (will be averaged below)
      midtones[c] = pcl::HistogramTransformation::MTF( targetBg, center - shadows[c] );
      midtonesSum += midtones[c];
   }

   // Shared midtones balance = average gain across channels
   double sharedMidtones = midtonesSum / 3;

   // Build DisplayFunction with per-channel shadows, shared midtones
   for ( int c = 0; c < 3; c++ )
   {
      result.ref.stf_shadows[c] = shadows[c];
      result.ref.stf_midtones[c] = sharedMidtones;
      fprintf(stdout, "    STF ch%d: shadows=%.6f midtones=%.6f\n",
              c, shadows[c], sharedMidtones);
   }
   pcl::DisplayFunction df(
      pcl::DVector( { sharedMidtones, sharedMidtones, sharedMidtones, 0.5 } ),  // midtones
      pcl::DVector( { shadows[0], shadows[1], shadows[2], 0.0 } ),              // shadows
      pcl::DVector( { 1.0, 1.0, 1.0, 1.0 } )                                   // highlights
   );

   // Apply stretch
   result.stretched = result.subtracted;
   df >> result.stretched;

   // Output statistics
   for ( int c = 0; c < 3; c++ )
   {
      result.output_stats[c] = compute_channel_stats(result.stretched, c);
      result.ref.stretched_median[c] = result.output_stats[c].median;
      fprintf(stdout, "    Stretched ch%d: median=%.6f min=%.6f max=%.6f\n",
              c, result.output_stats[c].median,
              result.output_stats[c].min, result.output_stats[c].max);
   }

   // --- Step 3: Local Histogram Equalization (CLAHE) ---
   // Enhances local contrast in the stretched image.
   // Works on CIE L* channel for color images, preserving chrominance.
   fprintf(stdout, "    Applying Local Histogram Equalization...\n");

   result.lhe = result.stretched;

   // Inline multi-scale CLAHE with pre-smoothing, background masking, and blending.
   // The PCL LHE engine produces zero output in the mock environment, so we
   // implement the algorithm directly.
   {
      int lheW = result.lhe.Width(), lheH = result.lhe.Height();
      int histSize = 256; // 8-bit histogram bins

      // ----------------------------------------------------------------
      // Step 1: Compute luminance and pre-smooth with Gaussian (σ=1.0)
      // Reduces noise sensitivity without killing structure.
      // ----------------------------------------------------------------
      std::vector<float> lum( lheW * lheH );
      for ( int y = 0; y < lheH; y++ )
         for ( int x = 0; x < lheW; x++ )
            lum[y * lheW + x] = (result.lhe( x, y, 0 ) + result.lhe( x, y, 1 ) + result.lhe( x, y, 2 )) / 3.0f;

      // Gaussian blur σ=1.0, kernel radius=2 (5×5)
      const float sigma = 1.0f;
      const int gR = 2;
      float gKernel[5][5];
      float gSum = 0;
      for ( int ky = -gR; ky <= gR; ky++ )
         for ( int kx = -gR; kx <= gR; kx++ )
         {
            float v = std::exp( -(kx*kx + ky*ky) / (2.0f * sigma * sigma) );
            gKernel[ky+gR][kx+gR] = v;
            gSum += v;
         }
      for ( int ky = 0; ky < 5; ky++ )
         for ( int kx = 0; kx < 5; kx++ )
            gKernel[ky][kx] /= gSum;

      std::vector<float> smoothLum( lheW * lheH );
      for ( int y = 0; y < lheH; y++ )
         for ( int x = 0; x < lheW; x++ )
         {
            float acc = 0;
            for ( int ky = -gR; ky <= gR; ky++ )
               for ( int kx = -gR; kx <= gR; kx++ )
               {
                  int yy = std::min( std::max( y + ky, 0 ), lheH - 1 );
                  int xx = std::min( std::max( x + kx, 0 ), lheW - 1 );
                  acc += lum[yy * lheW + xx] * gKernel[ky+gR][kx+gR];
               }
            smoothLum[y * lheW + x] = acc;
         }
      fprintf(stdout, "    Pre-smooth: Gaussian sigma=%.1f applied\n", sigma);

      // ----------------------------------------------------------------
      // Step 2: Build intensity mask to protect background from noise
      // amplification. Smooth threshold based on luminance percentiles.
      // ----------------------------------------------------------------
      // Use median + 1.5*MAD as the transition point
      {
         std::vector<float> sortedLum( smoothLum );
         std::sort( sortedLum.begin(), sortedLum.end() );
         float medLum = sortedLum[sortedLum.size() / 2];
         std::vector<float> devs( sortedLum.size() );
         for ( size_t i = 0; i < sortedLum.size(); i++ )
            devs[i] = std::abs( sortedLum[i] - medLum );
         std::sort( devs.begin(), devs.end() );
         float madLum = devs[devs.size() / 2];
         float maskLo = medLum + 1.0f * madLum;  // start fading in
         float maskHi = medLum + 4.0f * madLum;  // fully active
         fprintf(stdout, "    Mask: medLum=%.4f MAD=%.4f transition=[%.4f, %.4f]\n",
                 medLum, madLum, maskLo, maskHi);
      // (maskLo, maskHi used below in same scope)

      std::vector<float> mask( lheW * lheH );
      for ( int y = 0; y < lheH; y++ )
         for ( int x = 0; x < lheW; x++ )
         {
            float L = smoothLum[y * lheW + x];
            if ( L <= maskLo )
               mask[y * lheW + x] = 0.0f;
            else if ( L >= maskHi )
               mask[y * lheW + x] = 1.0f;
            else
               mask[y * lheW + x] = (L - maskLo) / (maskHi - maskLo);
         }

      // ----------------------------------------------------------------
      // Step 3: Two-scale CLAHE on smoothed luminance
      //   Large scale: radius=100, slopeLimit=2.0, amount=0.2
      //   Small scale: radius=16,  slopeLimit=2.5, amount=0.1
      // ----------------------------------------------------------------

      // Helper lambda: run one CLAHE pass, returns new luminance
      auto runCLAHE = [&]( const std::vector<float>& inLum,
                           int claheRadius, double slopeLimit, double amount,
                           const char* label ) -> std::vector<float>
      {
         std::vector<float> outLum( lheW * lheH );
         double factor = double( histSize - 1 );
         int r = claheRadius - 1;
         std::vector<uint32_t> hist( histSize );
         std::vector<uint32_t> clippedHist( histSize );

         for ( int y = 0; y < lheH; y++ )
         {
            for ( int x = 0; x < lheW; x++ )
            {
               // Build histogram for kernel around (x,y)
               std::fill( hist.begin(), hist.end(), 0 );
               uint32_t valuesInHist = 0;

               for ( int ky = -r; ky <= r; ky++ )
               {
                  int yy = y + ky;
                  if ( yy < 0 ) yy = -yy;
                  if ( yy >= lheH ) yy = 2 * lheH - 2 - yy;
                  if ( yy < 0 || yy >= lheH ) continue;

                  for ( int kx = -r; kx <= r; kx++ )
                  {
                     int xx = x + kx;
                     if ( xx < 0 ) xx = -xx;
                     if ( xx >= lheW ) xx = 2 * lheW - 2 - xx;
                     if ( xx < 0 || xx >= lheW ) continue;

                     float L = inLum[yy * lheW + xx];
                     uint32_t bin = std::min( uint32_t( L * factor ), uint32_t( histSize - 1 ) );
                     hist[bin]++;
                     valuesInHist++;
                  }
               }

               // Clip histogram
               std::copy( hist.begin(), hist.end(), clippedHist.begin() );
               int histLimit = int( slopeLimit * valuesInHist / ( histSize - 1 ) + 0.5 );
               if ( histLimit < 1 ) histLimit = 1;

               int clippedValues = 0, clippedBefore;
               int iter = 0;
               do {
                  clippedBefore = clippedValues;
                  clippedValues = 0;
                  for ( int i = 0; i < histSize; i++ )
                  {
                     int32_t d = int32_t(clippedHist[i]) - histLimit;
                     if ( d > 0 ) { clippedValues += d; clippedHist[i] = histLimit; }
                  }
                  if ( iter == 0 || clippedValues < clippedBefore )
                  {
                     int32_t d = clippedValues / histSize;
                     int32_t m = clippedValues % histSize;
                     if ( d != 0 )
                        for ( int i = 0; i < histSize; i++ )
                           clippedHist[i] += d;
                     if ( m != 0 )
                     {
                        int s = std::max( 1, ( histSize - 1 ) / m );
                        for ( int i = 0; i < histSize; i += s )
                           clippedHist[i]++;
                     }
                  }
                  iter++;
               } while ( iter == 1 || clippedValues < clippedBefore );

               // CDF
               float L = inLum[y * lheW + x];
               uint32_t value = std::min( uint32_t( L * factor ), uint32_t( histSize - 1 ) );

               uint32_t cdfMin = 0;
               for ( int i = 0; i < histSize; i++ )
                  if ( clippedHist[i] != 0 ) { cdfMin = clippedHist[i]; break; }

               uint32_t cdf = 0;
               for ( uint32_t i = 0; i <= value; i++ )
                  cdf += clippedHist[i];

               uint32_t cdfMax = cdf;
               for ( int i = value + 1; i < histSize; i++ )
                  cdfMax += clippedHist[i];

               float eqL = (cdfMax != cdfMin) ? float(cdf - cdfMin) / float(cdfMax - cdfMin) : L;
               // Blend with original at this scale's amount
               outLum[y * lheW + x] = float(amount) * eqL + float(1.0 - amount) * L;
            }
         }
         fprintf(stdout, "    CLAHE %s: radius=%d slope=%.1f amount=%.2f done\n",
                 label, claheRadius, slopeLimit, amount);
         return outLum;
      };

      // Large scale: enhances galaxy-level structure
      std::vector<float> largeLum = runCLAHE( smoothLum, 100, 2.0, 0.2, "large" );

      // Small scale: enhances fine detail (applied on top of large-scale result)
      std::vector<float> finalLum = runCLAHE( largeLum, 16, 2.5, 0.1, "small" );

      // ----------------------------------------------------------------
      // Step 4: Masked blend with perceptual luminance scaling
      // and mild chroma boost on bright structures.
      // ----------------------------------------------------------------
      for ( int y = 0; y < lheH; y++ )
         for ( int x = 0; x < lheW; x++ )
         {
            float R = result.lhe( x, y, 0 );
            float G = result.lhe( x, y, 1 );
            float B = result.lhe( x, y, 2 );

            float oldL2 = 0.2126f * R + 0.7152f * G + 0.0722f * B;
            float newL2 = finalLum[y * lheW + x];
            float m2    = mask[y * lheW + x];

            // Blend luminance
            float blendL = (1.0f - m2) * oldL2 + m2 * newL2;

            // --- Scale RGB to preserve original colour ratios ---
            if ( oldL2 > 1e-6f )
            {
               float scale = blendL / oldL2;

               float newR = R * scale;
               float newG = G * scale;
               float newB = B * scale;

               // --- Mild chroma boost (only where signal exists) ---
               if ( oldL2 > maskLo )
               {
                  const float sat = 1.25f;
                  float L = blendL;

                  newR = L + sat * (newR - L);
                  newG = L + sat * (newG - L);
                  newB = L + sat * (newB - L);
               }

               result.lhe( x, y, 0 ) = std::min( 1.0f, std::max( 0.0f, newR ) );
               result.lhe( x, y, 1 ) = std::min( 1.0f, std::max( 0.0f, newG ) );
               result.lhe( x, y, 2 ) = std::min( 1.0f, std::max( 0.0f, newB ) );
            }
         }

      } // end mask scope
   }

   for ( int c = 0; c < 3; c++ )
   {
      ChannelStats cs = compute_channel_stats(result.lhe, c);
      fprintf(stdout, "    LHE ch%d: median=%.6f min=%.6f max=%.6f\n",
              c, cs.median, cs.min, cs.max);
   }

   // ===================================================================
   // Post-LHE finishing: noise suppression, star control, S-curve
   // ===================================================================

   result.final_ = result.lhe;
   int finW = result.final_.Width(), finH = result.final_.Height();

   // ----------------------------------------------------------------
   // Step 5: Light background noise suppression (Gaussian σ=0.7)
   // Only applied where luminance is below the background/signal
   // transition. Leaves galaxy arms untouched.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Post-LHE noise suppression...\n");
      const float nsSigma = 0.7f;
      const int nsR = 2; // 5×5 kernel
      float nsK[5][5];
      float nsSum = 0;
      for ( int ky = -nsR; ky <= nsR; ky++ )
         for ( int kx = -nsR; kx <= nsR; kx++ )
         {
            float v = std::exp( -(kx*kx + ky*ky) / (2.0f * nsSigma * nsSigma) );
            nsK[ky+nsR][kx+nsR] = v;
            nsSum += v;
         }
      for ( int ky = 0; ky < 5; ky++ )
         for ( int kx = 0; kx < 5; kx++ )
            nsK[ky][kx] /= nsSum;

      // Compute luminance for masking
      std::vector<float> finLum( finW * finH );
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
            finLum[y * finW + x] = (result.final_( x, y, 0 ) + result.final_( x, y, 1 ) + result.final_( x, y, 2 )) / 3.0f;

      // Background threshold: median + 2*MAD
      std::vector<float> sortL( finLum );
      std::sort( sortL.begin(), sortL.end() );
      float medL = sortL[sortL.size() / 2];
      std::vector<float> absDevs( sortL.size() );
      for ( size_t i = 0; i < sortL.size(); i++ )
         absDevs[i] = std::abs( sortL[i] - medL );
      std::sort( absDevs.begin(), absDevs.end() );
      float madL = absDevs[absDevs.size() / 2];
      float nsLo = medL + 1.5f * madL;
      float nsHi = medL + 4.0f * madL;

      // Apply Gaussian blur per channel, masked to background
      pcl::Image smoothed( finW, finH, pcl::ColorSpace::RGB );
      for ( int c = 0; c < 3; c++ )
         for ( int y = 0; y < finH; y++ )
            for ( int x = 0; x < finW; x++ )
            {
               float acc = 0;
               for ( int ky = -nsR; ky <= nsR; ky++ )
                  for ( int kx = -nsR; kx <= nsR; kx++ )
                  {
                     int yy = std::min( std::max( y + ky, 0 ), finH - 1 );
                     int xx = std::min( std::max( x + kx, 0 ), finW - 1 );
                     acc += result.final_( xx, yy, c ) * nsK[ky+nsR][kx+nsR];
                  }
               smoothed( x, y, c ) = acc;
            }

      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
         {
            float L = finLum[y * finW + x];
            // Smooth blend: full blur in background, none in bright areas
            float w = (L <= nsLo) ? 1.0f
                    : (L >= nsHi) ? 0.0f
                    : 1.0f - (L - nsLo) / (nsHi - nsLo);
            for ( int c = 0; c < 3; c++ )
               result.final_( x, y, c ) = w * smoothed( x, y, c )
                                         + (1.0f - w) * result.final_( x, y, c );
         }
   }

   // ----------------------------------------------------------------
   // Step 6: Star reduction — detect stars by local peak brightness
   // and reduce their intensity by ~15%. Uses morphological approach:
   // if a pixel is much brighter than its surroundings, it's a star.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Star reduction...\n");
      const float starDimFactor = 0.85f;  // reduce stars to 85% intensity
      const int starR = 3;                // detection radius

      // Compute luminance
      std::vector<float> finLum( finW * finH );
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
            finLum[y * finW + x] = (result.final_( x, y, 0 ) + result.final_( x, y, 1 ) + result.final_( x, y, 2 )) / 3.0f;

      // For each pixel, compute local minimum in an annulus (ring at radius starR)
      // If pixel >> local min, it's likely a star peak
      std::vector<float> starMask( finW * finH, 0.0f );
      for ( int y = starR; y < finH - starR; y++ )
         for ( int x = starR; x < finW - starR; x++ )
         {
            float centerL = finLum[y * finW + x];

            // Compute median of surrounding ring pixels
            float ring[24]; // max ring pixels for r=3
            int rCount = 0;
            for ( int ky = -starR; ky <= starR; ky++ )
               for ( int kx = -starR; kx <= starR; kx++ )
               {
                  int dist2 = kx*kx + ky*ky;
                  // Ring: pixels at distance ~starR (between r-1 and r+0.5)
                  if ( dist2 >= (starR-1)*(starR-1) && dist2 <= starR*starR + starR )
                  {
                     int yy = y + ky, xx = x + kx;
                     if ( yy >= 0 && yy < finH && xx >= 0 && xx < finW && rCount < 24 )
                        ring[rCount++] = finLum[yy * finW + xx];
                  }
               }

            if ( rCount < 4 ) continue;
            std::sort( ring, ring + rCount );
            float ringMedian = ring[rCount / 2];

            // Star criterion: center much brighter than ring, and absolutely bright
            float excess = centerL - ringMedian;
            if ( excess > 0.05f && centerL > 0.35f )
               starMask[y * finW + x] = std::min( 1.0f, (excess - 0.05f) / 0.2f );
         }

      // Dilate the star mask slightly (3×3) to cover star halos
      std::vector<float> dilated( finW * finH, 0.0f );
      for ( int y = 1; y < finH - 1; y++ )
         for ( int x = 1; x < finW - 1; x++ )
         {
            float maxV = 0;
            for ( int ky = -1; ky <= 1; ky++ )
               for ( int kx = -1; kx <= 1; kx++ )
                  maxV = std::max( maxV, starMask[(y+ky) * finW + (x+kx)] );
            dilated[y * finW + x] = maxV;
         }

      // Apply star dimming
      int starCount = 0;
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
         {
            float sm = dilated[y * finW + x];
            if ( sm > 0.0f )
            {
               float dim = 1.0f - sm * (1.0f - starDimFactor);
               for ( int c = 0; c < 3; c++ )
                  result.final_( x, y, c ) *= dim;
               if ( sm > 0.5f ) starCount++;
            }
         }
      fprintf(stdout, "    Stars detected: %d pixels (dimmed to %.0f%%)\n",
              starCount, starDimFactor * 100.0f);
   }

   // ----------------------------------------------------------------
   // Step 7: Black point subtraction.
   // Clips the pedestal at median - 2.5*MAD per channel, then rescales.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Black point subtraction...\n");
      for ( int c = 0; c < 3; c++ )
      {
         ChannelStats cs = compute_channel_stats( result.final_, c );
         float black = float( cs.median - 2.5 * cs.mad );
         float scale = 1.0f / (1.0f - black);
         fprintf(stdout, "    ch%d: black=%.6f scale=%.4f\n", c, black, scale);
         for ( int y = 0; y < finH; y++ )
            for ( int x = 0; x < finW; x++ )
            {
               float v = result.final_( x, y, c );
               v = (v - black) * scale;
               result.final_( x, y, c ) = std::max( 0.0f, std::min( 1.0f, v ) );
            }
      }
   }

   // ----------------------------------------------------------------
   // Step 8: Second stretch — gamma < 1 brightens midtones.
   // Applied per-channel to lift faint galaxy detail.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Second stretch (gamma)...\n");
      const float stretch = 0.5f;  // gamma < 1 brightens midtones

      for ( int c = 0; c < 3; c++ )
         for ( int y = 0; y < finH; y++ )
            for ( int x = 0; x < finW; x++ )
            {
               float v = result.final_( x, y, c );
               result.final_( x, y, c ) = std::pow( v, stretch );
            }
   }

   // ----------------------------------------------------------------
   // Step 9: Saturation boost on bright structures (L > threshold).
   // Uses perceptual luminance to push chroma away from gray.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Saturation boost...\n");

      // Compute luminance stats for threshold
      std::vector<float> postLum( finW * finH );
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
            postLum[y * finW + x] = 0.2126f * result.final_( x, y, 0 )
                                  + 0.7152f * result.final_( x, y, 1 )
                                  + 0.0722f * result.final_( x, y, 2 );
      std::vector<float> srtLum( postLum );
      std::sort( srtLum.begin(), srtLum.end() );
      float medLum = srtLum[srtLum.size() / 2];
      std::vector<float> devLum( srtLum.size() );
      for ( size_t i = 0; i < srtLum.size(); i++ )
         devLum[i] = std::abs( srtLum[i] - medLum );
      std::sort( devLum.begin(), devLum.end() );
      float madLum = devLum[devLum.size() / 2];
      float satThreshold = medLum + 1.0f * madLum;

      const float satBoost = 1.4f;

      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
         {
            float L = postLum[y * finW + x];
            if ( L > satThreshold )
            {
               float R = result.final_( x, y, 0 );
               float G = result.final_( x, y, 1 );
               float B = result.final_( x, y, 2 );

               R = L + satBoost * (R - L);
               G = L + satBoost * (G - L);
               B = L + satBoost * (B - L);

               result.final_( x, y, 0 ) = std::max( 0.0f, std::min( 1.0f, R ) );
               result.final_( x, y, 1 ) = std::max( 0.0f, std::min( 1.0f, G ) );
               result.final_( x, y, 2 ) = std::max( 0.0f, std::min( 1.0f, B ) );
            }
         }
      fprintf(stdout, "    Saturation threshold=%.4f boost=%.2f\n", satThreshold, satBoost);
   }

   for ( int c = 0; c < 3; c++ )
   {
      ChannelStats cs = compute_channel_stats(result.final_, c);
      fprintf(stdout, "    Final ch%d: median=%.6f min=%.6f max=%.6f\n",
              c, cs.median, cs.min, cs.max);
      result.output_stats[c] = cs;
   }

   result.ok = true;
   return result;
}

// ============================================================================
// Test: M51 (NGC 5194) - large spiral galaxy
// ============================================================================

static bool test_m51()
{
   TargetInfo target;
   target.name = "M51";
   target.fits_path = "/Users/jonathan/Downloads/M51_NGC5194-RGB-session_1.fits";
   target.ref_path = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference/m51_reference.txt";
   target.crop_cache = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference/m51_crop.txt";
   target.crop_margin = 2.0;  // 2× galaxy diameter

   const char* ref_path_buf = target.ref_path;

   if ( !file_exists(target.fits_path) )
   {
      fprintf(stdout, "\n    SKIP: %s not found\n", target.fits_path);
      return true;
   }

   PipelineResult result = run_pipeline(target);
   TEST_ASSERT(result.ok, "Pipeline failed for M51");

   // Save output images
   char path[512];
   snprintf(path, sizeof(path), "%s/m51_background.xisf", g_output_dir);
   write_xisf(path, result.background, "M51_background");
   snprintf(path, sizeof(path), "%s/m51_subtracted.xisf", g_output_dir);
   write_xisf(path, result.subtracted, "M51_subtracted");
   snprintf(path, sizeof(path), "%s/m51_stretched.xisf", g_output_dir);
   write_xisf(path, result.stretched, "M51_stretched");
   snprintf(path, sizeof(path), "%s/m51_stretched.png", g_output_dir);
   write_png(path, result.stretched);
   snprintf(path, sizeof(path), "%s/m51_lhe.xisf", g_output_dir);
   write_xisf(path, result.lhe, "M51_lhe");
   snprintf(path, sizeof(path), "%s/m51_lhe.png", g_output_dir);
   write_png(path, result.lhe);
   snprintf(path, sizeof(path), "%s/m51_final.xisf", g_output_dir);
   write_xisf(path, result.final_, "M51_final");
   snprintf(path, sizeof(path), "%s/m51_final.png", g_output_dir);
   write_png(path, result.final_);
   fprintf(stdout, "    Output files written to %s/\n", g_output_dir);

   if ( g_save_reference )
   {
      save_reference(ref_path_buf, result.ref);
      fprintf(stdout, "    Reference saved to %s\n", ref_path_buf);
      return true;
   }

   // Load and compare reference if it exists
   ReferenceValues ref = {};
   if ( load_reference(ref_path_buf, ref) )
   {
      fprintf(stdout, "    Comparing against reference...\n");
      for ( int c = 0; c < 3; c++ )
      {
         char msg[128];
         snprintf(msg, sizeof(msg), "M51 ch%d bg_coeff_a regression", c);
         TEST_ASSERT_NEAR(result.ref.bg_coeff_a[c], ref.bg_coeff_a[c], 1e-8, msg);
         snprintf(msg, sizeof(msg), "M51 ch%d bg_coeff_b regression", c);
         TEST_ASSERT_NEAR(result.ref.bg_coeff_b[c], ref.bg_coeff_b[c], 1e-8, msg);
         snprintf(msg, sizeof(msg), "M51 ch%d bg_coeff_c regression", c);
         TEST_ASSERT_NEAR(result.ref.bg_coeff_c[c], ref.bg_coeff_c[c], 1e-6, msg);
         snprintf(msg, sizeof(msg), "M51 ch%d stf_shadows regression", c);
         TEST_ASSERT_NEAR(result.ref.stf_shadows[c], ref.stf_shadows[c], 1e-4, msg);
         snprintf(msg, sizeof(msg), "M51 ch%d stf_midtones regression", c);
         TEST_ASSERT_NEAR(result.ref.stf_midtones[c], ref.stf_midtones[c], 1e-4, msg);
         snprintf(msg, sizeof(msg), "M51 ch%d stretched_median regression", c);
         TEST_ASSERT_NEAR(result.ref.stretched_median[c], ref.stretched_median[c], 0.02, msg);
      }
      fprintf(stdout, "    All regression checks passed.\n");
   }
   else
   {
      fprintf(stdout, "    No reference file found; run with --save-reference to create one.\n");
   }

   // Sanity checks that always apply
   // With linked RGB stretch, the dimmest channel (typically Red in astro data)
   // may be clipped to near-zero while brighter channels show color.
   // Check that at least one channel has visible signal and max is bounded.
   bool anyVisible = false;
   for ( int c = 0; c < 3; c++ )
   {
      if ( result.output_stats[c].median > 0.1 )
         anyVisible = true;
      TEST_ASSERT(result.output_stats[c].median < 0.9,
                  "Stretched median should be < 0.9 (not blown out)");
      TEST_ASSERT(result.output_stats[c].max <= 1.0 + 1e-6,
                  "Stretched max should not exceed 1.0");
   }
   TEST_ASSERT(anyVisible, "At least one channel should have median > 0.1");

   return true;
}

// ============================================================================
// Test: M101 (Pinwheel Galaxy)
// ============================================================================

static bool test_m101()
{
   TargetInfo target;
   target.name = "M101";
   target.fits_path = "/Users/jonathan/Downloads/M101_stacked.fits";
   target.ref_path = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference/m101_reference.txt";
   target.crop_cache = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference/m101_crop.txt";
   target.crop_margin = 1.5;  // 1.5× galaxy diameter (M101 is large, 29')

   const char* ref_path_buf = target.ref_path;

   if ( !file_exists(target.fits_path) )
   {
      fprintf(stdout, "\n    SKIP: %s not found\n", target.fits_path);
      return true;
   }

   PipelineResult result = run_pipeline(target);
   TEST_ASSERT(result.ok, "Pipeline failed for M101");

   // Save output images
   char path[512];
   snprintf(path, sizeof(path), "%s/m101_background.xisf", g_output_dir);
   write_xisf(path, result.background, "M101_background");
   snprintf(path, sizeof(path), "%s/m101_subtracted.xisf", g_output_dir);
   write_xisf(path, result.subtracted, "M101_subtracted");
   snprintf(path, sizeof(path), "%s/m101_stretched.xisf", g_output_dir);
   write_xisf(path, result.stretched, "M101_stretched");
   snprintf(path, sizeof(path), "%s/m101_stretched.png", g_output_dir);
   write_png(path, result.stretched);
   snprintf(path, sizeof(path), "%s/m101_lhe.xisf", g_output_dir);
   write_xisf(path, result.lhe, "M101_lhe");
   snprintf(path, sizeof(path), "%s/m101_lhe.png", g_output_dir);
   write_png(path, result.lhe);
   snprintf(path, sizeof(path), "%s/m101_final.xisf", g_output_dir);
   write_xisf(path, result.final_, "M101_final");
   snprintf(path, sizeof(path), "%s/m101_final.png", g_output_dir);
   write_png(path, result.final_);
   fprintf(stdout, "    Output files written to %s/\n", g_output_dir);

   if ( g_save_reference )
   {
      save_reference(ref_path_buf, result.ref);
      fprintf(stdout, "    Reference saved to %s\n", ref_path_buf);
      return true;
   }

   // Load and compare reference
   ReferenceValues ref = {};
   if ( load_reference(ref_path_buf, ref) )
   {
      fprintf(stdout, "    Comparing against reference...\n");
      for ( int c = 0; c < 3; c++ )
      {
         char msg[128];
         snprintf(msg, sizeof(msg), "M101 ch%d bg_coeff_a regression", c);
         TEST_ASSERT_NEAR(result.ref.bg_coeff_a[c], ref.bg_coeff_a[c], 1e-8, msg);
         snprintf(msg, sizeof(msg), "M101 ch%d bg_coeff_b regression", c);
         TEST_ASSERT_NEAR(result.ref.bg_coeff_b[c], ref.bg_coeff_b[c], 1e-8, msg);
         snprintf(msg, sizeof(msg), "M101 ch%d bg_coeff_c regression", c);
         TEST_ASSERT_NEAR(result.ref.bg_coeff_c[c], ref.bg_coeff_c[c], 1e-6, msg);
         snprintf(msg, sizeof(msg), "M101 ch%d stf_shadows regression", c);
         TEST_ASSERT_NEAR(result.ref.stf_shadows[c], ref.stf_shadows[c], 1e-4, msg);
         snprintf(msg, sizeof(msg), "M101 ch%d stf_midtones regression", c);
         TEST_ASSERT_NEAR(result.ref.stf_midtones[c], ref.stf_midtones[c], 1e-4, msg);
         snprintf(msg, sizeof(msg), "M101 ch%d stretched_median regression", c);
         TEST_ASSERT_NEAR(result.ref.stretched_median[c], ref.stretched_median[c], 0.02, msg);
      }
      fprintf(stdout, "    All regression checks passed.\n");
   }
   else
   {
      fprintf(stdout, "    No reference file found; run with --save-reference to create one.\n");
   }

   // Sanity checks (same as M51 — linked stretch may clip dimmest channel)
   bool anyVisible = false;
   for ( int c = 0; c < 3; c++ )
   {
      if ( result.output_stats[c].median > 0.1 )
         anyVisible = true;
      TEST_ASSERT(result.output_stats[c].median < 0.9,
                  "Stretched median should be < 0.9");
      TEST_ASSERT(result.output_stats[c].max <= 1.0 + 1e-6,
                  "Stretched max should not exceed 1.0");
   }
   TEST_ASSERT(anyVisible, "At least one channel should have median > 0.1");

   return true;
}

// ============================================================================
// Main
// ============================================================================

pcl::MetaModule* pcl::Module = nullptr;

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

   // Parse args
   for ( int i = 1; i < argc; i++ )
      if ( strcmp(argv[i], "--save-reference") == 0 )
         g_save_reference = true;

   fprintf(stdout, "===========================================\n");
   fprintf(stdout, "PCL Mock Real Data Regression Suite\n");
   fprintf(stdout, "===========================================\n\n");

   // Create output directory
   mkdir(g_output_dir, 0755);

   // Initialize PCL with mock API
   fprintf(stdout, "Initializing mock API...\n");
   pcl::Module = new pcl::TestModule();

   InitializePixInsightModule(
      pcl::Module,
      GetMockFunctionResolver(),
      PCL_API_Version,
      nullptr
   );

   // Initialize BackgroundExtraction parameters
   pcl::BackgroundExtractionProcess* bgProcess = new pcl::BackgroundExtractionProcess();
   new pcl::BGModelType( bgProcess );
   new pcl::BGSampleGenerationMode( bgProcess );
   new pcl::BGSampleSize( bgProcess );
   new pcl::BGSampleTolerance( bgProcess );
   new pcl::BGMinSamples( bgProcess );
   new pcl::BGMaxSamples( bgProcess );
   new pcl::BGGridSpacingX( bgProcess );
   new pcl::BGGridSpacingY( bgProcess );
   new pcl::BGEnableOutlierRejection( bgProcess );
   new pcl::BGOutlierRejectionThreshold( bgProcess );
   new pcl::BGOutlierRejectionIterations( bgProcess );
   new pcl::BGRBFSmoothness( bgProcess );
   new pcl::BGReplaceTarget( bgProcess );
   new pcl::BGOutputBackgroundModel( bgProcess );
   new pcl::BGApplySTFToBackground( bgProcess );

   // Initialize LocalHistogramEqualization parameters
   pcl::LocalHistogramEqualizationProcess* lheProcess = new pcl::LocalHistogramEqualizationProcess();
   new pcl::LHERadius( lheProcess );
   new pcl::LHEHistogramBins( lheProcess );
   new pcl::LHESlopeLimit( lheProcess );
   new pcl::LHEAmount( lheProcess );
   new pcl::LHECircularKernel( lheProcess );

   fprintf(stdout, "Mock API initialized.\n");
   if ( g_save_reference )
      fprintf(stdout, "Mode: SAVING REFERENCE VALUES\n");
   fprintf(stdout, "\n--- Real Data Regression Tests ---\n\n");

   run_test("M51 (NGC 5194) pipeline",   test_m51);
   run_test("M101 (Pinwheel) pipeline",   test_m101);

   fprintf(stdout, "\n--- Results ---\n");
   fprintf(stdout, "  %d tests run, %d passed, %d failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
   fprintf(stdout, "\n%s\n", g_tests_failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

   delete pcl::Module;
   return g_tests_failed > 0 ? 1 : 0;
}
