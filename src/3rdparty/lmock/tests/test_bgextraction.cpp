// ============================================================================
// test_bgextraction.cpp - Background Extraction differential test
//
// Tests the BackgroundExtractor's linear plane fitting algorithm directly.
// Creates synthetic images with known gradients, fits a background model,
// and verifies the result matches the expected plane.
//
// Usage:
//   ./test_bgextraction              # Run all tests
// ============================================================================

#include <QApplication>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/Image.h>

#include "../PCLMockAPI.h"

// BackgroundExtraction module headers
#include "BackgroundExtractor.h"
#include "BackgroundExtractionInstance.h"
#include "BackgroundExtractionParameters.h"
#include "BackgroundExtractionProcess.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
   IsoString Name() const override             { return "TestBGExtraction"; }
   String Description() const override         { return "Background Extraction Test Module"; }
   String Company() const override             { return "Test"; }
   String Author() const override              { return "Test"; }
   String Copyright() const override           { return "Test"; }
   String TradeMarks() const override          { return String(); }
   String OriginalFileName() const override    { return "test_bgextraction"; }
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
// Test 1: Constant image → flat background
// ============================================================================

static bool test_constant_background()
{
   const int W = 256, H = 256;
   const double bgValue = 0.03;  // Typical sky background level

   // Create a constant DImage
   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = bgValue;

   // Create instance with manual sample mode
   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   // Access private fields via friend — the test is compiled with the module headers
   // We need to set the instance parameters directly since BackgroundExtractor accesses them

   // Use BackgroundExtractor
   pcl::BackgroundExtractor extractor( image, instance );

   // Add manual samples at grid positions (low-value samples will pass validation)
   int step = 32;
   for ( int y = step; y < H - step; y += step )
      for ( int x = step; x < W - step; x += step )
         extractor.AddSample( pcl::BackgroundSample( x, y, bgValue ) );

   fprintf(stdout, "\n    Samples: %zu\n", extractor.SampleCount());

   // Fit background
   extractor.FitBackground();

   // Verify the background is flat
   const pcl::DImage& bg = extractor.Background();
   double minBg = 1e30, maxBg = -1e30, sumBg = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double v = bg( x, y );
         if ( v < minBg ) minBg = v;
         if ( v > maxBg ) maxBg = v;
         sumBg += v;
      }
   double meanBg = sumBg / (W * H);

   fprintf(stdout, "    Background: min=%.8f max=%.8f mean=%.8f\n", minBg, maxBg, meanBg);
   fprintf(stdout, "    Expected: %.8f\n", bgValue);

   // For a constant input, the plane fit should give coefficients a≈0, b≈0, c≈bgValue
   // The background image should be very close to bgValue everywhere
   TEST_ASSERT_NEAR(meanBg, bgValue, 1e-10, "Mean background should match input");
   TEST_ASSERT(maxBg - minBg < 1e-6, "Background should be flat for constant input");

   return true;
}

// ============================================================================
// Test 2: Linear gradient in X → correct plane
// ============================================================================

static bool test_linear_gradient_x()
{
   const int W = 256, H = 256;
   // Gradient: bg(x,y) = 0.01 + 0.0001 * x
   // At x=0: 0.01, at x=255: 0.0355
   const double a = 0.0001, b = 0.0, c = 0.01;

   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = a * x + b * y + c;

   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   pcl::BackgroundExtractor extractor( image, instance );

   // Add grid samples — values computed directly from the gradient formula
   int step = 16;
   for ( int y = step; y < H - step; y += step )
      for ( int x = step; x < W - step; x += step )
         extractor.AddSample( pcl::BackgroundSample( x, y, a * x + b * y + c ) );

   fprintf(stdout, "\n    Samples: %zu\n", extractor.SampleCount());

   extractor.FitBackground();
   const pcl::DImage& bg = extractor.Background();

   // Check a few specific pixels
   double maxErr = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double expected = a * x + b * y + c;
         double err = std::abs(bg( x, y ) - expected);
         if ( err > maxErr ) maxErr = err;
      }

   fprintf(stdout, "    Max pixel error: %.2e\n", maxErr);
   fprintf(stdout, "    Background at (0,0)=%.6f  (255,0)=%.6f  (0,255)=%.6f  (255,255)=%.6f\n",
           bg(0,0), bg(255,0), bg(0,255), bg(255,255));
   fprintf(stdout, "    Expected  at (0,0)=%.6f  (255,0)=%.6f  (0,255)=%.6f  (255,255)=%.6f\n",
           c, a*255+c, b*255+c, a*255+b*255+c);

   TEST_ASSERT(maxErr < 1e-8, "X-gradient reconstruction error too large");
   return true;
}

// ============================================================================
// Test 3: Linear gradient in Y
// ============================================================================

static bool test_linear_gradient_y()
{
   const int W = 256, H = 256;
   const double a = 0.0, b = 0.00015, c = 0.02;

   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = a * x + b * y + c;

   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   pcl::BackgroundExtractor extractor( image, instance );

   int step = 16;
   for ( int y = step; y < H - step; y += step )
      for ( int x = step; x < W - step; x += step )
         extractor.AddSample( pcl::BackgroundSample( x, y, a * x + b * y + c ) );

   fprintf(stdout, "\n    Samples: %zu\n", extractor.SampleCount());

   extractor.FitBackground();
   const pcl::DImage& bg = extractor.Background();

   double maxErr = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double expected = a * x + b * y + c;
         double err = std::abs(bg( x, y ) - expected);
         if ( err > maxErr ) maxErr = err;
      }

   fprintf(stdout, "    Max pixel error: %.2e\n", maxErr);
   TEST_ASSERT(maxErr < 1e-8, "Y-gradient reconstruction error too large");
   return true;
}

// ============================================================================
// Test 4: Diagonal gradient (both X and Y)
// ============================================================================

static bool test_linear_gradient_diagonal()
{
   const int W = 256, H = 256;
   const double a = 0.00008, b = 0.00012, c = 0.015;

   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = a * x + b * y + c;

   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   pcl::BackgroundExtractor extractor( image, instance );

   int step = 16;
   for ( int y = step; y < H - step; y += step )
      for ( int x = step; x < W - step; x += step )
         extractor.AddSample( pcl::BackgroundSample( x, y, a * x + b * y + c ) );

   fprintf(stdout, "\n    Samples: %zu\n", extractor.SampleCount());

   extractor.FitBackground();
   const pcl::DImage& bg = extractor.Background();

   double maxErr = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double expected = a * x + b * y + c;
         double err = std::abs(bg( x, y ) - expected);
         if ( err > maxErr ) maxErr = err;
      }

   fprintf(stdout, "    Max pixel error: %.2e\n", maxErr);
   fprintf(stdout, "    Gradient coefficients: a=%.5e  b=%.5e  c=%.5e\n", a, b, c);
   TEST_ASSERT(maxErr < 1e-8, "Diagonal gradient reconstruction error too large");
   return true;
}

// ============================================================================
// Test 5: Gradient with noisy samples → robust fitting
// ============================================================================

static bool test_gradient_with_noise()
{
   const int W = 256, H = 256;
   const double a = 0.0001, b = 0.00005, c = 0.02;

   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = a * x + b * y + c;

   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   pcl::BackgroundExtractor extractor( image, instance );

   // Add samples with small noise
   int step = 16;
   double noiseAmplitude = 1e-6;
   int sampleIdx = 0;
   for ( int y = step; y < H - step; y += step )
      for ( int x = step; x < W - step; x += step )
      {
         // Deterministic "noise" based on position
         double noise = noiseAmplitude * sin(sampleIdx * 1.618033988749895);
         double value = a * x + b * y + c + noise;
         extractor.AddSample( pcl::BackgroundSample( x, y, value ) );
         sampleIdx++;
      }

   fprintf(stdout, "\n    Samples: %zu  (noise amplitude: %.1e)\n",
           extractor.SampleCount(), noiseAmplitude);

   extractor.FitBackground();
   const pcl::DImage& bg = extractor.Background();

   // The fit won't be perfect due to noise, but should be close
   double maxErr = 0;
   double sumErr2 = 0;
   int N = W * H;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double expected = a * x + b * y + c;
         double err = std::abs(bg( x, y ) - expected);
         sumErr2 += err * err;
         if ( err > maxErr ) maxErr = err;
      }
   double rmsErr = sqrt(sumErr2 / N);

   fprintf(stdout, "    Max pixel error: %.2e\n", maxErr);
   fprintf(stdout, "    RMS pixel error: %.2e\n", rmsErr);

   // With 1e-6 noise, the fit should still be quite accurate
   TEST_ASSERT(maxErr < 1e-4, "Noisy gradient max error too large");
   TEST_ASSERT(rmsErr < 1e-5, "Noisy gradient RMS error too large");
   return true;
}

// ============================================================================
// Test 6: Subtraction — gradient removed, residual should be flat
// ============================================================================

static bool test_gradient_subtraction()
{
   const int W = 256, H = 256;
   const double a = 0.0001, b = 0.00008, c = 0.02;
   const double signalLevel = 0.005;  // Small constant signal on top of gradient

   // Create image: gradient + constant signal
   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = a * x + b * y + c + signalLevel;

   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   pcl::BackgroundExtractor extractor( image, instance );

   // Provide samples from the gradient only (as if we know the true background)
   int step = 16;
   for ( int y = step; y < H - step; y += step )
      for ( int x = step; x < W - step; x += step )
         extractor.AddSample( pcl::BackgroundSample( x, y, a * x + b * y + c ) );

   extractor.FitBackground();
   const pcl::DImage& bg = extractor.Background();

   // Subtract background and check residual
   double minResidual = 1e30, maxResidual = -1e30;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double residual = image( x, y ) - bg( x, y );
         if ( residual < minResidual ) minResidual = residual;
         if ( residual > maxResidual ) maxResidual = residual;
      }

   fprintf(stdout, "\n    Residual after subtraction: min=%.8f max=%.8f\n",
           minResidual, maxResidual);
   fprintf(stdout, "    Expected constant: %.8f\n", signalLevel);

   TEST_ASSERT_NEAR(minResidual, signalLevel, 1e-8,
                     "Residual min should equal signal level");
   TEST_ASSERT_NEAR(maxResidual, signalLevel, 1e-8,
                     "Residual max should equal signal level");
   TEST_ASSERT(maxResidual - minResidual < 1e-8,
               "Residual should be constant after gradient subtraction");
   return true;
}

// ============================================================================
// Test 7: Automatic sample generation on low-background image
// ============================================================================

static bool test_automatic_sampling()
{
   const int W = 512, H = 512;
   const double a = 0.00005, b = 0.00003, c = 0.02;

   // Create gradient image with values well below 0.1 (IsSampleValid threshold)
   pcl::DImage image( W, H );
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
         image( x, y ) = a * x + b * y + c;

   // Verify max value is below IsSampleValid's 0.1 threshold
   double maxVal = a * (W-1) + b * (H-1) + c;
   fprintf(stdout, "\n    Image max value: %.6f (threshold: 0.1)\n", maxVal);

   pcl::BackgroundExtractionInstance instance( pcl::TheBackgroundExtractionProcess );
   pcl::BackgroundExtractor extractor( image, instance );

   // Use automatic sampling
   extractor.GenerateSamples();

   fprintf(stdout, "    Automatic samples generated: %zu\n", extractor.SampleCount());

   TEST_ASSERT(extractor.SampleCount() >= 3,
               "Should generate at least 3 samples for plane fitting");

   // Fit and check
   extractor.FitBackground();
   const pcl::DImage& bg = extractor.Background();

   double maxErr = 0;
   for ( int y = 0; y < H; y++ )
      for ( int x = 0; x < W; x++ )
      {
         double expected = a * x + b * y + c;
         double err = std::abs(bg( x, y ) - expected);
         if ( err > maxErr ) maxErr = err;
      }

   fprintf(stdout, "    Max pixel error: %.2e\n", maxErr);

   // Automatic sampling uses median measurement in local regions,
   // so we allow a larger tolerance
   TEST_ASSERT(maxErr < 0.01, "Auto-sampled gradient error too large");
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
   fprintf(stdout, "PCL Mock Background Extraction Test Suite\n");
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

   // Initialize the BackgroundExtractionProcess first (creates BE* parameters)
   pcl::BackgroundExtractionProcess* bgProcess = new pcl::BackgroundExtractionProcess();

   // Now initialize BG* parameters (used by BackgroundExtractionInstance constructor).
   // These are a separate parameter set from the BE* ones created by the Process.
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

   fprintf(stdout, "Mock API and parameters initialized.\n\n");

   // Run tests
   fprintf(stdout, "--- Background Extraction Tests ---\n\n");

   run_test("Constant background",              test_constant_background);
   run_test("Linear gradient (X)",              test_linear_gradient_x);
   run_test("Linear gradient (Y)",              test_linear_gradient_y);
   run_test("Diagonal gradient (X+Y)",          test_linear_gradient_diagonal);
   run_test("Gradient with noise",              test_gradient_with_noise);
   run_test("Gradient subtraction",             test_gradient_subtraction);
   run_test("Automatic sampling",               test_automatic_sampling);

   fprintf(stdout, "\n--- Results ---\n");
   fprintf(stdout, "  %d tests run, %d passed, %d failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
   fprintf(stdout, "\n%s\n", g_tests_failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

   delete pcl::Module;
   return g_tests_failed > 0 ? 1 : 0;
}
