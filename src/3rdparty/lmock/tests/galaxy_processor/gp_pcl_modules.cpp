// gp_pcl_modules.cpp -- Direct implementations of common PixInsight process
// module operations on pcl::Image data.
//
// These are standalone reimplementations that don't depend on the PCL process
// infrastructure (View, MetaProcess, LockParameter, etc.), making them usable
// in the mock/test environment.

#include "gp_pcl_modules.h"
#include "gp_common.h"

#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>

// ============================================================================
// Midtones Transfer Function (MTF)
// ============================================================================

static inline float MTF( float x, float m )
{
   // PixInsight's midtones transfer function
   if ( x <= 0 ) return 0;
   if ( x >= 1 ) return 1;
   if ( m == 0.5f ) return x;
   return ( m - 1 ) * x / ( ( 2 * m - 1 ) * x - m );
}

void ApplyHistogramTransformation( Image& image, float midtones,
                                   float shadows, float highlights )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   float range = highlights - shadows;
   if ( range <= 0 ) return;

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );
      int N = W * H;
      for ( int i = 0; i < N; ++i )
      {
         float v = p[i];
         // Clip to [shadows, highlights] and rescale to [0,1]
         v = ( v - shadows ) / range;
         v = std::max( 0.0f, std::min( 1.0f, v ) );
         // Apply MTF
         p[i] = MTF( v, midtones );
      }
   }
}

// ============================================================================
// Curves Transformation — Akima subspline interpolation
// ============================================================================

// Akima spline: build slopes for control points
static void akimaSlopes( const std::vector<CurvePoint>& pts,
                         std::vector<double>& slopes )
{
   int n = (int)pts.size();
   slopes.resize( n );
   if ( n < 2 ) return;

   std::vector<double> d( n + 3 );
   // Compute divided differences
   for ( int i = 0; i < n - 1; ++i )
   {
      double dx = pts[i + 1].x - pts[i].x;
      d[i + 2] = ( dx > 0 ) ? ( pts[i + 1].y - pts[i].y ) / dx : 0;
   }
   // Extrapolate boundary differences
   d[1] = 2 * d[2] - d[3];
   d[0] = 2 * d[1] - d[2];
   d[n + 1] = 2 * d[n] - d[n - 1];
   d[n + 2] = 2 * d[n + 1] - d[n];

   for ( int i = 0; i < n; ++i )
   {
      double w1 = std::abs( d[i + 3] - d[i + 2] );
      double w2 = std::abs( d[i + 1] - d[i] );
      if ( w1 + w2 > 1e-30 )
         slopes[i] = ( w1 * d[i + 1] + w2 * d[i + 2] ) / ( w1 + w2 );
      else
         slopes[i] = 0.5 * ( d[i + 1] + d[i + 2] );
   }
}

static double evalAkima( const std::vector<CurvePoint>& pts,
                         const std::vector<double>& slopes, double x )
{
   int n = (int)pts.size();
   if ( n == 0 ) return x;
   if ( n == 1 ) return pts[0].y;
   if ( x <= pts[0].x ) return pts[0].y;
   if ( x >= pts[n - 1].x ) return pts[n - 1].y;

   // Find segment
   int i = 0;
   for ( int j = 0; j < n - 1; ++j )
      if ( x >= pts[j].x && x < pts[j + 1].x ) { i = j; break; }

   double h = pts[i + 1].x - pts[i].x;
   if ( h <= 0 ) return pts[i].y;
   double t = ( x - pts[i].x ) / h;
   double a = pts[i].y;
   double b = slopes[i] * h;
   double c2 = 3 * ( pts[i + 1].y - pts[i].y ) - ( 2 * slopes[i] + slopes[i + 1] ) * h;
   double d3 = 2 * ( pts[i].y - pts[i + 1].y ) + ( slopes[i] + slopes[i + 1] ) * h;
   return a + t * ( b + t * ( c2 + t * d3 ) );
}

void ApplyCurvesTransformation( Image& image,
                                const std::vector<CurvePoint>& rgbCurve )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 || rgbCurve.size() < 2 ) return;

   // Build Akima spline for the RGB curve
   std::vector<double> slopes;
   akimaSlopes( rgbCurve, slopes );

   // Build LUT for speed (256 entries)
   float lut[256];
   for ( int i = 0; i < 256; ++i )
   {
      double x = i / 255.0;
      double y = evalAkima( rgbCurve, slopes, x );
      lut[i] = (float)std::max( 0.0, std::min( 1.0, y ) );
   }

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );
      int N = W * H;
      for ( int i = 0; i < N; ++i )
      {
         float v = std::max( 0.0f, std::min( 1.0f, p[i] ) );
         // Linear interpolation in LUT
         float fi = v * 255.0f;
         int lo = (int)fi;
         int hi = std::min( lo + 1, 255 );
         float frac = fi - lo;
         p[i] = lut[lo] * ( 1 - frac ) + lut[hi] * frac;
      }
   }
}

// ============================================================================
// Unsharp Mask
// ============================================================================

// 1D Gaussian kernel
static std::vector<float> makeGaussianKernel( float sigma )
{
   int r = (int)std::ceil( sigma * 3.0f );
   if ( r < 1 ) r = 1;
   int sz = 2 * r + 1;
   std::vector<float> k( sz );
   float sum = 0;
   for ( int i = 0; i < sz; ++i )
   {
      float d = (float)( i - r );
      k[i] = std::exp( -0.5f * d * d / ( sigma * sigma ) );
      sum += k[i];
   }
   for ( auto& v : k ) v /= sum;
   return k;
}

// Separable Gaussian blur on a single channel buffer
static void gaussianBlur( const float* src, float* dst, int W, int H, float sigma )
{
   auto kernel = makeGaussianKernel( sigma );
   int r = (int)( kernel.size() / 2 );

   // Horizontal pass → temp
   std::vector<float> tmp( W * H );
   for ( int y = 0; y < H; ++y )
      for ( int x = 0; x < W; ++x )
      {
         float sum = 0;
         for ( int k = -r; k <= r; ++k )
         {
            int xx = std::max( 0, std::min( W - 1, x + k ) );
            sum += src[y * W + xx] * kernel[k + r];
         }
         tmp[y * W + x] = sum;
      }

   // Vertical pass → dst
   for ( int y = 0; y < H; ++y )
      for ( int x = 0; x < W; ++x )
      {
         float sum = 0;
         for ( int k = -r; k <= r; ++k )
         {
            int yy = std::max( 0, std::min( H - 1, y + k ) );
            sum += tmp[yy * W + x] * kernel[k + r];
         }
         dst[y * W + x] = sum;
      }
}

void ApplyUnsharpMask( Image& image, float sigma, float amount )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   int N = W * H;
   std::vector<float> blurred( N );

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );
      gaussianBlur( p, blurred.data(), W, H, sigma );
      for ( int i = 0; i < N; ++i )
      {
         float sharp = p[i] + amount * ( p[i] - blurred[i] );
         p[i] = std::max( 0.0f, std::min( 1.0f, sharp ) );
      }
   }
}

// ============================================================================
// Morphological Transformation
// ============================================================================

void ApplyMorphologicalTransformation( Image& image, int op, int size )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   int N = W * H;
   std::vector<float> buf( N );

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );

      // Collect neighborhood values and apply the chosen operator
      for ( int y = 0; y < H; ++y )
         for ( int x = 0; x < W; ++x )
         {
            std::vector<float> vals;
            vals.reserve( ( 2 * size + 1 ) * ( 2 * size + 1 ) );
            for ( int dy = -size; dy <= size; ++dy )
               for ( int dx = -size; dx <= size; ++dx )
               {
                  int yy = std::max( 0, std::min( H - 1, y + dy ) );
                  int xx = std::max( 0, std::min( W - 1, x + dx ) );
                  vals.push_back( p[yy * W + xx] );
               }
            std::sort( vals.begin(), vals.end() );
            float v;
            switch ( op )
            {
            case 0: // erosion (min)
               v = vals.front(); break;
            case 1: // dilation (max)
               v = vals.back(); break;
            case 2: // opening (erosion then dilation — simplified as median-low)
               v = vals[vals.size() / 4]; break;
            case 3: // closing (dilation then erosion — simplified as median-high)
               v = vals[3 * vals.size() / 4]; break;
            case 4: // median
            default:
               v = vals[vals.size() / 2]; break;
            }
            buf[y * W + x] = v;
         }

      std::memcpy( p, buf.data(), N * sizeof( float ) );
   }
}

// ============================================================================
// Auto Histogram — percentile-based automatic stretch
// ============================================================================

void ApplyAutoHistogram( Image& image )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   int N = W * H;
   float targetMedian = 0.25f;  // typical target for astrophoto

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );

      // Find shadows clip (0.1%) and highlights clip (99.9%)
      std::vector<float> sorted( p, p + N );
      std::sort( sorted.begin(), sorted.end() );
      float shadowClip = sorted[(int)( 0.001 * N )];
      float highlightClip = sorted[(int)( 0.999 * N )];

      // Find current median
      float median = sorted[N / 2];

      // Compute MTF midtone that maps median → targetMedian
      // MTF(median, m) = targetMedian  →  m = targetMedian*(median-1) / (2*targetMedian*median - median - targetMedian)
      float m = 0.5f;
      float num = median - shadowClip;
      float range = highlightClip - shadowClip;
      if ( range > 1e-8 )
      {
         float normalizedMedian = num / range;
         if ( normalizedMedian > 0.001f && normalizedMedian < 0.999f )
         {
            m = targetMedian * ( normalizedMedian - 1 )
              / ( ( 2 * targetMedian - 1 ) * normalizedMedian - targetMedian );
            m = std::max( 0.0001f, std::min( 0.9999f, m ) );
         }
      }

      for ( int i = 0; i < N; ++i )
      {
         float v = ( p[i] - shadowClip ) / ( range > 1e-8f ? range : 1.0f );
         v = std::max( 0.0f, std::min( 1.0f, v ) );
         p[i] = MTF( v, m );
      }
   }
}

// ============================================================================
// Convolution — Gaussian convolution
// ============================================================================

void ApplyConvolution( Image& image, float sigma )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   int N = W * H;
   std::vector<float> blurred( N );

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );
      gaussianBlur( p, blurred.data(), W, H, sigma );
      std::memcpy( p, blurred.data(), N * sizeof( float ) );
   }
}

// ============================================================================
// Resample — bilinear resize by scale factor
// ============================================================================

void ApplyResample( Image& image, double scaleFactor )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 || scaleFactor <= 0 ) return;

   int nW = std::max( 1, (int)std::round( W * scaleFactor ) );
   int nH = std::max( 1, (int)std::round( H * scaleFactor ) );

   Image result;
   result.AllocateData( nW, nH, nc, image.ColorSpace() );

   for ( int c = 0; c < nc; ++c )
   {
      const float* src = image.PixelData( c );
      float* dst = result.PixelData( c );

      for ( int y = 0; y < nH; ++y )
         for ( int x = 0; x < nW; ++x )
         {
            float sx = ( x + 0.5f ) / scaleFactor - 0.5f;
            float sy = ( y + 0.5f ) / scaleFactor - 0.5f;
            int x0 = (int)std::floor( sx );
            int y0 = (int)std::floor( sy );
            int x1 = std::min( x0 + 1, W - 1 );
            int y1 = std::min( y0 + 1, H - 1 );
            x0 = std::max( 0, x0 );
            y0 = std::max( 0, y0 );
            float fx = sx - std::floor( sx );
            float fy = sy - std::floor( sy );

            float v = src[y0 * W + x0] * ( 1 - fx ) * ( 1 - fy )
                    + src[y0 * W + x1] * fx * ( 1 - fy )
                    + src[y1 * W + x0] * ( 1 - fx ) * fy
                    + src[y1 * W + x1] * fx * fy;
            dst[y * nW + x] = v;
         }
   }

   image = std::move( result );
}

// ============================================================================
// Crop
// ============================================================================

void ApplyCrop( Image& image, int left, int top, int right, int bottom )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   // Clamp to valid bounds
   left   = std::max( 0, left );
   top    = std::max( 0, top );
   right  = std::min( W, right );
   bottom = std::min( H, bottom );
   int nW = right - left;
   int nH = bottom - top;
   if ( nW <= 0 || nH <= 0 ) return;

   Image result;
   result.AllocateData( nW, nH, nc, image.ColorSpace() );

   for ( int c = 0; c < nc; ++c )
   {
      const float* src = image.PixelData( c );
      float* dst = result.PixelData( c );
      for ( int y = 0; y < nH; ++y )
         std::memcpy( dst + y * nW, src + ( y + top ) * W + left, nW * sizeof( float ) );
   }

   image = std::move( result );
}

// ============================================================================
// Rotation
// ============================================================================

void ApplyRotation( Image& image, double angleDeg )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 ) return;

   double rad = angleDeg * M_PI / 180.0;
   double cosA = std::cos( rad ), sinA = std::sin( rad );

   // Compute output size to contain entire rotated image
   double cx = W / 2.0, cy = H / 2.0;
   double corners[4][2] = { {0,0}, {(double)W,0}, {0,(double)H}, {(double)W,(double)H} };
   double minX = 1e30, maxX = -1e30, minY = 1e30, maxY = -1e30;
   for ( auto& corner : corners )
   {
      double rx = cosA * ( corner[0] - cx ) - sinA * ( corner[1] - cy ) + cx;
      double ry = sinA * ( corner[0] - cx ) + cosA * ( corner[1] - cy ) + cy;
      minX = std::min( minX, rx ); maxX = std::max( maxX, rx );
      minY = std::min( minY, ry ); maxY = std::max( maxY, ry );
   }
   int nW = (int)std::ceil( maxX - minX );
   int nH = (int)std::ceil( maxY - minY );
   double ncx = nW / 2.0, ncy = nH / 2.0;

   Image result;
   result.AllocateData( nW, nH, nc, image.ColorSpace() );

   for ( int c = 0; c < nc; ++c )
   {
      const float* src = image.PixelData( c );
      float* dst = result.PixelData( c );

      for ( int y = 0; y < nH; ++y )
         for ( int x = 0; x < nW; ++x )
         {
            // Inverse rotation: source coordinates
            double dx = x - ncx, dy = y - ncy;
            double sx = cosA * dx + sinA * dy + cx;
            double sy = -sinA * dx + cosA * dy + cy;

            // Bilinear interpolation
            int x0 = (int)std::floor( sx ), y0 = (int)std::floor( sy );
            if ( x0 < 0 || x0 >= W - 1 || y0 < 0 || y0 >= H - 1 )
            {
               dst[y * nW + x] = 0; // fill with black
               continue;
            }
            float fx = (float)( sx - x0 ), fy = (float)( sy - y0 );
            dst[y * nW + x] =
               src[y0 * W + x0] * ( 1 - fx ) * ( 1 - fy )
             + src[y0 * W + x0 + 1] * fx * ( 1 - fy )
             + src[( y0 + 1 ) * W + x0] * ( 1 - fx ) * fy
             + src[( y0 + 1 ) * W + x0 + 1] * fx * fy;
         }
   }

   image = std::move( result );
}

// ============================================================================
// PixelMath — simple expression evaluator
// ============================================================================
// Supports: $T (current pixel), constants, +, -, *, /, ^, (, )
// Common functions: sqrt, log, ln, exp, abs, min, max, pow, sin, cos

namespace {

struct PMParser
{
   const char* s;
   float T; // current pixel value

   float parse()        { return parseAdd(); }

   float parseAdd()
   {
      float v = parseMul();
      while ( *s == '+' || *s == '-' )
      {
         char op = *s++;
         float r = parseMul();
         v = ( op == '+' ) ? v + r : v - r;
      }
      return v;
   }

   float parseMul()
   {
      float v = parsePow();
      while ( *s == '*' || *s == '/' )
      {
         char op = *s++;
         float r = parsePow();
         v = ( op == '*' ) ? v * r : ( r != 0 ? v / r : 0 );
      }
      return v;
   }

   float parsePow()
   {
      float v = parseUnary();
      if ( *s == '^' ) { ++s; v = std::pow( v, parseUnary() ); }
      return v;
   }

   float parseUnary()
   {
      skipWS();
      if ( *s == '-' ) { ++s; return -parseAtom(); }
      if ( *s == '+' ) { ++s; }
      return parseAtom();
   }

   float parseAtom()
   {
      skipWS();

      // $T — current pixel
      if ( s[0] == '$' && s[1] == 'T' ) { s += 2; return T; }

      // Parenthesized expression
      if ( *s == '(' )
      {
         ++s;
         float v = parse();
         if ( *s == ')' ) ++s;
         return v;
      }

      // Function calls
      if ( std::isalpha( *s ) )
      {
         const char* start = s;
         while ( std::isalpha( *s ) ) ++s;
         std::string name( start, s );
         skipWS();
         if ( *s == '(' )
         {
            ++s;
            float a = parse();
            float b = 0;
            bool hasB = false;
            if ( *s == ',' ) { ++s; b = parse(); hasB = true; }
            if ( *s == ')' ) ++s;

            if ( name == "sqrt" ) return std::sqrt( std::max( 0.0f, a ) );
            if ( name == "log" || name == "ln" ) return a > 0 ? std::log( a ) : -30.0f;
            if ( name == "log10" ) return a > 0 ? std::log10( a ) : -30.0f;
            if ( name == "exp" ) return std::exp( std::min( 80.0f, a ) );
            if ( name == "abs" ) return std::abs( a );
            if ( name == "min" && hasB ) return std::min( a, b );
            if ( name == "max" && hasB ) return std::max( a, b );
            if ( name == "pow" && hasB ) return std::pow( a, b );
            if ( name == "sin" ) return std::sin( a );
            if ( name == "cos" ) return std::cos( a );
            return 0;
         }
         // Named constant
         if ( name == "pi" ) return (float)M_PI;
         if ( name == "e" ) return (float)M_E;
         return 0;
      }

      // Numeric literal
      char* end = nullptr;
      float v = std::strtof( s, &end );
      if ( end > s ) { s = end; return v; }
      return 0;
   }

   void skipWS() { while ( *s == ' ' || *s == '\t' ) ++s; }
};

} // anon namespace

void ApplyPixelMath( Image& image, const std::string& expr )
{
   int W = image.Width(), H = image.Height();
   int nc = image.NumberOfChannels();
   if ( W == 0 || H == 0 || expr.empty() ) return;

   for ( int c = 0; c < nc; ++c )
   {
      float* p = image.PixelData( c );
      int N = W * H;
      for ( int i = 0; i < N; ++i )
      {
         PMParser pm;
         pm.s = expr.c_str();
         pm.T = p[i];
         p[i] = std::max( 0.0f, std::min( 1.0f, pm.parse() ) );
      }
   }
}

// ============================================================================
// Lua registration — adds galaxy.* functions for PCL module operations
// ============================================================================

void RegisterPCLModulesLua( lua_State* L )
{
   // This function is called from gp_lua.cpp to add module functions
   // to the galaxy table. The actual sol2-based registration is done
   // in RegisterPCLModulesLuaSol() below, called from RunLuaScript().
   // This C API version is a no-op placeholder for the header declaration.
   (void)L;
}
