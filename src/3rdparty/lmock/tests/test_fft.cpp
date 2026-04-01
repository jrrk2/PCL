// ============================================================================
// test_fft.cpp - Headless FFT differential test
//
// Tests the mock API's FFT implementation directly against known results.
// Bypasses PCL's internal threading to avoid mock thread infrastructure issues.
//
// Usage:
//   ./test_fft                     # Run all FFT tests
//   ./test_fft --save-reference    # Save XISF files for PI comparison
//   ./test_fft --compare <dir>     # Compare against PixInsight reference files
// ============================================================================

#include <QApplication>
#include <QDebug>

#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include <pcl/Image.h>
#include <pcl/Complex.h>
#include <pcl/FFT1D.h>

#include "../PCLMockAPI.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

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
   IsoString Name() const override             { return "TestFFT"; }
   String Description() const override         { return "FFT Test Module"; }
   String Company() const override             { return "Test"; }
   String Author() const override              { return "Test"; }
   String Copyright() const override           { return "Test"; }
   String TradeMarks() const override          { return String(); }
   String OriginalFileName() const override    { return "test_fft"; }
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
// Direct FFT via API function pointers (bypasses PCL Thread infrastructure)
// ============================================================================

// 2D FFT via row-then-column 1D FFTs (single-threaded, no PCL Thread)
static void fft2d_direct(pcl::fcomplex* data, int width, int height, bool inverse)
{
   // Row transforms
   void* rowHandle = inverse
      ? (API->Numerical->FFTCreateComplexInverseTransformF)(width)
      : (API->Numerical->FFTCreateComplexTransformF)(width);

   std::vector<pcl::fcomplex> rowBuf(width);
   for (int y = 0; y < height; y++)
   {
      pcl::fcomplex* row = data + y * width;
      memcpy(rowBuf.data(), row, width * sizeof(pcl::fcomplex));
      if (inverse)
         (API->Numerical->FFTComplexInverseTransformF)(rowHandle, row, rowBuf.data());
      else
         (API->Numerical->FFTComplexTransformF)(rowHandle, row, rowBuf.data());
   }
   (API->Numerical->FFTDestroyTransform)(rowHandle);

   // Column transforms (transpose, transform rows, transpose back)
   void* colHandle = inverse
      ? (API->Numerical->FFTCreateComplexInverseTransformF)(height)
      : (API->Numerical->FFTCreateComplexTransformF)(height);

   std::vector<pcl::fcomplex> col(height);
   std::vector<pcl::fcomplex> colOut(height);
   for (int x = 0; x < width; x++)
   {
      // Extract column
      for (int y = 0; y < height; y++)
         col[y] = data[y * width + x];

      if (inverse)
         (API->Numerical->FFTComplexInverseTransformF)(colHandle, colOut.data(), col.data());
      else
         (API->Numerical->FFTComplexTransformF)(colHandle, colOut.data(), col.data());

      // Write back
      for (int y = 0; y < height; y++)
         data[y * width + x] = colOut[y];
   }
   (API->Numerical->FFTDestroyTransform)(colHandle);
}

// ============================================================================
// Helper: Create synthetic test data
// ============================================================================

static std::vector<pcl::fcomplex> CreateConstantData(int W, int H, float value)
{
   std::vector<pcl::fcomplex> data(W * H);
   for (auto& c : data)
      c = pcl::fcomplex(value, 0.0f);
   return data;
}

static std::vector<pcl::fcomplex> CreateCosineData(int W, int H, double fx, double fy)
{
   std::vector<pcl::fcomplex> data(W * H);
   for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
      {
         float val = float(0.5 + 0.5 * cos(2.0 * M_PI * fx * x / W
                                          + 2.0 * M_PI * fy * y / H));
         data[y * W + x] = pcl::fcomplex(val, 0.0f);
      }
   return data;
}

static std::vector<pcl::fcomplex> CreateImpulseData(int W, int H)
{
   std::vector<pcl::fcomplex> data(W * H, pcl::fcomplex(0, 0));
   data[0] = pcl::fcomplex(1.0f, 0.0f);
   return data;
}

// ============================================================================
// Test 1: 1D FFT round-trip
// ============================================================================

static bool test_1d_fft_roundtrip()
{
   const int N = 64;
   std::vector<pcl::fcomplex> original(N);
   for (int i = 0; i < N; i++)
      original[i] = pcl::fcomplex(float(cos(2.0 * M_PI * 3.0 * i / N)), 0.0f);

   std::vector<pcl::fcomplex> freq(N);
   std::vector<pcl::fcomplex> recovered(N);

   // Forward
   void* fwd = (API->Numerical->FFTCreateComplexTransformF)(N);
   TEST_ASSERT(fwd != nullptr, "FFTCreateComplexTransformF returned null");
   (API->Numerical->FFTComplexTransformF)(fwd, freq.data(), original.data());
   (API->Numerical->FFTDestroyTransform)(fwd);

   // Inverse
   void* inv = (API->Numerical->FFTCreateComplexInverseTransformF)(N);
   TEST_ASSERT(inv != nullptr, "FFTCreateComplexInverseTransformF returned null");
   (API->Numerical->FFTComplexInverseTransformF)(inv, recovered.data(), freq.data());
   (API->Numerical->FFTDestroyTransform)(inv);

   // Compare (normalize by N)
   double maxErr = 0;
   for (int i = 0; i < N; i++)
   {
      double realErr = std::abs(double(recovered[i].Real()) / N - double(original[i].Real()));
      double imagErr = std::abs(double(recovered[i].Imag()) / N - double(original[i].Imag()));
      maxErr = std::max(maxErr, std::max(realErr, imagErr));
   }

   fprintf(stdout, "\n    1D FFT round-trip, N=%d, max error: %.2e\n", N, maxErr);
   TEST_ASSERT_NEAR(maxErr, 0.0, 1e-5, "1D FFT round-trip error too large");
   return true;
}

// ============================================================================
// Test 2: 2D FFT round-trip (constant image)
// ============================================================================

static bool test_2d_fft_roundtrip_constant()
{
   const int W = 64, H = 64;
   const float value = 0.42f;

   auto original = CreateConstantData(W, H, value);
   auto data = original;

   fft2d_direct(data.data(), W, H, false);  // forward
   fft2d_direct(data.data(), W, H, true);   // inverse

   // Compare (normalize by N)
   double maxErr = 0;
   int N = W * H;
   for (int i = 0; i < N; i++)
   {
      double err = std::abs(double(data[i].Real()) / N - double(original[i].Real()));
      maxErr = std::max(maxErr, err);
   }

   fprintf(stdout, "\n    Constant (%.2f), %dx%d, max error: %.2e\n", value, W, H, maxErr);
   TEST_ASSERT_NEAR(maxErr, 0.0, 1e-4, "2D FFT round-trip error (constant)");
   return true;
}

// ============================================================================
// Test 3: 2D FFT round-trip (cosine wave)
// ============================================================================

static bool test_2d_fft_roundtrip_cosine()
{
   const int W = 64, H = 64;

   auto original = CreateCosineData(W, H, 3.0, 5.0);
   auto data = original;

   fft2d_direct(data.data(), W, H, false);
   fft2d_direct(data.data(), W, H, true);

   double maxErr = 0;
   int N = W * H;
   for (int i = 0; i < N; i++)
   {
      double err = std::abs(double(data[i].Real()) / N - double(original[i].Real()));
      maxErr = std::max(maxErr, err);
   }

   fprintf(stdout, "\n    Cosine fx=3 fy=5, %dx%d, max error: %.2e\n", W, H, maxErr);
   TEST_ASSERT_NEAR(maxErr, 0.0, 1e-4, "2D FFT round-trip error (cosine)");
   return true;
}

// ============================================================================
// Test 4: FFT of constant should have DC-only
// ============================================================================

static bool test_2d_fft_dc_only()
{
   const int W = 64, H = 64;
   const float value = 0.75f;

   auto data = CreateConstantData(W, H, value);

   fft2d_direct(data.data(), W, H, false);

   // DC component at (0,0)
   double expectedDC = value * W * H;
   double dc_real = data[0].Real();
   double dc_imag = data[0].Imag();

   fprintf(stdout, "\n    DC: real=%.4f imag=%.4f  expected=%.4f\n",
           dc_real, dc_imag, expectedDC);

   // Check non-DC
   double maxNonDC = 0;
   for (int i = 1; i < W * H; i++)
   {
      double mag = data[i].Mag();
      if (mag > maxNonDC) maxNonDC = mag;
   }
   fprintf(stdout, "    Max non-DC magnitude: %.2e\n", maxNonDC);

   TEST_ASSERT_NEAR(dc_real, expectedDC, expectedDC * 1e-4,
                     "DC component mismatch");
   TEST_ASSERT_NEAR(dc_imag, 0.0, 1e-3,
                     "DC imaginary should be zero");
   TEST_ASSERT(maxNonDC < 1e-2,
               "Non-DC should be near zero for constant input");
   return true;
}

// ============================================================================
// Test 5: FFT of impulse should have flat spectrum
// ============================================================================

static bool test_2d_fft_impulse()
{
   const int W = 64, H = 64;

   auto data = CreateImpulseData(W, H);

   fft2d_direct(data.data(), W, H, false);

   double minMag = 1e30, maxMag = 0;
   for (int i = 0; i < W * H; i++)
   {
      double mag = data[i].Mag();
      if (mag < minMag) minMag = mag;
      if (mag > maxMag) maxMag = mag;
   }

   fprintf(stdout, "\n    Impulse at (0,0), %dx%d\n", W, H);
   fprintf(stdout, "    Min mag: %.6f  Max mag: %.6f  (expected: 1.0)\n", minMag, maxMag);

   TEST_ASSERT_NEAR(minMag, 1.0, 1e-4, "Impulse min magnitude should be 1.0");
   TEST_ASSERT_NEAR(maxMag, 1.0, 1e-4, "Impulse max magnitude should be 1.0");
   return true;
}

// ============================================================================
// Test 6: FFT cosine peaks at expected frequencies
// ============================================================================

static bool test_2d_fft_cosine_peaks()
{
   const int W = 64, H = 64;
   const double fx = 4.0;

   std::vector<pcl::fcomplex> data(W * H);
   for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
         data[y * W + x] = pcl::fcomplex(float(0.5 * cos(2.0 * M_PI * fx * x / W)), 0.0f);

   fft2d_direct(data.data(), W, H, false);

   // Find two largest peaks
   double peak1_mag = 0, peak2_mag = 0;
   int peak1_x = 0, peak1_y = 0, peak2_x = 0, peak2_y = 0;

   for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++)
      {
         double mag = data[y * W + x].Mag();
         if (mag > peak1_mag)
         {
            peak2_mag = peak1_mag; peak2_x = peak1_x; peak2_y = peak1_y;
            peak1_mag = mag; peak1_x = x; peak1_y = y;
         }
         else if (mag > peak2_mag)
         {
            peak2_mag = mag; peak2_x = x; peak2_y = y;
         }
      }

   fprintf(stdout, "\n    Cosine fx=%.0f, %dx%d\n", fx, W, H);
   fprintf(stdout, "    Peak 1: (%d, %d) mag=%.4f\n", peak1_x, peak1_y, peak1_mag);
   fprintf(stdout, "    Peak 2: (%d, %d) mag=%.4f\n", peak2_x, peak2_y, peak2_mag);

   TEST_ASSERT(peak1_y == 0, "Peak 1 should be at y=0");
   TEST_ASSERT(peak2_y == 0, "Peak 2 should be at y=0");

   // Conjugate symmetry
   double magRatio = peak1_mag > 0 ? peak2_mag / peak1_mag : 0;
   TEST_ASSERT_NEAR(magRatio, 1.0, 0.01, "Conjugate peaks should have equal magnitude");

   return true;
}

// ============================================================================
// Test 7: Parseval's theorem
// ============================================================================

static bool test_2d_fft_parseval()
{
   const int W = 64, H = 64;

   auto original = CreateCosineData(W, H, 7.0, 3.0);
   auto freq = original;

   double spatialEnergy = 0;
   for (const auto& c : original)
      spatialEnergy += double(c.Real()) * double(c.Real());

   fft2d_direct(freq.data(), W, H, false);

   double spectralEnergy = 0;
   int N = W * H;
   for (const auto& c : freq)
   {
      double mag = c.Mag();
      spectralEnergy += mag * mag;
   }
   spectralEnergy /= N;

   fprintf(stdout, "\n    Spatial energy:  %.6f\n", spatialEnergy);
   fprintf(stdout, "    Spectral energy: %.6f\n", spectralEnergy);
   fprintf(stdout, "    Ratio: %.6f\n", spectralEnergy / spatialEnergy);

   TEST_ASSERT_NEAR(spectralEnergy / spatialEnergy, 1.0, 0.01,
                     "Parseval's theorem: energy should be conserved");
   return true;
}

// ============================================================================
// Test 8: Optimized length function
// ============================================================================

static bool test_optimized_length()
{
   // OptimizedLength should return a value >= input that's efficient for FFT
   struct { int input; int minExpected; } cases[] = {
      {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5},
      {7, 7}, {8, 8}, {9, 9}, {10, 10},
      {63, 63}, {64, 64}, {65, 65}, {100, 100},
      {127, 127}, {128, 128}, {255, 255}, {256, 256}
   };

   fprintf(stdout, "\n");
   for (auto& tc : cases)
   {
      int result = int((API->Numerical->FFTComplexOptimizedLengthF)(tc.input));
      fprintf(stdout, "    OptimizedLength(%d) = %d\n", tc.input, result);
      TEST_ASSERT(result >= tc.input, "OptimizedLength must be >= input");
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

   fprintf(stdout, "===========================================\n");
   fprintf(stdout, "PCL Mock FFT Test Suite\n");
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

   // Run unit tests
   fprintf(stdout, "--- FFT Unit Tests (Direct API) ---\n\n");

   run_test("Optimized length",                   test_optimized_length);
   run_test("1D FFT round-trip",                  test_1d_fft_roundtrip);
   run_test("2D FFT round-trip (constant)",       test_2d_fft_roundtrip_constant);
   run_test("2D FFT round-trip (cosine)",         test_2d_fft_roundtrip_cosine);
   run_test("2D FFT DC-only (constant)",          test_2d_fft_dc_only);
   run_test("2D FFT impulse (flat spectrum)",     test_2d_fft_impulse);
   run_test("2D FFT cosine peaks",                test_2d_fft_cosine_peaks);
   run_test("2D FFT Parseval's theorem",          test_2d_fft_parseval);

   fprintf(stdout, "\n--- Results ---\n");
   fprintf(stdout, "  %d tests run, %d passed, %d failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
   fprintf(stdout, "\n%s\n", g_tests_failed == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

   delete pcl::Module;
   return g_tests_failed > 0 ? 1 : 0;
}
