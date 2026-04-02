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

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
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

// ============================================================================
// Pipeline parameters — all tweakable from command line via --key=value
// Convention: group.subgroup.param (e.g. --wavelet.gain2=1.5)
// ============================================================================

struct PipelineParams
{
   // Crop
   double m51_crop_margin   = 2.0;
   double m101_crop_margin  = 1.5;

   // Background extraction
   int    bg_grid_spacing   = 200;

   // GHS (Generalised Hyperbolic Stretch)
   double ghs_D             = 0.15;   // target median (auto-D); >1.0 = fixed D
   double ghs_b             = 0.0;    // shape: 0=asinh-like, >0 towards log
   double ghs_SP            = 0.0;    // symmetry point (0 = auto from data)
   double ghs_LP            = 0.0;    // local intensity protect (0 = auto)
   double ghs_clip          = -2.80;  // shadows clip in sigma units

   // Chrominance noise reduction
   double chroma_sigma      = 1.5;
   int    chroma_radius     = 3;

   // Starlet wavelet decomposition (5 scales)
   int    wavelet_scales    = 5;
   double wavelet_gain0     = 0.0;   // scale 0: kill noise
   double wavelet_gain1     = 0.6;   // scale 1: slight detail
   double wavelet_gain2     = 1.2;   // scale 2: arm contrast
   double wavelet_gain3     = 1.0;   // scale 3: structure depth
   double wavelet_gain4     = 0.5;   // scale 4: larger scale separation
   double wavelet_residual  = 1.2;   // restore smooth glow   // residual (smooth) scale factor
   double wavelet_mask_lo_mad = 1.8; // mask starts at median + N*MAD
   double wavelet_mask_hi_mad = 4.5; // mask fully active at median + N*MAD

   // Post-wavelet noise suppression
   double noise_sigma       = 0.8;
   double noise_mask_lo_mad = 1.5;
   double noise_mask_hi_mad = 4.0;

   // Star reduction
   double star_dim          = 0.92;  // reduce to this fraction
   int    star_radius       = 3;
   double star_excess       = 4.5;   // excess in units of local ring MAD
   double star_min_bright   = 0.35;  // minimum absolute brightness

   // Black point
   double black_mad         = 2.4;   // median - N*MAD

   // Adaptive gamma (smoothstep from 1.0 to gamma based on luminance)
   double gamma             = 0.8;
   double gamma_lo_mad      = 1.0;   // smoothstep starts at median + N*MAD
   double gamma_hi_mad      = 5.0;   // smoothstep ends at median + N*MAD

   // Smoothstep saturation boost
   double sat_boost         = 1.3;
   double sat_lo_mad        = 1.0;   // smoothstep starts at median + N*MAD
   double sat_hi_mad        = 4.0;   // smoothstep ends at median + N*MAD
};

static PipelineParams g_params;

// Parse --key=value into PipelineParams
static void parse_param( const char* arg )
{
   // Skip leading --
   if ( arg[0] == '-' && arg[1] == '-' )
      arg += 2;
   else
      return;

   const char* eq = strchr( arg, '=' );
   if ( !eq ) return;

   std::string key( arg, eq - arg );
   double val = atof( eq + 1 );

   // Crop
   if      ( key == "m51.crop"  )             g_params.m51_crop_margin = val;
   else if ( key == "m101.crop" )             g_params.m101_crop_margin = val;
   // Background
   else if ( key == "bg.grid" )               g_params.bg_grid_spacing = int(val);
   // GHS
   else if ( key == "ghs.D" )                 g_params.ghs_D = val;
   else if ( key == "ghs.b" )                 g_params.ghs_b = val;
   else if ( key == "ghs.SP" )                g_params.ghs_SP = val;
   else if ( key == "ghs.LP" )                g_params.ghs_LP = val;
   else if ( key == "ghs.clip" )              g_params.ghs_clip = val;
   // Chrominance
   else if ( key == "chroma.sigma" )          g_params.chroma_sigma = val;
   else if ( key == "chroma.radius" )         g_params.chroma_radius = int(val);
   // Wavelet
   else if ( key == "wavelet.scales" )        g_params.wavelet_scales = int(val);
   else if ( key == "wavelet.gain0" )         g_params.wavelet_gain0 = val;
   else if ( key == "wavelet.gain1" )         g_params.wavelet_gain1 = val;
   else if ( key == "wavelet.gain2" )         g_params.wavelet_gain2 = val;
   else if ( key == "wavelet.gain3" )         g_params.wavelet_gain3 = val;
   else if ( key == "wavelet.gain4" )         g_params.wavelet_gain4 = val;
   else if ( key == "wavelet.residual" )      g_params.wavelet_residual = val;
   else if ( key == "wavelet.mask.lo" )       g_params.wavelet_mask_lo_mad = val;
   else if ( key == "wavelet.mask.hi" )       g_params.wavelet_mask_hi_mad = val;
   // Noise
   else if ( key == "noise.sigma" )           g_params.noise_sigma = val;
   else if ( key == "noise.mask.lo" )         g_params.noise_mask_lo_mad = val;
   else if ( key == "noise.mask.hi" )         g_params.noise_mask_hi_mad = val;
   // Stars
   else if ( key == "star.dim" )              g_params.star_dim = val;
   else if ( key == "star.radius" )           g_params.star_radius = int(val);
   else if ( key == "star.excess" )           g_params.star_excess = val;
   else if ( key == "star.bright" )           g_params.star_min_bright = val;
   // Black point
   else if ( key == "black.mad" )             g_params.black_mad = val;
   // Gamma
   else if ( key == "gamma" )                 g_params.gamma = val;
   else if ( key == "gamma.lo" )              g_params.gamma_lo_mad = val;
   else if ( key == "gamma.hi" )              g_params.gamma_hi_mad = val;
   // Saturation
   else if ( key == "sat.boost" )             g_params.sat_boost = val;
   else if ( key == "sat.lo" )                g_params.sat_lo_mad = val;
   else if ( key == "sat.hi" )                g_params.sat_hi_mad = val;
   else
      fprintf(stderr, "WARNING: unknown parameter --%s\n", key.c_str());
}

static void print_params()
{
   fprintf(stdout, "Parameters:\n");
   fprintf(stdout, "  m51.crop=%.1f  m101.crop=%.1f  bg.grid=%d\n",
           g_params.m51_crop_margin, g_params.m101_crop_margin, g_params.bg_grid_spacing);
   fprintf(stdout, "  ghs: D=%.1f b=%.1f SP=%.3f LP=%.3f clip=%.2f\n",
           g_params.ghs_D, g_params.ghs_b, g_params.ghs_SP, g_params.ghs_LP, g_params.ghs_clip);
   fprintf(stdout, "  chroma.sigma=%.1f  chroma.radius=%d\n",
           g_params.chroma_sigma, g_params.chroma_radius);
   fprintf(stdout, "  wavelet: scales=%d gains=[%.1f, %.1f, %.1f, %.1f, %.1f] residual=%.1f\n",
           g_params.wavelet_scales,
           g_params.wavelet_gain0, g_params.wavelet_gain1, g_params.wavelet_gain2,
           g_params.wavelet_gain3, g_params.wavelet_gain4, g_params.wavelet_residual);
   fprintf(stdout, "  wavelet.mask=[%.1f,%.1f]mad\n",
           g_params.wavelet_mask_lo_mad, g_params.wavelet_mask_hi_mad);
   fprintf(stdout, "  noise.sigma=%.1f  noise.mask=[%.1f,%.1f]mad\n",
           g_params.noise_sigma, g_params.noise_mask_lo_mad, g_params.noise_mask_hi_mad);
   fprintf(stdout, "  star: dim=%.2f r=%d excess=%.2f bright=%.2f\n",
           g_params.star_dim, g_params.star_radius, g_params.star_excess, g_params.star_min_bright);
   fprintf(stdout, "  black.mad=%.1f  gamma=%.2f  gamma.[lo,hi]=[%.1f,%.1f]mad\n",
           g_params.black_mad, g_params.gamma, g_params.gamma_lo_mad, g_params.gamma_hi_mad);
   fprintf(stdout, "  sat.boost=%.1f  sat.[lo,hi]=[%.1f,%.1f]mad\n",
           g_params.sat_boost, g_params.sat_lo_mad, g_params.sat_hi_mad);
   fprintf(stdout, "\n");
}

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
// Sampled median/MAD — O(N_samples log N_samples) instead of O(N log N)
// Uses deterministic pseudo-random sampling (fixed seed) for reproducibility.
// ============================================================================

struct MedianMAD { float median; float mad; };

static MedianMAD sampled_median_mad( const std::vector<float>& data, int nSamples = 100000 )
{
   const int N = int( data.size() );
   if ( N == 0 )
      return { 0.0f, 0.0f };

   nSamples = std::min( nSamples, N );

   std::mt19937 rng( 12345 ); // deterministic seed
   std::uniform_int_distribution<int> dist( 0, N - 1 );

   std::vector<float> samples( nSamples );
   for ( int i = 0; i < nSamples; i++ )
      samples[i] = data[dist( rng )];

   std::sort( samples.begin(), samples.end() );
   float med = samples[nSamples / 2];

   std::vector<float> devs( nSamples );
   for ( int i = 0; i < nSamples; i++ )
      devs[i] = std::abs( samples[i] - med );
   std::sort( devs.begin(), devs.end() );
   float mad = devs[nSamples / 2];

   return { med, mad };
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
// Generalised Hyperbolic Stretch (GHS)
//
// Mike Cranfield's formulation. For b=0 this is an inverse hyperbolic sine
// (asinh) stretch; b>0 moves towards logarithmic; b<0 towards linear.
//
// Parameters:
//   D  - stretch factor (higher = stronger nonlinear stretch)
//   b  - shape parameter (0 = asinh)
//   SP - symmetry point (where the stretch is centred)
//   LP - local intensity protect (shadows below this are linear)
// ============================================================================

static float ghs_stretch( float x, float D, float b, float SP, float LP )
{
   if ( x <= 0.0f ) return 0.0f;
   if ( x >= 1.0f ) return 1.0f;

   // Shadows protection: linear below LP
   if ( LP > 0 && x < LP )
      return x * ghs_stretch( LP, D, b, SP, LP ) / LP;

   // For b == 0: asinh-based stretch
   // f(x) = asinh(D * (x - SP)) / asinh(D * (1 - SP))  [normalised]
   if ( std::abs( b ) < 1e-6f )
   {
      float q1 = std::asinh( D * (x - SP) );
      float q0 = std::asinh( D * (0.0f - SP) );
      float q2 = std::asinh( D * (1.0f - SP) );
      if ( std::abs( q2 - q0 ) < 1e-10f ) return x;
      return (q1 - q0) / (q2 - q0);
   }

   // General case: b != 0
   // f(x) = (exp(b * asinh(D*(x-SP))) - exp(b * asinh(D*(0-SP)))) /
   //         (exp(b * asinh(D*(1-SP))) - exp(b * asinh(D*(0-SP))))
   float e1 = std::exp( b * std::asinh( D * (x - SP) ) );
   float e0 = std::exp( b * std::asinh( D * (0.0f - SP) ) );
   float e2 = std::exp( b * std::asinh( D * (1.0f - SP) ) );
   float denom = e2 - e0;
   if ( std::abs( denom ) < 1e-10f ) return x;
   return (e1 - e0) / denom;
}

// ============================================================================
// Starlet (à trous) wavelet transform
// ============================================================================

static const float s_atrousKernel[5] = { 1, 4, 6, 4, 1 }; // B3 spline, sum=16

// À trous convolution on a flat float buffer (single channel)
static void convolve_atrous_1d(
   const std::vector<float>& in,
   std::vector<float>& out,
   int W, int H,
   int scale ) // 0,1,2,... (step = 2^scale)
{
   int step = 1 << scale;
   const float norm = 1.0f / 16.0f;

   std::vector<float> tmp( W * H );

   // Horizontal pass
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float acc = 0;
         for ( int k = -2; k <= 2; k++ )
         {
            int xx = x + k * step;
            xx = std::min( std::max( xx, 0 ), W - 1 );
            acc += s_atrousKernel[k+2] * in[y * W + xx];
         }
         tmp[y * W + x] = acc * norm;
      }

   // Vertical pass
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float acc = 0;
         for ( int k = -2; k <= 2; k++ )
         {
            int yy = y + k * step;
            yy = std::min( std::max( yy, 0 ), H - 1 );
            acc += s_atrousKernel[k+2] * tmp[yy * W + x];
         }
         out[y * W + x] = acc * norm;
      }
}

struct WaveletLayers1D
{
   std::vector<std::vector<float>> detail; // w0, w1, ...
   std::vector<float> residual;            // last smooth
   int W, H;
};

static WaveletLayers1D starlet_decompose_1d( const std::vector<float>& input, int W, int H, int nScales )
{
   WaveletLayers1D w;
   w.W = W;
   w.H = H;
   std::vector<float> current = input;

   for ( int s = 0; s < nScales; s++ )
   {
      std::vector<float> smooth( W * H );
      convolve_atrous_1d( current, smooth, W, H, s );

      std::vector<float> detail( W * H );
      for ( int i = 0; i < W * H; i++ )
         detail[i] = current[i] - smooth[i];

      w.detail.push_back( detail );
      current = smooth;
   }

   w.residual = current;
   return w;
}

static std::vector<float> starlet_reconstruct_1d(
   const WaveletLayers1D& w,
   const std::vector<float>& gain, // one per scale
   float residualScale = 1.0f )
{
   int N = w.W * w.H;
   std::vector<float> out( N );

   // Scale residual (smooth base)
   for ( int i = 0; i < N; i++ )
      out[i] = w.residual[i] * residualScale;

   for ( size_t s = 0; s < w.detail.size(); s++ )
   {
      float g = (s < gain.size()) ? gain[s] : 1.0f;
      for ( int i = 0; i < N; i++ )
         out[i] += g * w.detail[s][i];
   }

   // Clamp to [0,1]
   for ( int i = 0; i < N; i++ )
      out[i] = std::min( 1.0f, std::max( 0.0f, out[i] ) );

   return out;
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
   *static_cast<pcl::int32*>( bgInstance.LockParameter( pcl::TheBGGridSpacingXParameter, 0 ) ) = g_params.bg_grid_spacing;
   *static_cast<pcl::int32*>( bgInstance.LockParameter( pcl::TheBGGridSpacingYParameter, 0 ) ) = g_params.bg_grid_spacing;

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

   // --- Step 2: Generalised Hyperbolic Stretch (GHS) ---
   // Per-channel shadows clipping, then shared GHS stretch to preserve colour.
   fprintf(stdout, "    Computing GHS stretch...\n");

   double clip = g_params.ghs_clip;
   double shadows[3];

   for ( int c = 0; c < 3; c++ )
   {
      ChannelStats cs = compute_channel_stats(result.subtracted, c);
      double sigma = cs.mad * 1.4826;
      double center = cs.median;
      fprintf(stdout, "    Subtracted ch%d: median=%.6f sigma=%.6f\n",
              c, center, sigma);
      shadows[c] = (1 + sigma != 1) ? std::max( 0.0, std::min( 1.0, center + clip * sigma ) ) : 0.0;
   }

   // Compute shared SP from average median (after shadow subtraction)
   double avgMedian = 0;
   for ( int c = 0; c < 3; c++ )
   {
      ChannelStats cs = compute_channel_stats(result.subtracted, c);
      avgMedian += (cs.median - shadows[c]) / (1.0 - shadows[c]);
   }
   avgMedian /= 3;

   float ghsB  = float( g_params.ghs_b );
   float ghsSP = (g_params.ghs_SP > 0) ? float( g_params.ghs_SP ) : float( avgMedian );
   float ghsLP = (g_params.ghs_LP > 0) ? float( g_params.ghs_LP ) : float( avgMedian * 0.5 );

   // Binary search for D that places the stretched median at target_bg.
   // Evaluate on the green channel median (middle channel, good proxy).
   float targetBg = float( g_params.ghs_D ); // reuse D param as target if auto
   if ( targetBg > 1.0f )
   {
      // D was specified directly — use it
      float ghsD = targetBg;
      fprintf(stdout, "    GHS: using fixed D=%.1f\n", ghsD);

      for ( int c = 0; c < 3; c++ )
      {
         result.ref.stf_shadows[c] = shadows[c];
         result.ref.stf_midtones[c] = ghsD;
         fprintf(stdout, "    GHS ch%d: shadows=%.6f\n", c, shadows[c]);
      }
      fprintf(stdout, "    GHS params: D=%.1f b=%.1f SP=%.6f LP=%.6f\n",
              ghsD, ghsB, ghsSP, ghsLP);

      result.stretched = result.subtracted;
      int sW = result.stretched.Width(), sH = result.stretched.Height();
      for ( int c = 0; c < 3; c++ )
      {
         float sh = float( shadows[c] );
         float range = 1.0f - sh;
         if ( range < 1e-6f ) range = 1e-6f;
         for ( int y = 0; y < sH; y++ )
            for ( int x = 0; x < sW; x++ )
            {
               float v = result.stretched( x, y, c );
               v = std::max( 0.0f, (v - sh) / range );
               v = ghs_stretch( v, ghsD, ghsB, ghsSP, ghsLP );
               result.stretched( x, y, c ) = std::min( 1.0f, std::max( 0.0f, v ) );
            }
      }
   }
   else
   {
      // D <= 1.0 means "target median luminance" — search for optimal D
      float targetMed = targetBg;
      if ( targetMed <= 0 ) targetMed = 0.15f;

      // Binary search: what D maps avgMedian to targetMed?
      float dLo = 1.0f, dHi = 100000.0f;
      float bestD = 100.0f;

      for ( int iter = 0; iter < 50; iter++ )
      {
         float dMid = (dLo + dHi) * 0.5f;
         float stretched = ghs_stretch( float(avgMedian), dMid, ghsB, ghsSP, ghsLP );

         if ( stretched < targetMed )
            dLo = dMid;
         else
            dHi = dMid;

         if ( std::abs( stretched - targetMed ) < 1e-5f )
            break;
      }
      bestD = (dLo + dHi) * 0.5f;

      fprintf(stdout, "    GHS auto-D: target median=%.3f, found D=%.1f (maps %.6f -> %.6f)\n",
              targetMed, bestD, float(avgMedian),
              ghs_stretch( float(avgMedian), bestD, ghsB, ghsSP, ghsLP ));

      for ( int c = 0; c < 3; c++ )
      {
         result.ref.stf_shadows[c] = shadows[c];
         result.ref.stf_midtones[c] = bestD;
         fprintf(stdout, "    GHS ch%d: shadows=%.6f\n", c, shadows[c]);
      }
      fprintf(stdout, "    GHS params: D=%.1f b=%.1f SP=%.6f LP=%.6f\n",
              bestD, ghsB, ghsSP, ghsLP);

      result.stretched = result.subtracted;
      int sW = result.stretched.Width(), sH = result.stretched.Height();
      for ( int c = 0; c < 3; c++ )
      {
         float sh = float( shadows[c] );
         float range = 1.0f - sh;
         if ( range < 1e-6f ) range = 1e-6f;
         for ( int y = 0; y < sH; y++ )
            for ( int x = 0; x < sW; x++ )
            {
               float v = result.stretched( x, y, c );
               v = std::max( 0.0f, (v - sh) / range );
               v = ghs_stretch( v, bestD, ghsB, ghsSP, ghsLP );
               result.stretched( x, y, c ) = std::min( 1.0f, std::max( 0.0f, v ) );
            }
      }
   }

   // Output statistics
   for ( int c = 0; c < 3; c++ )
   {
      result.output_stats[c] = compute_channel_stats(result.stretched, c);
      result.ref.stretched_median[c] = result.output_stats[c].median;
      fprintf(stdout, "    Stretched ch%d: median=%.6f min=%.6f max=%.6f\n",
              c, result.output_stats[c].median,
              result.output_stats[c].min, result.output_stats[c].max);
   }

   // --- Step 2b: Chrominance noise reduction ---
   // Smooth only the colour channels in CIE Lab space, preserving luminance detail.
   fprintf(stdout, "    Applying chrominance noise reduction...\n");
   {
      int cW = result.stretched.Width(), cH = result.stretched.Height();
      const int cR = g_params.chroma_radius;
      const float cSigma = float( g_params.chroma_sigma );

      // Precompute Gaussian kernel
      int cKSize = 2 * cR + 1;
      std::vector<float> cKernel( cKSize * cKSize );
      float cKSum = 0;
      for ( int ky = -cR; ky <= cR; ky++ )
         for ( int kx = -cR; kx <= cR; kx++ )
         {
            float v = std::exp( -(kx*kx + ky*ky) / (2.0f * cSigma * cSigma) );
            cKernel[(ky+cR) * cKSize + (kx+cR)] = v;
            cKSum += v;
         }
      for ( auto& v : cKernel ) v /= cKSum;

      auto toLinear = []( float v ) {
         return v <= 0.04045f ? v / 12.92f : std::pow( (v + 0.055f) / 1.055f, 2.4f );
      };
      auto labF = []( float t ) {
         return t > 0.008856f ? std::cbrt( t ) : 7.787f * t + 16.0f / 116.0f;
      };
      auto labFinv = []( float t ) {
         return t > 0.2069f ? t * t * t : (t - 16.0f / 116.0f) / 7.787f;
      };
      auto toGamma = []( float v ) {
         v = std::max( 0.0f, v );
         return v <= 0.0031308f ? 12.92f * v : 1.055f * std::pow( v, 1.0f / 2.4f ) - 0.055f;
      };

      // Convert entire image to Lab a*/b* for efficient kernel access
      std::vector<float> labA( cW * cH ), labB( cW * cH ), labL( cW * cH );
      for ( int y = 0; y < cH; y++ )
         for ( int x = 0; x < cW; x++ )
         {
            float r = toLinear( result.stretched( x, y, 0 ) );
            float g = toLinear( result.stretched( x, y, 1 ) );
            float b = toLinear( result.stretched( x, y, 2 ) );
            float X = r*0.4124f + g*0.3576f + b*0.1805f;
            float Y = r*0.2126f + g*0.7152f + b*0.0722f;
            float Z = r*0.0193f + g*0.1192f + b*0.9505f;
            float fx = labF( X / 0.9505f ), fy = labF( Y ), fz = labF( Z / 1.0890f );
            labL[y * cW + x] = 116.0f * fy - 16.0f;
            labA[y * cW + x] = 500.0f * (fx - fy);
            labB[y * cW + x] = 200.0f * (fy - fz);
         }

      // Blur only a* and b* channels
      std::vector<float> smoothA( cW * cH ), smoothB( cW * cH );
      for ( int y = 0; y < cH; y++ )
         for ( int x = 0; x < cW; x++ )
         {
            float aAcc = 0, bAcc = 0;
            for ( int ky = -cR; ky <= cR; ky++ )
               for ( int kx = -cR; kx <= cR; kx++ )
               {
                  int yy = std::min( std::max( y + ky, 0 ), cH - 1 );
                  int xx = std::min( std::max( x + kx, 0 ), cW - 1 );
                  float w = cKernel[(ky+cR) * cKSize + (kx+cR)];
                  aAcc += w * labA[yy * cW + xx];
                  bAcc += w * labB[yy * cW + xx];
               }
            smoothA[y * cW + x] = aAcc;
            smoothB[y * cW + x] = bAcc;
         }

      // Reconstruct with blurred chrominance, original L
      for ( int y = 0; y < cH; y++ )
         for ( int x = 0; x < cW; x++ )
         {
            float L = labL[y * cW + x];
            float fy2 = (L + 16.0f) / 116.0f;
            float fx2 = smoothA[y * cW + x] / 500.0f + fy2;
            float fz2 = fy2 - smoothB[y * cW + x] / 200.0f;
            float Xo = labFinv( fx2 ) * 0.9505f;
            float Yo = labFinv( fy2 );
            float Zo = labFinv( fz2 ) * 1.0890f;
            result.stretched( x, y, 0 ) = std::min( 1.0f, std::max( 0.0f,
               toGamma(  3.2406f * Xo - 1.5372f * Yo - 0.4986f * Zo ) ) );
            result.stretched( x, y, 1 ) = std::min( 1.0f, std::max( 0.0f,
               toGamma( -0.9689f * Xo + 1.8758f * Yo + 0.0415f * Zo ) ) );
            result.stretched( x, y, 2 ) = std::min( 1.0f, std::max( 0.0f,
               toGamma(  0.0557f * Xo - 0.2040f * Yo + 1.0570f * Zo ) ) );
         }
      fprintf(stdout, "    Chrominance smoothing applied (sigma=%.1f, radius=%d)\n", cSigma, cR);
   }

   // --- Step 3: Local Histogram Equalization (CLAHE) ---
   // Enhances local contrast in the stretched image.
   // Works on CIE L* channel for color images, preserving chrominance.
   fprintf(stdout, "    Applying Local Histogram Equalization...\n");

   // ===================================================================
   // Starlet wavelet decomposition — replaces CLAHE
   // ===================================================================
   {
      fprintf(stdout, "    Starlet wavelet decomposition (%d scales, luminance only)...\n",
              g_params.wavelet_scales);

      int wW = result.stretched.Width(), wH = result.stretched.Height();

      // Extract perceptual luminance (BT.709)
      std::vector<float> lum( wW * wH );
      for ( int y = 0; y < wH; y++ )
         for ( int x = 0; x < wW; x++ )
            lum[y * wW + x] = 0.2126f * result.stretched( x, y, 0 )
                             + 0.7152f * result.stretched( x, y, 1 )
                             + 0.0722f * result.stretched( x, y, 2 );

      std::vector<float> gain = {
         float( g_params.wavelet_gain0 ),
         float( g_params.wavelet_gain1 ),
         float( g_params.wavelet_gain2 ),
         float( g_params.wavelet_gain3 ),
         float( g_params.wavelet_gain4 )
      };
      gain.resize( g_params.wavelet_scales, 1.0f );

      auto w = starlet_decompose_1d( lum, wW, wH, g_params.wavelet_scales );

      // Spatial mask: suppress detail in background, boost in signal
      auto [wMed, wMAD] = sampled_median_mad( lum );
      float wMaskLo = wMed + float( g_params.wavelet_mask_lo_mad ) * wMAD;
      float wMaskHi = wMed + float( g_params.wavelet_mask_hi_mad ) * wMAD;
      fprintf(stdout, "    Wavelet mask: med=%.4f MAD=%.4f transition=[%.4f, %.4f]\n",
              wMed, wMAD, wMaskLo, wMaskHi);

      for ( int s = 0; s < g_params.wavelet_scales; s++ )
      {
         for ( int y = 0; y < wH; y++ )
            for ( int x = 0; x < wW; x++ )
            {
               float L = lum[y * wW + x];

               // Structure mask: ramps up from background into signal
               float ts = (wMaskHi > wMaskLo)
                        ? std::min( 1.0f, std::max( 0.0f, (L - wMaskLo) / (wMaskHi - wMaskLo) ) )
                        : 0.0f;
               ts = ts * ts * (3.0f - 2.0f * ts); // smoothstep

               // Highlight rolloff: protects saturated cores from ringing
               float th = std::min( 1.0f, std::max( 0.0f, (L - 0.5f) / (0.85f - 0.5f) ) );
               th = th * th * (3.0f - 2.0f * th); // smoothstep

               float mask = ts * (1.0f - th);
               w.detail[s][y * wW + x] *= mask;
            }
         fprintf(stdout, "    scale %d: gain=%.2f (masked)\n", s, gain[s]);
      }

      std::vector<float> newLum = starlet_reconstruct_1d( w, gain,
         float( g_params.wavelet_residual ) );

      // Rescale RGB by newL/oldL to preserve colour ratios
      result.lhe = result.stretched;
      for ( int y = 0; y < wH; y++ )
         for ( int x = 0; x < wW; x++ )
         {
            float oldL = lum[y * wW + x];
            float newL = newLum[y * wW + x];
            if ( oldL > 1e-6f )
            {
               float scale = newL / oldL;
               for ( int c = 0; c < 3; c++ )
                  result.lhe( x, y, c ) = std::min( 1.0f,
                     std::max( 0.0f, result.stretched( x, y, c ) * scale ) );
            }
            else
            {
               // Near-black: set to new luminance as gray
               for ( int c = 0; c < 3; c++ )
                  result.lhe( x, y, c ) = newL;
            }
         }
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
   // Step 5: Star reduction — detect stars by local peak brightness
   // and reduce their intensity by ~15%. Uses morphological approach:
   // if a pixel is much brighter than its surroundings, it's a star.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Star reduction...\n");
      const float starDimFactor = float( g_params.star_dim );
      const int starR = g_params.star_radius;

      // Compute luminance
      std::vector<float> finLum( finW * finH );
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
            finLum[y * finW + x] = (result.final_( x, y, 0 ) + result.final_( x, y, 1 ) + result.final_( x, y, 2 )) / 3.0f;

      // Build Gaussian kernel for neighbourhood (sigma = starR/2)
      const int kSize = 2 * starR + 1;
      const float starSigma = float( starR ) / 2.0f;
      std::vector<float> gk( kSize * kSize );
      float gkSum = 0;
      for ( int ky = -starR; ky <= starR; ky++ )
         for ( int kx = -starR; kx <= starR; kx++ )
         {
            // Exclude center pixel from the neighbourhood estimate
            float w = (kx == 0 && ky == 0) ? 0.0f
                    : std::exp( -(kx*kx + ky*ky) / (2.0f * starSigma * starSigma) );
            gk[(ky+starR) * kSize + (kx+starR)] = w;
            gkSum += w;
         }
      for ( auto& w : gk ) w /= gkSum;

      // Detect stars: center vs Gaussian-weighted neighbourhood
      std::vector<float> starMask( finW * finH, 0.0f );
      for ( int y = starR; y < finH - starR; y++ )
         for ( int x = starR; x < finW - starR; x++ )
         {
            float centerL = finLum[y * finW + x];

            // Gaussian-weighted local mean (excluding center)
            float localMean = 0;
            for ( int ky = -starR; ky <= starR; ky++ )
               for ( int kx = -starR; kx <= starR; kx++ )
                  localMean += gk[(ky+starR) * kSize + (kx+starR)]
                             * finLum[(y+ky) * finW + (x+kx)];

            // Collect neighbourhood values for MAD estimate
            float nbVals[49]; // max for r=3: 7x7=49
            int nCount = 0;
            for ( int ky = -starR; ky <= starR; ky++ )
               for ( int kx = -starR; kx <= starR; kx++ )
                  if ( !(kx == 0 && ky == 0) )
                     nbVals[nCount++] = finLum[(y+ky) * finW + (x+kx)];

            // Median of neighbourhood
            std::sort( nbVals, nbVals + nCount );
            float nbMedian = nbVals[nCount / 2];

            // MAD of neighbourhood
            float nbDevs[49];
            for ( int i = 0; i < nCount; i++ )
               nbDevs[i] = std::abs( nbVals[i] - nbMedian );
            std::sort( nbDevs, nbDevs + nCount );
            float localMAD = nbDevs[nCount / 2];
            if ( localMAD < 1e-6f ) localMAD = 1e-6f;

            // Star criterion: excess over local mean, relative to local MAD
            float excess = centerL - localMean;
            float threshold = float( g_params.star_excess ) * localMAD;
            if ( excess > threshold && centerL > float( g_params.star_min_bright ) )
               starMask[y * finW + x] = std::min( 1.0f, (excess - threshold) / (4.0f * threshold) );
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

      // Apply star dimming with sqrt falloff (gentler edges)
      int starCount = 0;
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
         {
            float sm = std::pow( dilated[y * finW + x], 0.5f );
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
   // Step 6: Black point subtraction.
   // Clips the pedestal at median - 2.5*MAD per channel, then rescales.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Black point subtraction...\n");
      for ( int c = 0; c < 3; c++ )
      {
         ChannelStats cs = compute_channel_stats( result.final_, c );
         float black = float( cs.median - g_params.black_mad * cs.mad );
         black = std::min( black, 0.95f );
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
   // Step 7: Luminance-adaptive gamma. Bright structures get stronger
   // gamma (0.5) via smoothstep; background stays at gamma=1.0.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Adaptive gamma...\n");
      const float gammaMin = float( g_params.gamma );

      // Compute perceptual luminance and stats for thresholds
      std::vector<float> gammaLum( finW * finH );
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
            gammaLum[y * finW + x] = 0.2126f * result.final_( x, y, 0 )
                                   + 0.7152f * result.final_( x, y, 1 )
                                   + 0.0722f * result.final_( x, y, 2 );

      auto [medGL, madGL] = sampled_median_mad( gammaLum );

      float stretchLo = medGL + float( g_params.gamma_lo_mad ) * madGL;
      float stretchHi = medGL + float( g_params.gamma_hi_mad ) * madGL;
      fprintf(stdout, "    gamma range [%.2f, %.2f]  stretchLo=%.4f stretchHi=%.4f\n",
              gammaMin, 1.0f, stretchLo, stretchHi);

      for ( int c = 0; c < 3; c++ )
         for ( int y = 0; y < finH; y++ )
            for ( int x = 0; x < finW; x++ )
            {
               float L = gammaLum[y * finW + x];
               float t = (stretchHi > stretchLo)
                       ? std::min( 1.0f, std::max( 0.0f, (L - stretchLo) / (stretchHi - stretchLo) ) )
                       : 0.0f;
               t = t * t * (3.0f - 2.0f * t); // smoothstep
               float gamma = 1.0f - t * (1.0f - gammaMin);
               float v = result.final_( x, y, c );
               result.final_( x, y, c ) = std::pow( v, gamma );
            }
   }

   // ----------------------------------------------------------------
   // Step 8: Light background noise suppression (Gaussian σ=0.7)
   // Only applied where luminance is below the background/signal
   // transition. Leaves galaxy arms untouched.
   // Applied after gamma so the brightness scale is final.
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Post-gamma noise suppression...\n");
      const float nsSigma = float( g_params.noise_sigma );
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

      // Background threshold: median + N*MAD
      auto [medL, madL] = sampled_median_mad( finLum );
      float nsLo = medL + float( g_params.noise_mask_lo_mad ) * madL;
      float nsHi = medL + float( g_params.noise_mask_hi_mad ) * madL;

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
   // Step 9: Smoothstep saturation boost. Smoothly ramps sat from
   // 1.0 to sat_boost based on luminance (satLo..satHi).
   // ----------------------------------------------------------------
   {
      fprintf(stdout, "    Smoothstep saturation boost...\n");

      // Compute luminance stats for thresholds
      std::vector<float> postLum( finW * finH );
      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
            postLum[y * finW + x] = 0.2126f * result.final_( x, y, 0 )
                                  + 0.7152f * result.final_( x, y, 1 )
                                  + 0.0722f * result.final_( x, y, 2 );
      auto [medLum, madLum] = sampled_median_mad( postLum );

      float satLo = medLum + float( g_params.sat_lo_mad ) * madLum;
      float satHi = medLum + float( g_params.sat_hi_mad ) * madLum;
      const float satBoostMax = float( g_params.sat_boost );
      fprintf(stdout, "    satLo=%.4f satHi=%.4f boost=%.2f\n", satLo, satHi, satBoostMax);

      for ( int y = 0; y < finH; y++ )
         for ( int x = 0; x < finW; x++ )
         {
            float L = postLum[y * finW + x];
            float t = (satHi > satLo)
                    ? std::min( 1.0f, std::max( 0.0f, (L - satLo) / (satHi - satLo) ) )
                    : 0.0f;
            t = t * t * (3.0f - 2.0f * t); // smoothstep
            float sat = 1.0f + t * (satBoostMax - 1.0f);

            if ( sat > 1.0f )
            {
               float R = result.final_( x, y, 0 );
               float G = result.final_( x, y, 1 );
               float B = result.final_( x, y, 2 );

               R = L + sat * (R - L);
               G = L + sat * (G - L);
               B = L + sat * (B - L);

               result.final_( x, y, 0 ) = std::max( 0.0f, std::min( 1.0f, R ) );
               result.final_( x, y, 1 ) = std::max( 0.0f, std::min( 1.0f, G ) );
               result.final_( x, y, 2 ) = std::max( 0.0f, std::min( 1.0f, B ) );
            }
         }
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
   target.crop_margin = g_params.m51_crop_margin;

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
   target.crop_margin = g_params.m101_crop_margin;

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
   {
      if ( strcmp(argv[i], "--save-reference") == 0 )
         g_save_reference = true;
      else if ( strncmp(argv[i], "--", 2) == 0 && strchr(argv[i], '=') )
         parse_param( argv[i] );
      else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 )
      {
         fprintf(stdout, "Usage: %s [--save-reference] [--key=value ...]\n\n", argv[0]);
         print_params();
         return 0;
      }
   }

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
   print_params();
   fprintf(stdout, "--- Real Data Regression Tests ---\n\n");

   run_test("M51 (NGC 5194) pipeline",   test_m51);
   run_test("M101 (Pinwheel) pipeline",   test_m101);

   fprintf(stdout, "\n--- Results ---\n");
   fprintf(stdout, "  %d tests run, %d passed, %d failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
   fprintf(stdout, "\n%s\n", g_tests_failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

   delete pcl::Module;
   return g_tests_failed > 0 ? 1 : 0;
}
