#include "gp_histogram.h"

ImageHistogram compute_histogram( const Image& image )
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

float histogram_distance( const ImageHistogram& a, const ImageHistogram& b )
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
   float pDist = 0;
   for ( int p = 0; p <= 10; p++ )
   {
      float w = ( p >= 7 ) ? 3.0f : 1.0f;
      pDist += w * std::abs( a.percentiles[p] - b.percentiles[p] );
   }

   // Signal brightness penalty
   float signalPenalty = 0;
   for ( int p = 7; p <= 10; p++ )
   {
      float diff = a.percentiles[p] - b.percentiles[p];
      if ( diff < 0 )
         signalPenalty += 2.0f * diff * diff;
      else
         signalPenalty += diff * diff;
   }

   // Dynamic range penalty
   float refRange = b.percentiles[9] - b.percentiles[2];
   float candRange = a.percentiles[9] - a.percentiles[2];
   float rangePenalty = 0;
   if ( candRange < refRange * 0.5f )
      rangePenalty = ( refRange - candRange ) * ( refRange - candRange );

   return lumEMD * 2.0f + chEMD * 0.5f + pDist * 50.0f
        + signalPenalty * 500.0f + rangePenalty * 200.0f;
}
