// ============================================================================
// test_stretch.cpp - Histogram Transformation / Auto-Stretch differential test
//
// Tests the MTF formula, HistogramTransformation pixel mapping, and
// DisplayFunction auto-stretch computation against known values.
//
// Usage:
//   ./test_stretch              # Run all tests
// ============================================================================

#include <QApplication>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/Image.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/DisplayFunction.h>

#include "../PCLMockAPI.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

// ============================================================================
// Minimal MetaModule for test harness
// ============================================================================

namespace pcl
{

class TestModule : public MetaModule
{
public:
   TestModule() : MetaModule() {}

   const char* Version() const override        { return "1.0.0"; }
   IsoString Name() const override             { return "TestStretch"; }
   String Description() const override         { return "Stretch Test Module"; }
   String Company() const override             { return "Test"; }
   String Author() const override              { return "Test"; }
   String Copyright() const override           { return "Test"; }
   String TradeMarks() const override          { return String(); }
   String OriginalFileName() const override    { return "test_stretch"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2025; m=1; d=1; }
};

} // pcl

// ============================================================================
// Test Infrastructure
// ============================================================================

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

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
// Reference MTF implementation for verification
// ============================================================================

static double reference_mtf(double m, double x)
{
   // MTF formula: (m-1)*x / ((2m-1)*x - m)
   // Fixed points: MTF(m, 0) = 0, MTF(m, m) = 0.5, MTF(m, 1) = 1
   if ( x <= 0 ) return 0;
   if ( x >= 1 ) return 1;
   double m1 = m - 1;
   return m1 * x / ((m + m1) * x - m);
}

// ============================================================================
// Test 1: MTF fixed points
// ============================================================================

static bool test_mtf_fixed_points()
{
   double m_values[] = { 0.1, 0.2, 0.25, 0.5, 0.75, 0.9 };

   fprintf(stdout, "\n");
   for ( double m : m_values )
   {
      double y0 = pcl::HistogramTransformation::MTF( m, 0.0 );
      double ym = pcl::HistogramTransformation::MTF( m, m );
      double y1 = pcl::HistogramTransformation::MTF( m, 1.0 );

      fprintf(stdout, "    MTF(m=%.2f): f(0)=%.6f  f(m)=%.6f  f(1)=%.6f\n",
              m, y0, ym, y1);

      TEST_ASSERT_NEAR(y0, 0.0, 1e-15, "MTF(m, 0) should be 0");
      TEST_ASSERT_NEAR(ym, 0.5, 1e-12, "MTF(m, m) should be 0.5");
      TEST_ASSERT_NEAR(y1, 1.0, 1e-15, "MTF(m, 1) should be 1");
   }

   return true;
}

// ============================================================================
// Test 2: MTF monotonicity and known values
// ============================================================================

static bool test_mtf_monotonicity()
{
   // MTF should be monotonically increasing for 0 < m < 1
   double m_values[] = { 0.1, 0.25, 0.5, 0.75, 0.9 };

   fprintf(stdout, "\n");
   for ( double m : m_values )
   {
      double prev = -1;
      bool monotonic = true;
      for ( int i = 0; i <= 100; i++ )
      {
         double x = i / 100.0;
         double y = pcl::HistogramTransformation::MTF( m, x );
         if ( y < prev - 1e-15 )
         {
            monotonic = false;
            fprintf(stderr, "    Non-monotonic at m=%.2f x=%.2f: y=%.10f < prev=%.10f\n",
                    m, x, y, prev);
         }
         prev = y;
      }

      fprintf(stdout, "    MTF(m=%.2f): monotonic=%s\n", m, monotonic ? "yes" : "NO");
      TEST_ASSERT(monotonic, "MTF should be monotonically increasing");
   }

   // Verify against reference implementation
   double maxDiff = 0;
   for ( double m : m_values )
      for ( int i = 0; i <= 1000; i++ )
      {
         double x = i / 1000.0;
         double pcl_val = pcl::HistogramTransformation::MTF( m, x );
         double ref_val = reference_mtf( m, x );
         double diff = std::abs(pcl_val - ref_val);
         if ( diff > maxDiff ) maxDiff = diff;
      }

   fprintf(stdout, "    Max diff vs reference: %.2e\n", maxDiff);
   TEST_ASSERT(maxDiff < 1e-14, "MTF should match reference implementation");

   return true;
}

// ============================================================================
// Test 3: MTF midtones balance effect
// ============================================================================

static bool test_mtf_balance_effect()
{
   // m < 0.5 should darken (curve below identity)
   // m > 0.5 should brighten (curve above identity)
   // m = 0.5 should be identity
   double x = 0.3;

   double y_dark    = pcl::HistogramTransformation::MTF( 0.2, x );
   double y_neutral = pcl::HistogramTransformation::MTF( 0.5, x );
   double y_bright  = pcl::HistogramTransformation::MTF( 0.8, x );

   fprintf(stdout, "\n    At x=%.1f:  m=0.2 → %.6f,  m=0.5 → %.6f,  m=0.8 → %.6f\n",
           x, y_dark, y_neutral, y_bright);

   TEST_ASSERT_NEAR(y_neutral, x, 1e-14, "m=0.5 should be identity");
   // m < 0.5 pushes values toward 0.5 (brightens dark midtones)
   // m > 0.5 pushes values toward 0.5 (darkens bright midtones)
   TEST_ASSERT(y_dark > x, "m<0.5 should brighten (MTF maps m→0.5, so low m lifts darks)");
   TEST_ASSERT(y_bright < x, "m>0.5 should darken (MTF maps m→0.5, so high m dims brights)");

   return true;
}

// ============================================================================
// Test 4: HistogramTransformation identity
// ============================================================================

static bool test_ht_identity()
{
   // Default HT should be identity: mb=0.5, sc=0, hc=1, lr=0, hr=1
   pcl::HistogramTransformation ht;

   double test_values[] = { 0.0, 0.1, 0.25, 0.5, 0.75, 0.9, 1.0 };

   fprintf(stdout, "\n");
   for ( double v : test_values )
   {
      double result = v;
      ht.Transform( result );
      fprintf(stdout, "    Identity: %.2f → %.6f\n", v, result);
      TEST_ASSERT_NEAR(result, v, 1e-14, "Identity HT should not change value");
   }

   return true;
}

// ============================================================================
// Test 5: HistogramTransformation clipping
// ============================================================================

static bool test_ht_clipping()
{
   // sc=0.2, hc=0.8, mb=0.5 (no MTF effect)
   // Maps [0.2, 0.8] → [0, 1] linearly (since mb=0.5 is identity)
   pcl::HistogramTransformation ht( 0.5/*mb*/, 0.2/*sc*/, 0.8/*hc*/ );

   struct { double in; double expected; } cases[] = {
      { 0.0,  0.0 },
      { 0.1,  0.0 },   // below shadow clip
      { 0.2,  0.0 },   // at shadow clip
      { 0.5,  0.5 },   // midpoint: (0.5-0.2)/(0.8-0.2) = 0.5
      { 0.8,  1.0 },   // at highlight clip
      { 0.9,  1.0 },   // above highlight clip
      { 1.0,  1.0 },
   };

   fprintf(stdout, "\n");
   for ( auto& tc : cases )
   {
      double result = tc.in;
      ht.Transform( result );
      fprintf(stdout, "    Clip [0.2,0.8]: %.2f → %.6f  (expected: %.6f)\n",
              tc.in, result, tc.expected);
      TEST_ASSERT_NEAR(result, tc.expected, 1e-12,
                        "Clipping transform mismatch");
   }

   return true;
}

// ============================================================================
// Test 6: HistogramTransformation with MTF
// ============================================================================

static bool test_ht_with_mtf()
{
   // sc=0, hc=1, mb=0.25 — should darken midtones
   double mb = 0.25;
   pcl::HistogramTransformation ht( mb, 0.0, 1.0 );

   // After clipping (identity since sc=0, hc=1), apply MTF
   double test_values[] = { 0.0, 0.1, 0.25, 0.5, 0.75, 1.0 };

   fprintf(stdout, "\n");
   for ( double v : test_values )
   {
      double result = v;
      ht.Transform( result );
      double expected = reference_mtf( mb, v );

      fprintf(stdout, "    MTF(mb=%.2f): %.2f → %.6f  (expected: %.6f)\n",
              mb, v, result, expected);
      TEST_ASSERT_NEAR(result, expected, 1e-12,
                        "HT with MTF should match reference");
   }

   return true;
}

// ============================================================================
// Test 7: Full HistogramTransformation pipeline
// ============================================================================

static bool test_ht_full_pipeline()
{
   // sc=0.1, hc=0.9, mb=0.3 — clipping + darkening MTF
   double sc = 0.1, hc = 0.9, mb = 0.3;
   pcl::HistogramTransformation ht( mb, sc, hc );

   fprintf(stdout, "\n    Pipeline: sc=%.1f hc=%.1f mb=%.1f\n", sc, hc, mb);

   double test_values[] = { 0.0, 0.05, 0.1, 0.3, 0.5, 0.7, 0.9, 0.95, 1.0 };

   for ( double v : test_values )
   {
      // Manual reference: clip, then MTF
      double expected = v;
      // Stage 1: Clipping
      if ( expected <= sc )
         expected = 0.0;
      else if ( expected >= hc )
         expected = 1.0;
      else
         expected = (expected - sc) / (hc - sc);
      // Stage 2: MTF
      expected = reference_mtf( mb, expected );

      double result = v;
      ht.Transform( result );

      fprintf(stdout, "    %.2f → %.6f  (expected: %.6f  diff: %.2e)\n",
              v, result, expected, std::abs(result - expected));
      TEST_ASSERT_NEAR(result, expected, 1e-12,
                        "Full pipeline transform mismatch");
   }

   return true;
}

// ============================================================================
// Test 8: DisplayFunction auto-stretch (single channel, non-inverted)
// ============================================================================

static bool test_autostretch_single_channel()
{
   // Typical astronomical image: low background, faint signal
   // median ≈ 0.02, MAD ≈ 0.005 (normalized MAD*1.4826 for sigma)
   double median = 0.02;
   double sigma = 0.005 * 1.4826;  // MAD to sigma

   pcl::DisplayFunction df;
   // Defaults: clip=-2.80, tbkg=0.25, linked=false

   pcl::DVector sigmaVec( 1 );
   pcl::DVector centerVec( 1 );
   sigmaVec[0] = sigma;
   centerVec[0] = median;

   df.ComputeAutoStretch( sigmaVec, centerVec );

   pcl::HistogramTransformation ht = df[0];

   // For non-inverted (center < 0.5):
   //   shadows = Range(center + clip*sigma, 0, 1) = Range(0.02 + (-2.80)*0.007413, 0, 1)
   //           = Range(0.02 - 0.020756, 0, 1) = Range(-0.000756, 0, 1) = 0.0
   //   midtones = MTF(targetBackground=0.25, center - shadows) = MTF(0.25, 0.02 - 0.0) = MTF(0.25, 0.02)
   //   highlights = 1.0
   double expectedShadows = pcl::Range( median + (-2.80) * sigma, 0.0, 1.0 );
   double expectedMidtones = reference_mtf( 0.25, median - expectedShadows );

   fprintf(stdout, "\n    Input: median=%.4f  sigma=%.6f\n", median, sigma);
   fprintf(stdout, "    Expected: shadows=%.6f  midtones=%.6f  highlights=1.0\n",
           expectedShadows, expectedMidtones);
   fprintf(stdout, "    Got:      shadows=%.6f  midtones=%.6f  highlights=%.6f\n",
           ht.ShadowsClipping(), ht.MidtonesBalance(), ht.HighlightsClipping());

   TEST_ASSERT_NEAR(ht.ShadowsClipping(), expectedShadows, 1e-10,
                     "Auto-stretch shadows clipping");
   TEST_ASSERT_NEAR(ht.MidtonesBalance(), expectedMidtones, 1e-10,
                     "Auto-stretch midtones balance");
   TEST_ASSERT_NEAR(ht.HighlightsClipping(), 1.0, 1e-10,
                     "Auto-stretch highlights clipping");

   // Verify the transformation maps median to approximately targetBackground
   double medianAfter = median;
   ht.Transform( medianAfter );
   fprintf(stdout, "    Median after stretch: %.6f  (target: 0.25)\n", medianAfter);
   TEST_ASSERT_NEAR(medianAfter, 0.25, 0.05,
                     "Stretched median should be near target background");

   return true;
}

// ============================================================================
// Test 9: DisplayFunction auto-stretch on a real image
// ============================================================================

static bool test_autostretch_on_image()
{
   // Create a 256x256 image simulating a dark astronomical frame
   // Background: ~0.02 with Gaussian-like noise pattern
   const int W = 256, H = 256;
   pcl::DImage image( W, H );

   // Fill with a smooth gradient to simulate sky background + a "star"
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         // Low background with slight gradient
         double bg = 0.015 + 0.00002 * x + 0.00001 * y;
         // Add a synthetic "star" near center as a Gaussian
         double dx = x - 128.0, dy = y - 128.0;
         double star = 0.8 * exp(-(dx*dx + dy*dy) / (2*5*5));
         image( x, y ) = bg + star;
      }

   // Compute statistics
   double median = image.Median();
   double mad = image.MAD( median );
   double sigma = mad * 1.4826;

   fprintf(stdout, "\n    Image stats: median=%.6f  MAD=%.6f  sigma=%.6f\n",
           median, mad, sigma);

   // Compute auto-stretch
   pcl::DisplayFunction df;
   pcl::DVector sigmaVec( 1 ), centerVec( 1 );
   sigmaVec[0] = sigma;
   centerVec[0] = median;
   df.ComputeAutoStretch( sigmaVec, centerVec );

   pcl::HistogramTransformation ht = df[0];

   fprintf(stdout, "    STF: shadows=%.6f  midtones=%.6f  highlights=%.6f\n",
           ht.ShadowsClipping(), ht.MidtonesBalance(), ht.HighlightsClipping());

   // Verify basic sanity
   TEST_ASSERT(ht.ShadowsClipping() >= 0 && ht.ShadowsClipping() < 0.1,
               "Shadows clipping should be in [0, 0.1) for dark image");
   TEST_ASSERT(ht.MidtonesBalance() > 0 && ht.MidtonesBalance() < 0.5,
               "Midtones should be < 0.5 (boosting dark background)");
   TEST_ASSERT_NEAR(ht.HighlightsClipping(), 1.0, 1e-10,
                     "Highlights should be 1.0 for non-inverted");

   // Apply stretch and verify the background is lifted near target
   double bgAfter = median;
   ht.Transform( bgAfter );
   fprintf(stdout, "    Background after stretch: %.6f  (target: ~0.25)\n", bgAfter);
   TEST_ASSERT(bgAfter > 0.1 && bgAfter < 0.5,
               "Stretched background should be in visible range [0.1, 0.5]");

   // Star peak should be close to 1.0 after stretch
   double starPeak = image( 128, 128 );
   ht.Transform( starPeak );
   fprintf(stdout, "    Star peak after stretch: %.6f\n", starPeak);
   TEST_ASSERT(starPeak > 0.9, "Star should be bright after stretch");

   return true;
}

// ============================================================================
// Test 10: Apply HT to array of pixel values
// ============================================================================

static bool test_ht_apply_array()
{
   pcl::HistogramTransformation ht( 0.3/*mb*/, 0.05/*sc*/, 0.95/*hc*/ );

   const int N = 1000;
   double pixels[N];
   for ( int i = 0; i < N; i++ )
      pixels[i] = double(i) / (N - 1);

   // Apply using array method
   ht.Apply( pixels, N, 0.0, 1.0 );

   // Verify against Transform() applied individually
   double maxDiff = 0;
   for ( int i = 0; i < N; i++ )
   {
      double v = double(i) / (N - 1);
      ht.Transform( v );
      double diff = std::abs(pixels[i] - v);
      if ( diff > maxDiff ) maxDiff = diff;
   }

   fprintf(stdout, "\n    Array Apply vs Transform: max diff = %.2e\n", maxDiff);
   TEST_ASSERT(maxDiff < 1e-12, "Array Apply should match individual Transform");

   // Verify output is monotonically increasing
   for ( int i = 1; i < N; i++ )
   {
      TEST_ASSERT(pixels[i] >= pixels[i-1] - 1e-15,
                  "Output should be monotonically increasing");
   }

   // Verify boundaries
   TEST_ASSERT_NEAR(pixels[0], 0.0, 1e-14, "Input 0 should map to 0");
   TEST_ASSERT_NEAR(pixels[N-1], 1.0, 1e-14, "Input 1 should map to 1");

   return true;
}

// ============================================================================
// Main
// ============================================================================

pcl::MetaModule* pcl::Module = nullptr;

int main(int argc, char** argv)
{
   QApplication app(argc, argv);

   fprintf(stdout, "===========================================\n");
   fprintf(stdout, "PCL Mock Stretch Test Suite\n");
   fprintf(stdout, "===========================================\n\n");

   // Initialize PCL with mock API
   fprintf(stdout, "Initializing mock API...\n");
   pcl::Module = new pcl::TestModule();

   InitializePixInsightModule(
      pcl::Module,
      GetMockFunctionResolver(),
      PCL_API_Version,
      nullptr
   );
   fprintf(stdout, "Mock API initialized.\n\n");

   // Run tests
   fprintf(stdout, "--- Stretch / Histogram Transformation Tests ---\n\n");

   run_test("MTF fixed points",                  test_mtf_fixed_points);
   run_test("MTF monotonicity & reference",      test_mtf_monotonicity);
   run_test("MTF balance effect",                test_mtf_balance_effect);
   run_test("HT identity",                       test_ht_identity);
   run_test("HT clipping",                       test_ht_clipping);
   run_test("HT with MTF",                       test_ht_with_mtf);
   run_test("HT full pipeline",                  test_ht_full_pipeline);
   run_test("Auto-stretch (single channel)",     test_autostretch_single_channel);
   run_test("Auto-stretch on image",             test_autostretch_on_image);
   run_test("HT Apply array",                    test_ht_apply_array);

   fprintf(stdout, "\n--- Results ---\n");
   fprintf(stdout, "  %d tests run, %d passed, %d failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
   fprintf(stdout, "\n%s\n", g_tests_failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

   delete pcl::Module;
   return g_tests_failed > 0 ? 1 : 0;
}
