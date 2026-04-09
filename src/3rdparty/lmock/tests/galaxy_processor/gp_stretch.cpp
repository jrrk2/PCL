#include "gp_stretch.h"

float vl_sorted_percentile( const std::vector<float>& sorted, float p )
{
   if ( sorted.empty() ) return 0.0f;
   float idx = ( p / 100.0f ) * float( sorted.size() - 1 );
   int lo = int( idx );
   int hi = std::min( lo + 1, int( sorted.size() ) - 1 );
   float w = idx - float( lo );
   return sorted[lo] * ( 1.0f - w ) + sorted[hi] * w;
}

inline float vl_hyperbolic( float v, float D, float b )
{
   float denom = std::asinh( D + b ) - std::asinh( b );
   if ( denom < 1e-9f ) return v;
   return ( std::asinh( D * v + b ) - std::asinh( b ) ) / denom;
}

inline float vl_mtf( float v, float m )
{
   if ( v <= 0.0f ) return 0.0f;
   if ( v >= 1.0f ) return 1.0f;
   float num = ( m - 1.0f ) * v;
   float den = ( 2.0f * m - 1.0f ) * v - m;
   if ( std::abs( den ) < 1e-9f ) return v;
   return std::max( 0.0f, std::min( 1.0f, num / den ) );
}

inline float vl_soft_clip( float v, float threshold, float rolloff )
{
   if ( v <= threshold ) return v;
   float t = ( v - threshold ) / ( 1.0f - threshold );
   t = std::max( 0.0f, std::min( 1.0f, t ) );
   return threshold + ( 1.0f - threshold ) * ( 1.0f - std::pow( 1.0f - t, rolloff ) );
}

float vl_anchor_stats( const Image& img, const double weights[3] )
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

float vl_anchor_adaptive( const Image& img, const double weights[3] )
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

double vl_solve_log_d( float medianIn, float targetMedian, float protectB )
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

void vl_adaptive_scaling( Image& img, const double weights[3], float targetBg )
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
void VeraLuxStretch( Image& img, const VeraLuxParams& p )
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

WaveletLayers starlet_decompose(
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

std::vector<float> starlet_reconstruct(
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

void WaveletEnhance( Image& img, const VeraLuxParams& p )
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
