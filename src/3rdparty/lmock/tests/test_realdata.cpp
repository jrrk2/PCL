// ============================================================================
// test_realdata_veralux.cpp - Real astronomical data regression tests
//
// VeraLux HyperMetric Stretch edition.
// Pipeline: background extraction → VeraLux HyperMetric Stretch (ready_to_use)
//
// GHS stretch, Richardson-Lucy deconvolution, chrominance noise reduction,
// starlet wavelet, adaptive gamma, noise suppression, saturation boost and
// star reduction are all preserved below as commented-out reference code.
//
// Usage:
//   ./test_realdata_veralux                    # Run all tests
//   ./test_realdata_veralux --save-reference   # Save reference values
//
// VeraLux algorithm ported from VeraLux Suite v2.0.7 (PJSR) by Riccardo Paterniti
// Original Python: https://gitlab.com/RikyPate/vera-lux-siril-scripts
// ============================================================================

#include <QApplication>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/Image.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/DisplayFunction.h>
#include <pcl/XISF.h>
// FITS support removed — all inputs are now pre-calibrated XISF

#include "../PCLMockAPI.h"

#include "BackgroundExtractor.h"
#include "BackgroundExtractionInstance.h"
#include "BackgroundExtractionParameters.h"
#include "BackgroundExtractionProcess.h"

// LocalHistogramEqualization kept for potential future use
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
   IsoString Name() const override             { return "TestRealDataVeraLux"; }
   String Description() const override         { return "VeraLux Real Data Test Module"; }
   String Company() const override             { return "Test"; }
   String Author() const override              { return "Test"; }
   String Copyright() const override           { return "Test"; }
   String TradeMarks() const override          { return String(); }
   String OriginalFileName() const override    { return "test_realdata_veralux"; }
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

static const char* g_output_dir = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference";

// ============================================================================
// Pipeline parameters
// ============================================================================

struct PipelineParams
{
   // VeraLux HyperMetric Stretch
   // Sensor luminance weights — default Rec.709.
   // See SENSOR_PROFILES in veralux.js for per-sensor values.
   double vl_weights[3]     = { 0.2126, 0.7152, 0.0722 }; // Rec.709

   // logD: 0 = auto-solve to vl_target_bg; >0 = use directly (log10 scale)
   double vl_log_d          = 0.0;

   // Highlight protection. Larger = stronger shadow/midtone protection.
   // VeraLux JS: typically 5.0-10.0. Minimum clamped to 0.1 in transform.
   double vl_protect_b      = 5.0;

   // Convergence: how quickly pixels blend toward white at highlights.
   // k = Lstr^convergence; higher = faster whitening. Default 3.5.
   double vl_convergence    = 3.5;

   // Color grip [0,1]: 1.0 = full luminance-preserving (VeraLux colour fidelity),
   // 0.0 = naive per-channel stretch (more saturated, less physically accurate).
   double vl_color_grip     = 1.0;

   // Shadow convergence: if >0, grip fades in shadows (Lstr^shadowConv weighting).
   double vl_shadow_conv    = 0.0;

   // Target background level for ready_to_use adaptive scaling (0.08-0.15 typical).
   double vl_target_bg      = 0.12;

   // Use adaptive histogram anchor (slower but more accurate for complex gradients).
   bool   vl_adaptive       = false;

   // Add 0.5% pedestal to lift the absolute black (prevents hard clip at 0).
   bool   vl_add_pedestal   = true;

   // --- Reference: GHS parameters (used by commented-out GHS stretch) ---
   // double ghs_D             = 0.18;
   // double ghs_b             = 0.0;
   // double ghs_SP            = 0.0;
   // double ghs_LP            = 0.0;
   // double ghs_clip          = -2.80;

   // --- Reference: Richardson-Lucy deconvolution ---
   // int    deconv_iterations = 5;
   // int    deconv_radius     = 2;
   // double deconv_sigma      = 1.2;
   // double deconv_damp       = 0.2;

   // --- Reference: Chrominance noise reduction ---
   // double chroma_sigma      = 1.5;
   // int    chroma_radius     = 3;

   // Starlet wavelet
   int    wavelet_scales    = 5;
   double wavelet_gain0     = 0.0;
   double wavelet_gain1     = 0.4;
   double wavelet_gain2     = 0.9;
   double wavelet_gain3     = 1.1;
   double wavelet_gain4     = 0.5;
   double wavelet_residual  = 1.2;
   double wavelet_mask_lo_mad = 1.0;
   double wavelet_mask_hi_mad = 3.0;

   // --- Reference: Post-wavelet noise suppression ---
   // double noise_sigma       = 0.5;
   // double noise_mask_lo_mad = 1.5;
   // double noise_mask_hi_mad = 4.0;

   // --- Reference: Star reduction ---
   // double star_dim          = 0.9;
   // int    star_radius       = 3;
   // double star_excess       = 8.0;
   // double star_min_bright   = 0.35;

   // --- Reference: Black point ---
   // double black_mad         = 3.2;

   // --- Reference: Adaptive gamma ---
   // double gamma             = 0.8;
   // double gamma_lo_mad      = 1.0;
   // double gamma_hi_mad      = 5.0;

   // --- Reference: Saturation boost ---
   // double sat_boost         = 1.5;
   // double sat_lo_mad        = 1.0;
   // double sat_hi_mad        = 4.0;
};

static PipelineParams g_params;

static void parse_param( const char* arg )
{
   if ( arg[0] == '-' && arg[1] == '-' )
      arg += 2;
   else
      return;

   const char* eq = strchr( arg, '=' );
   if ( !eq ) return;

   std::string key( arg, eq - arg );
   double val = atof( eq + 1 );

   // VeraLux params
   if ( key == "vl.logD" )      g_params.vl_log_d = val;
   else if ( key == "vl.b" )         g_params.vl_protect_b = val;
   else if ( key == "vl.conv" )      g_params.vl_convergence = val;
   else if ( key == "vl.grip" )      g_params.vl_color_grip = val;
   else if ( key == "vl.shconv" )    g_params.vl_shadow_conv = val;
   else if ( key == "vl.target" )    g_params.vl_target_bg = val;
   else if ( key == "vl.adaptive" )  g_params.vl_adaptive = val > 0.5;
   else if ( key == "vl.wr" )        g_params.vl_weights[0] = val;
   else if ( key == "vl.wg" )        g_params.vl_weights[1] = val;
   else if ( key == "vl.wb" )        g_params.vl_weights[2] = val;
   else
      fprintf(stderr, "WARNING: unknown parameter --%s\n", key.c_str());
}

static void print_params()
{
   fprintf(stdout, "Parameters:\n");
   fprintf(stdout, "  VeraLux: logD=%.2f b=%.1f conv=%.1f grip=%.2f shconv=%.1f target=%.3f\n",
           g_params.vl_log_d, g_params.vl_protect_b, g_params.vl_convergence,
           g_params.vl_color_grip, g_params.vl_shadow_conv, g_params.vl_target_bg);
   fprintf(stdout, "  VeraLux: weights=[%.4f, %.4f, %.4f] adaptive=%s pedestal=%s\n",
           g_params.vl_weights[0], g_params.vl_weights[1], g_params.vl_weights[2],
           g_params.vl_adaptive ? "yes" : "no",
           g_params.vl_add_pedestal ? "yes" : "no");
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

// Percentile on a pre-sorted vector [0,100]
static float sorted_percentile( const std::vector<float>& sorted, float p )
{
   if ( sorted.empty() ) return 0.0f;
   float idx = (p / 100.0f) * float( sorted.size() - 1 );
   int lo = int( idx );
   int hi = std::min( lo + 1, int( sorted.size() ) - 1 );
   float w = idx - float( lo );
   return sorted[lo] * (1.0f - w) + sorted[hi] * w;
}

// ============================================================================
// Starlet (à trous) wavelet decomposition — luminance only, 1D separable
// ============================================================================

static void convolve_atrous_1d(
   const std::vector<float>& in,
   std::vector<float>& out,
   int W, int H, int scale )
{
   // B3 spline kernel [1,4,6,4,1]/16, separable, spacing = 2^scale
   static const float k[5] = { 1.0f/16, 4.0f/16, 6.0f/16, 4.0f/16, 1.0f/16 };
   int step = 1 << scale;

   std::vector<float> tmp( W * H );

   // Horizontal pass
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

   // Vertical pass
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

struct WaveletLayers1D
{
   std::vector<std::vector<float>> detail;
   std::vector<float> residual;
};

static WaveletLayers1D starlet_decompose_1d(
   const std::vector<float>& input,
   int W, int H, int nScales )
{
   WaveletLayers1D result;
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

static std::vector<float> starlet_reconstruct_1d(
   const WaveletLayers1D& w,
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
      float g = (s < int( gain.size() )) ? gain[s] : 1.0f;
      for ( int i = 0; i < N; i++ )
         out[i] += g * w.detail[s][i];
   }
   return out;
}

// ============================================================================
// Target/crop infrastructure
// ============================================================================

struct TargetInfo
{
   const char* name;
   const char* xisf_path;      // pre-calibrated XISF input (MGC + SPCC already applied)
   const char* ref_path;
};

static bool file_exists(const char* path)
{
   struct stat st;
   return stat(path, &st) == 0;
}

// ============================================================================
// I/O helpers
// ============================================================================

static bool read_xisf(const char* path, pcl::Image& image)
{
   try
   {
      pcl::XISFReader reader;
      reader.Open( pcl::String( path ) );
      reader.SelectImage( 0 );
      reader.ReadImage( image );
      reader.Close();
      return true;
   }
   catch ( const pcl::Exception& e )
   {
      fprintf(stderr, "    Error reading XISF: %s\n", pcl::IsoString( e.Message() ).c_str());
      return false;
   }
}

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

static bool write_png(const char* path, const pcl::Image& image)
{
   int W = image.Width(), H = image.Height();
   int C = std::min( 3, image.NumberOfChannels() );

   FILE* f = fopen(path, "wb");
   if ( !f ) return false;

   png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
   png_infop info = png_create_info_struct(png);
   if ( setjmp(png_jmpbuf(png)) ) { png_destroy_write_struct(&png, &info); fclose(f); return false; }

   png_init_io(png, f);
   png_set_IHDR(png, info, W, H, 16,
                C == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
   png_write_info(png, info);
   png_set_swap(png);

   std::vector<uint16_t> row( W * C );
   for ( int y = 0; y < H; y++ )
   {
      for ( int x = 0; x < W; x++ )
         for ( int c = 0; c < C; c++ )
         {
            float v = std::min( 1.0f, std::max( 0.0f, image( x, y, c ) ) );
            row[x * C + c] = uint16_t( v * 65535.0f + 0.5f );
         }
      png_write_row(png, reinterpret_cast<png_bytep>(row.data()));
   }
   png_write_end(png, nullptr);
   png_destroy_write_struct(&png, &info);
   fclose(f);
   return true;
}

// ============================================================================
// Image statistics
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

   int sampleStep = std::max(1, N / 100000);
   std::vector<float> samples;
   samples.reserve(N / sampleStep + 1);
   for ( int i = 0; i < N; i += sampleStep )
      samples.push_back( image( i % W, i / W, channel ) );

   std::sort(samples.begin(), samples.end());
   s.median = samples[samples.size() / 2];

   std::vector<float> deviations(samples.size());
   for ( size_t i = 0; i < samples.size(); i++ )
      deviations[i] = std::abs(samples[i] - float(s.median));
   std::sort(deviations.begin(), deviations.end());
   s.mad = deviations[deviations.size() / 2];

   return s;
}

// ============================================================================
// Reference save / load
// ============================================================================

struct ReferenceValues
{
   // Background model — only populated for raw FITS pipeline (not calibrated XISF)
   bool   has_bg_ref = false;
   double bg_coeff_a[3] = {};
   double bg_coeff_b[3] = {};
   double bg_coeff_c[3] = {};

   // VeraLux stretch diagnostics
   double vl_anchor  = 0;
   double vl_log_d   = 0;
   double stretched_median[3] = {};
};

static bool save_reference(const char* path, const ReferenceValues& ref)
{
   FILE* f = fopen(path, "w");
   if ( !f ) return false;
   fprintf(f, "# VeraLux Real Data Reference Values\n");
   if ( ref.has_bg_ref )
      for ( int c = 0; c < 3; c++ )
         fprintf(f, "bg_coeff %d %.15e %.15e %.15e\n", c,
                 ref.bg_coeff_a[c], ref.bg_coeff_b[c], ref.bg_coeff_c[c]);
   fprintf(f, "vl_anchor %.15e\n", ref.vl_anchor);
   fprintf(f, "vl_log_d %.15e\n",  ref.vl_log_d);
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
      int c; double v1, v2, v3;
      if ( sscanf(line, "bg_coeff %d %le %le %le", &c, &v1, &v2, &v3) == 4 && c >= 0 && c < 3 )
         { ref.bg_coeff_a[c]=v1; ref.bg_coeff_b[c]=v2; ref.bg_coeff_c[c]=v3; ref.has_bg_ref=true; continue; }
      if ( sscanf(line, "vl_anchor %le", &v1) == 1 )
         { ref.vl_anchor = v1; continue; }
      if ( sscanf(line, "vl_log_d %le", &v1) == 1 )
         { ref.vl_log_d = v1; continue; }
      if ( sscanf(line, "stretched_median %d %le", &c, &v1) == 2 && c >= 0 && c < 3 )
         { ref.stretched_median[c] = v1; continue; }
   }
   fclose(f);
   return true;
}

// ============================================================================
// VeraLux HyperMetric Stretch
//
// Ported from VeraLux Suite v2.0.7 (PJSR) by Riccardo Paterniti.
// Original Python: https://gitlab.com/RikyPate/vera-lux-siril-scripts
//
// Core transform (hyperbolicStretch):
//   f(v) = [ asinh(D·v + b) - asinh(b) ] / [ asinh(D + b) - asinh(b) ]
//   where D = 10^logD, b = protectB, symmetry point = 0.
//
// Colour preservation strategy:
//   L   = dot(weights, [R,G,B])           weighted luminance
//   Ls  = f(L)                            stretch luminance
//   k   = Ls^convergence                  blend factor: 0 at black, 1 at white
//   Rc  = Ls · (R/L·(1-k) + k)           colour-vector output
//
// At k=0 (dark pixels): channels scale proportionally to L → RGB ratios preserved.
// At k=1 (bright pixels): all channels → Ls → neutral white convergence.
// colorGrip blends between luminance-preserving and naive per-channel stretch.
// ============================================================================

// Core hyperbolic transform (Cranfield/Paterniti formulation)
static inline float vl_hyperbolic( float v, float D, float b )
{
   // D = 10^logD already expanded by caller
   float denom = std::asinh( D + b ) - std::asinh( b );
   if ( denom < 1e-9f ) return v;
   return ( std::asinh( D * v + b ) - std::asinh( b ) ) / denom;
}

// MTF (midtone transfer) used during adaptive scaling background targeting
static inline float vl_mtf( float v, float m )
{
   if ( v <= 0.0f ) return 0.0f;
   if ( v >= 1.0f ) return 1.0f;
   float num = (m - 1.0f) * v;
   float den = (2.0f * m - 1.0f) * v - m;
   if ( std::abs(den) < 1e-9f ) return v;
   return std::max( 0.0f, std::min( 1.0f, num / den ) );
}

// Soft clip rolloff above threshold (JS: applySoftClip with rolloff=2.0)
static inline float vl_soft_clip( float v, float threshold = 0.98f, float rolloff = 2.0f )
{
   if ( v <= threshold ) return v;
   float t = (v - threshold) / (1.0f - threshold);
   t = std::max( 0.0f, std::min( 1.0f, t ) );
   return threshold + (1.0f - threshold) * (1.0f - std::pow( 1.0f - t, rolloff ));
}

// Statistical anchor: 0.5th percentile of weighted luminance - small offset.
// Matches JS calculateAnchorStats for RGB.
static float vl_anchor_stats( const pcl::Image& img, const double weights[3] )
{
   int W = img.Width(), H = img.Height();
   int total = W * H;
   int step = std::max( 1, total / 500000 );

   std::vector<float> samples;
   samples.reserve( total / step + 1 );

   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      float lum = float(weights[0]) * img(x,y,0)
                + float(weights[1]) * img(x,y,1)
                + float(weights[2]) * img(x,y,2);
      samples.push_back( lum );
   }
   std::sort( samples.begin(), samples.end() );
   float p05 = sorted_percentile( samples, 0.5f );
   return std::max( 0.0f, p05 - 0.00025f );
}

// Adaptive anchor: 65536-bin weighted-luminance histogram, Gaussian-smoothed,
// find peak, walk back to 6% of peak value. Matches JS calculateAnchorAdaptive.
static float vl_anchor_adaptive( const pcl::Image& img, const double weights[3] )
{
   int W = img.Width(), H = img.Height();
   int total = W * H;
   int step = std::max( 1, total / 2000000 );

   const int BINS = 65536;
   std::vector<float> hist( BINS, 0.0f );

   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      float lum = float(weights[0]) * img(x,y,0)
                + float(weights[1]) * img(x,y,1)
                + float(weights[2]) * img(x,y,2);
      int bin = int( lum * (BINS - 1) );
      if ( bin >= 0 && bin < BINS )
         hist[bin] += 1.0f;
   }

   // Gaussian smooth with window ±50
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

   // Find peak (skip first 100 bins to avoid dark-frame DC)
   int peakIdx = 100; float peakVal = smoothed[100];
   for ( int k = 101; k < BINS; k++ )
      if ( smoothed[k] > peakVal ) { peakVal = smoothed[k]; peakIdx = k; }

   // Walk back from peak to where smoothed < 6% of peak
   float target = peakVal * 0.06f;
   int anchorIdx = 0;
   for ( int k = peakIdx; k >= 0; k-- )
      if ( smoothed[k] < target ) { anchorIdx = k; break; }

   float anchor = float(anchorIdx) / float(BINS - 1);
   if ( anchor <= 0.0f || std::isnan(anchor) )
   {
      // Fallback to statistical anchor
      return vl_anchor_stats( img, weights );
   }
   return std::max( 0.0f, anchor );
}

// Binary search for logD such that hyperbolicStretch(medianIn) ≈ targetMedian.
// Matches JS solveLogD (40 iterations, search range [0, 7]).
static double vl_solve_log_d( float medianIn, float targetMedian, float protectB )
{
   if ( medianIn < 1e-9f ) return 2.0;
   double lo = 0.0, hi = 7.0, best = 2.0;
   for ( int i = 0; i < 40; i++ )
   {
      double mid = (lo + hi) * 0.5;
      float D = float( std::pow( 10.0, mid ) );
      float test = vl_hyperbolic( medianIn, D, protectB );
      if ( std::abs(test - targetMedian) < 0.0001f ) { best = mid; break; }
      if ( test < targetMedian ) lo = mid; else hi = mid;
      best = mid;
   }
   return best;
}

// Adaptive scaling pass (ready_to_use mode).
// Sets floor at median - 2.7σ, maps to [pedestal=0.001, 0.98],
// then applies MTF to push median to targetBg.
// Matches JS applyAdaptiveScaling.
static void vl_adaptive_scaling( pcl::Image& img, const double weights[3], float targetBg )
{
   int W = img.Width(), H = img.Height();
   int total = W * H;
   int step = std::max( 1, total / 500000 );

   // Sample weighted luminance
   std::vector<float> luma;
   luma.reserve( total / step + 1 );
   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      luma.push_back( float(weights[0])*img(x,y,0)
                    + float(weights[1])*img(x,y,1)
                    + float(weights[2])*img(x,y,2) );
   }

   float mean = 0;
   for ( float v : luma ) mean += v;
   mean /= float( luma.size() );
   float var = 0;
   for ( float v : luma ) var += (v - mean) * (v - mean);
   float stdDev = std::sqrt( var / float( luma.size() ) );

   std::vector<float> sorted = luma;
   std::sort( sorted.begin(), sorted.end() );
   float med = sorted[sorted.size() / 2];
   float minVal = sorted.front();

   float floor_ = std::max( minVal, med - 2.7f * stdDev );
   float softCeil  = sorted_percentile( sorted, 99.0f );
   float hardCeil  = sorted_percentile( sorted, 99.99f );

   if ( softCeil <= floor_ ) softCeil = floor_ + 1e-6f;
   if ( hardCeil <= softCeil ) hardCeil = softCeil + 1e-6f;

   const float PEDESTAL = 0.001f;
   float scaleA = (0.98f - PEDESTAL) / (softCeil - floor_ + 1e-9f);
   float scaleB = (1.00f - PEDESTAL) / (hardCeil - floor_ + 1e-9f);
   float scale  = std::min( scaleA, scaleB );

   // Expand pixels
   for ( int c = 0; c < 3; c++ )
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
         {
            float v = std::max( 0.0f, std::min( 1.0f,
               (img(x,y,c) - floor_) * scale + PEDESTAL ) );
            img(x, y, c) = v;
         }

   // Re-sample luminance to compute current background for MTF shift
   luma.clear();
   for ( int i = 0; i < total; i += step )
   {
      int x = i % W, y = i / W;
      luma.push_back( float(weights[0])*img(x,y,0)
                    + float(weights[1])*img(x,y,1)
                    + float(weights[2])*img(x,y,2) );
   }
   std::sort( luma.begin(), luma.end() );
   float currentBg = luma[luma.size() / 2];

   if ( currentBg > 0 && currentBg < 1 && std::abs(currentBg - targetBg) > 0.001f )
   {
      // MTF midtone parameter: solve so that MTF(currentBg) = targetBg
      float m = (currentBg * (targetBg - 1.0f))
              / (currentBg * (2.0f * targetBg - 1.0f) - targetBg);
      for ( int c = 0; c < 3; c++ )
         for ( int y = 0; y < H; y++ )
            for ( int x = 0; x < W; x++ )
               img(x, y, c) = vl_mtf( img(x,y,c), m );
   }
}

// Main VeraLux HyperMetric Stretch.
// Input: linear, background-subtracted, colour-calibrated RGB image.
// Params: see vl_* fields in PipelineParams.
// Returns solved logD (for regression reference).
static double vl_stretch( pcl::Image& img, const PipelineParams& p )
{
   int W = img.Width(), H = img.Height();
   const double* weights = p.vl_weights;

   // --- Anchor ---
   float anchor;
   if ( p.vl_adaptive )
   {
      fprintf(stdout, "    VeraLux: computing adaptive histogram anchor...\n");
      anchor = vl_anchor_adaptive( img, weights );
   }
   else
   {
      anchor = vl_anchor_stats( img, weights );
   }
   fprintf(stdout, "    VeraLux: anchor = %.6f\n", anchor);

   // --- Solve logD if not specified ---
   double logD = p.vl_log_d;
   if ( logD <= 0.0 )
   {
      // Sample weighted luminance after anchor subtraction to find median
      int total = W * H;
      int step = std::max( 1, total / 500000 );
      std::vector<float> lumaSamples;
      lumaSamples.reserve( total / step + 1 );
      for ( int i = 0; i < total; i += step )
      {
         int x = i % W, y = i / W;
         float r = std::max( 0.0f, img(x,y,0) - anchor );
         float g = std::max( 0.0f, img(x,y,1) - anchor );
         float b = std::max( 0.0f, img(x,y,2) - anchor );
         lumaSamples.push_back( float(weights[0])*r + float(weights[1])*g + float(weights[2])*b );
      }
      std::sort( lumaSamples.begin(), lumaSamples.end() );
      float medLuma = lumaSamples[lumaSamples.size() / 2];

      logD = vl_solve_log_d( medLuma, float(p.vl_target_bg), float(p.vl_protect_b) );
      fprintf(stdout, "    VeraLux: auto-solved logD = %.4f (median %.6f → target %.3f)\n",
              logD, medLuma, p.vl_target_bg);
   }
   else
   {
      fprintf(stdout, "    VeraLux: using fixed logD = %.4f\n", logD);
   }

   const float D        = float( std::pow( 10.0, logD ) );
   const float b        = float( std::max( 0.1, p.vl_protect_b ) );
   const float conv     = float( p.vl_convergence );
   const float grip     = float( p.vl_color_grip );
   const float shConv   = float( p.vl_shadow_conv );
   const float pedestal = 0.005f;
   const bool  addPed   = p.vl_add_pedestal;
   const float eps      = 1e-9f;

   // --- Main pixel loop ---
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         float r = std::max( 0.0f, img(x,y,0) - anchor );
         float g = std::max( 0.0f, img(x,y,1) - anchor );
         float bv = std::max( 0.0f, img(x,y,2) - anchor );

         float L    = float(weights[0])*r + float(weights[1])*g + float(weights[2])*bv;
         float Lsafe = L + eps;

         // Stretch luminance
         float Ls = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic(L, D, b) ) );

         // Convergence blend factor: 0 at black → preserves ratios; 1 at white → neutral
         float k  = std::pow( Ls, conv );

         // Colour-preserving output (vector logic)
         float rF = Ls * ( (r  / Lsafe) * (1.0f - k) + k );
         float gF = Ls * ( (g  / Lsafe) * (1.0f - k) + k );
         float bF = Ls * ( (bv / Lsafe) * (1.0f - k) + k );

         // Hybrid logic: blend with naive per-channel stretch if grip < 1 or shadow fade
         if ( grip < 1.0f || shConv > 0.01f )
         {
            float rS = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic(r,  D, b) ) );
            float gS = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic(g,  D, b) ) );
            float bS = std::max( 0.0f, std::min( 1.0f, vl_hyperbolic(bv, D, b) ) );

            float cg = grip;
            if ( shConv > 0.01f ) cg *= std::pow( Ls, shConv );
            float omg = 1.0f - cg;

            rF = rF * cg + rS * omg;
            gF = gF * cg + gS * omg;
            bF = bF * cg + bS * omg;
         }

         if ( addPed )
         {
            rF = rF * (1.0f - pedestal) + pedestal;
            gF = gF * (1.0f - pedestal) + pedestal;
            bF = bF * (1.0f - pedestal) + pedestal;
         }

         img(x,y,0) = std::max( 0.0f, std::min( 1.0f, rF ) );
         img(x,y,1) = std::max( 0.0f, std::min( 1.0f, gF ) );
         img(x,y,2) = std::max( 0.0f, std::min( 1.0f, bF ) );
      }

   // --- Ready-to-use post-processing ---
   fprintf(stdout, "    VeraLux: adaptive scaling (target bg=%.3f)...\n", p.vl_target_bg);
   vl_adaptive_scaling( img, weights, float(p.vl_target_bg) );

   fprintf(stdout, "    VeraLux: soft clip polish...\n");
   for ( int c = 0; c < 3; c++ )
      for ( int y = 0; y < H; y++ )
         for ( int x = 0; x < W; x++ )
            img(x,y,c) = vl_soft_clip( img(x,y,c) );

   return logD;
}

// ============================================================================
// Pipeline result
// ============================================================================

struct PipelineResult
{
   ReferenceValues ref;
   pcl::Image background;
   pcl::Image subtracted;
   pcl::Image stretched;  // VeraLux output (= final image in this pipeline)
   pcl::Image lhe;        // unused; alias of stretched for output convenience
   pcl::Image final_;     // unused; alias of stretched for output convenience
   ChannelStats input_stats[3];
   ChannelStats output_stats[3];
   bool ok;
};

// ============================================================================
// Core pipeline: background extraction + VeraLux stretch
// ============================================================================

static PipelineResult run_pipeline(const TargetInfo& target)
{
   PipelineResult result;
   result.ok = false;

   pcl::Image image;

   fprintf(stdout, "\n    Reading calibrated XISF: %s ...\n", target.xisf_path);
   if ( !read_xisf(target.xisf_path, image) )
      return result;

   {
      int W = image.Width(), H = image.Height();
      int C = image.NumberOfChannels();
      fprintf(stdout, "    Image: %dx%d, %d channels (pre-calibrated)\n", W, H, C);
      if ( C < 3 ) { fprintf(stderr, "    Error: expected RGB image\n"); return result; }

      for ( int c = 0; c < 3; c++ )
      {
         result.input_stats[c] = compute_channel_stats(image, c);
         fprintf(stdout, "    Input ch%d: median=%.6f MAD=%.6f min=%.6f max=%.6f\n",
                 c, result.input_stats[c].median, result.input_stats[c].mad,
                 result.input_stats[c].min, result.input_stats[c].max);
      }

      result.background = pcl::Image( W, H, pcl::ColorSpace::RGB ); // zero
      result.subtracted = image;
      result.ref.has_bg_ref = false;
   }

   // ==========================================================================
   // VeraLux HyperMetric Stretch
   // ==========================================================================
   fprintf(stdout, "    Applying VeraLux HyperMetric Stretch...\n");
   result.stretched = result.subtracted;
   double solvedLogD = vl_stretch( result.stretched, g_params );

   result.ref.vl_anchor = g_params.vl_adaptive
                        ? double( vl_anchor_adaptive( result.subtracted, g_params.vl_weights ) )
                        : double( vl_anchor_stats(    result.subtracted, g_params.vl_weights ) );
   result.ref.vl_log_d  = solvedLogD;

   for ( int c = 0; c < 3; c++ )
   {
      result.output_stats[c] = compute_channel_stats( result.stretched, c );
      result.ref.stretched_median[c] = result.output_stats[c].median;
      fprintf(stdout, "    Stretched ch%d: median=%.6f min=%.6f max=%.6f\n",
              c, result.output_stats[c].median,
              result.output_stats[c].min, result.output_stats[c].max);
   }

   // ===================================================================
   // Starlet wavelet decomposition — luminance-only detail enhancement
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
      // Compute luminance stats for mask thresholds
      std::vector<float> sortedLum = lum;
      std::sort( sortedLum.begin(), sortedLum.end() );
      float wMed = sorted_percentile( sortedLum, 50.0f );
      // MAD
      std::vector<float> devs( wW * wH );
      for ( int i = 0; i < wW * wH; i++ )
         devs[i] = std::abs( lum[i] - wMed );
      std::sort( devs.begin(), devs.end() );
      float wMAD = sorted_percentile( devs, 50.0f );

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

               // Star suppression: detect sharp peaks via 3x3 local mean
               float localMean = 0;
               int cnt = 0;
               for ( int ky2 = -1; ky2 <= 1; ky2++ )
                  for ( int kx2 = -1; kx2 <= 1; kx2++ )
                  {
                     int yy2 = std::min( std::max( y + ky2, 0 ), wH - 1 );
                     int xx2 = std::min( std::max( x + kx2, 0 ), wW - 1 );
                     localMean += lum[yy2 * wW + xx2];
                     cnt++;
                  }
               localMean /= cnt;
               float peakness = L - localMean;
               float starSuppress = std::exp( -peakness * 20.0f );

               float mask = ts * starSuppress;
               w.detail[s][y * wW + x] *= mask;
            }
         fprintf(stdout, "    scale %d: gain=%.2f (masked)\n", s, gain[s]);
      }

      std::vector<float> newLum = starlet_reconstruct_1d( w, gain,
         float( g_params.wavelet_residual ) );

      // Rescale RGB by pow(newL/oldL, 0.6) to preserve colour ratios
      result.lhe = result.stretched;
      for ( int y = 0; y < wH; y++ )
         for ( int x = 0; x < wW; x++ )
         {
            float oldL = lum[y * wW + x];
            float newL = newLum[y * wW + x];
            if ( oldL > 1e-6f )
            {
               float scale = std::pow( newL / oldL, 0.6f );
               for ( int c = 0; c < 3; c++ )
                  result.lhe( x, y, c ) = std::min( 1.0f,
                     std::max( 0.0f, result.stretched( x, y, c ) * scale ) );
            }
            else
            {
               for ( int c = 0; c < 3; c++ )
                  result.lhe( x, y, c ) = newL;
            }
         }
   }

   for ( int c = 0; c < 3; c++ )
   {
      ChannelStats cs = compute_channel_stats( result.lhe, c );
      fprintf(stdout, "    LHE ch%d: median=%.6f min=%.6f max=%.6f\n",
              c, cs.median, cs.min, cs.max);
   }

   result.final_ = result.lhe;

   result.ok = true;
   return result;
}

// ============================================================================
// Test: M51 (Whirlpool Galaxy)
// ============================================================================

static bool test_m51()
{
   TargetInfo target;
   target.name        = "M51";
   target.xisf_path   = "/Users/jonathan/Downloads/M51_NGC5194-RGB-session_1_crop3_cal.xisf";
   target.ref_path    = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference/m51_vl_reference.txt";

   if ( !file_exists(target.xisf_path) )
   {
      fprintf(stdout, "\n    SKIP: %s not found\n", target.xisf_path);
      return true;
   }

   PipelineResult result = run_pipeline(target);
   TEST_ASSERT(result.ok, "Pipeline failed for M51");

   char path[512];
   snprintf(path, sizeof(path), "%s/m51_vl_stretched.xisf", g_output_dir);
   write_xisf(path, result.stretched, "M51_vl_stretched");
   snprintf(path, sizeof(path), "%s/m51_vl_stretched.png", g_output_dir);
   write_png(path, result.stretched);
   fprintf(stdout, "    Output written to %s/\n", g_output_dir);

   if ( g_save_reference )
   {
      save_reference(target.ref_path, result.ref);
      fprintf(stdout, "    Reference saved to %s\n", target.ref_path);
      return true;
   }

   ReferenceValues ref = {};
   if ( load_reference(target.ref_path, ref) )
   {
      fprintf(stdout, "    Comparing against reference...\n");
      TEST_ASSERT_NEAR(result.ref.vl_anchor, ref.vl_anchor, 1e-5, "M51 vl_anchor");
      TEST_ASSERT_NEAR(result.ref.vl_log_d,  ref.vl_log_d,  1e-3, "M51 vl_log_d");
      for ( int c = 0; c < 3; c++ )
      {
         char msg[128];
         snprintf(msg, sizeof(msg), "M51 ch%d stretched_median", c);
         TEST_ASSERT_NEAR(result.ref.stretched_median[c], ref.stretched_median[c], 0.02, msg);
      }
      fprintf(stdout, "    All regression checks passed.\n");
   }
   else
   {
      fprintf(stdout, "    No reference file — run with --save-reference to create one.\n");
   }

   for ( int c = 0; c < 3; c++ )
   {
      char msg[128];
      snprintf(msg, sizeof(msg), "M51 ch%d median near target bg", c);
      TEST_ASSERT(result.output_stats[c].median > float(g_params.vl_target_bg) - 0.06f, msg);
      TEST_ASSERT(result.output_stats[c].median < float(g_params.vl_target_bg) + 0.10f, msg);
      snprintf(msg, sizeof(msg), "M51 ch%d max <= 1.0", c);
      TEST_ASSERT(result.output_stats[c].max <= 1.0 + 1e-6, msg);
      snprintf(msg, sizeof(msg), "M51 ch%d min >= 0", c);
      TEST_ASSERT(result.output_stats[c].min >= -1e-6, msg);
   }
   bool anyVisible = false;
   for ( int c = 0; c < 3; c++ )
      if ( result.output_stats[c].median > 0.05 ) anyVisible = true;
   TEST_ASSERT(anyVisible, "At least one channel should have visible signal");

   return true;
}

// ============================================================================
// Test: M101 (Pinwheel Galaxy)
// ============================================================================

static bool test_m101()
{
   TargetInfo target;
   target.name        = "M101";
   target.xisf_path   = "/Users/jonathan/Downloads/M101_stacked_cal.xisf";
   target.ref_path    = "/Users/jonathan/PCL/src/3rdparty/lmock/tests/reference/m101_vl_reference.txt";

   if ( !file_exists(target.xisf_path) )
   {
      fprintf(stdout, "\n    SKIP: %s not found\n", target.xisf_path);
      return true;
   }

   PipelineResult result = run_pipeline(target);
   TEST_ASSERT(result.ok, "Pipeline failed for M101");

   char path[512];
   snprintf(path, sizeof(path), "%s/m101_vl_stretched.xisf", g_output_dir);
   write_xisf(path, result.stretched, "M101_vl_stretched");
   snprintf(path, sizeof(path), "%s/m101_vl_stretched.png", g_output_dir);
   write_png(path, result.stretched);
   fprintf(stdout, "    Output written to %s/\n", g_output_dir);

   if ( g_save_reference )
   {
      save_reference(target.ref_path, result.ref);
      fprintf(stdout, "    Reference saved to %s\n", target.ref_path);
      return true;
   }

   ReferenceValues ref = {};
   if ( load_reference(target.ref_path, ref) )
   {
      fprintf(stdout, "    Comparing against reference...\n");
      if ( result.ref.has_bg_ref && ref.has_bg_ref )
      {
         for ( int c = 0; c < 3; c++ )
         {
            char msg[128];
            snprintf(msg, sizeof(msg), "M101 ch%d bg_coeff_a", c);
            TEST_ASSERT_NEAR(result.ref.bg_coeff_a[c], ref.bg_coeff_a[c], 1e-8, msg);
            snprintf(msg, sizeof(msg), "M101 ch%d bg_coeff_b", c);
            TEST_ASSERT_NEAR(result.ref.bg_coeff_b[c], ref.bg_coeff_b[c], 1e-8, msg);
            snprintf(msg, sizeof(msg), "M101 ch%d bg_coeff_c", c);
            TEST_ASSERT_NEAR(result.ref.bg_coeff_c[c], ref.bg_coeff_c[c], 1e-6, msg);
         }
      }
      TEST_ASSERT_NEAR(result.ref.vl_anchor, ref.vl_anchor, 1e-5, "M101 vl_anchor");
      TEST_ASSERT_NEAR(result.ref.vl_log_d,  ref.vl_log_d,  1e-3, "M101 vl_log_d");
      for ( int c = 0; c < 3; c++ )
      {
         char msg[128];
         snprintf(msg, sizeof(msg), "M101 ch%d stretched_median", c);
         TEST_ASSERT_NEAR(result.ref.stretched_median[c], ref.stretched_median[c], 0.02, msg);
      }
      fprintf(stdout, "    All regression checks passed.\n");
   }
   else
   {
      fprintf(stdout, "    No reference file — run with --save-reference to create one.\n");
   }

   for ( int c = 0; c < 3; c++ )
   {
      char msg[128];
      snprintf(msg, sizeof(msg), "M101 ch%d median near target", c);
      TEST_ASSERT(result.output_stats[c].median > float(g_params.vl_target_bg) - 0.06f, msg);
      TEST_ASSERT(result.output_stats[c].median < float(g_params.vl_target_bg) + 0.10f, msg);
      snprintf(msg, sizeof(msg), "M101 ch%d max <= 1.0", c);
      TEST_ASSERT(result.output_stats[c].max <= 1.0 + 1e-6, msg);
   }

   return true;
}


// ============================================================================
// Main
// ============================================================================

pcl::MetaModule* pcl::Module = nullptr;

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

   for ( int i = 1; i < argc; i++ )
   {
      if ( strcmp(argv[i], "--save-reference") == 0 )
         g_save_reference = true;
      else if ( strncmp(argv[i], "--", 2) == 0 && strchr(argv[i], '=') )
         parse_param( argv[i] );
      else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 )
      {
         fprintf(stdout, "Usage: %s [--save-reference] [--key=value ...]\n\n", argv[0]);
         fprintf(stdout, "VeraLux params: vl.logD vl.b vl.conv vl.grip vl.shconv vl.target\n");
         fprintf(stdout, "                vl.wr vl.wg vl.wb (sensor weights)\n");
         fprintf(stdout, "                vl.adaptive (0|1)\n\n");
         print_params();
         return 0;
      }
   }

   fprintf(stdout, "===========================================\n");
   fprintf(stdout, "PCL Mock Real Data Regression Suite\n");
   fprintf(stdout, "VeraLux HyperMetric Stretch Edition\n");
   fprintf(stdout, "===========================================\n\n");

   mkdir(g_output_dir, 0755);

   fprintf(stdout, "Initializing mock API...\n");
   pcl::Module = new pcl::TestModule();
   InitializePixInsightModule(
      pcl::Module, GetMockFunctionResolver(), PCL_API_Version, nullptr );

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

   // LHE process kept registered in case post-processing steps are re-enabled
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

   run_test("M51 (NGC 5194) VeraLux pipeline",  test_m51);
   run_test("M101 (Pinwheel) VeraLux pipeline", test_m101);

   fprintf(stdout, "\n--- Results ---\n");
   fprintf(stdout, "  %d tests run, %d passed, %d failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
   fprintf(stdout, "\n%s\n", g_tests_failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

   delete pcl::Module;
   return g_tests_failed > 0 ? 1 : 0;
}


