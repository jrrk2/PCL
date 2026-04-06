// xisf_snr.cpp — Measure S/N ratio and key image quality metrics from XISF/FITS files
//
// Usage: xisf_snr <file.xisf|file.fits> [file2 ...]
//
// Reports per-channel and combined:
//   - Signal: median, mean, peak
//   - Noise: MAD, σ (from MAD), Sn estimator
//   - S/N ratio (signal/noise)
//   - Dynamic range (peak/noise floor)
//   - Background level and uniformity
//   - Star signal (estimated from upper percentiles vs background)

#include <pcl/XISF.h>
#include <pcl/MetaModule.h>

// FITS module reader — exported from libFITS-pxm
#include "FITS/FITS.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace pcl;

// ─── Robust noise estimators ────────────────────────────────────────────────

// Median Absolute Deviation from median
static double ComputeMAD( std::vector<float>& v )
{
   size_t n = v.size();
   if ( n == 0 ) return 0;
   std::nth_element( v.begin(), v.begin() + n / 2, v.end() );
   float median = v[n / 2];
   std::vector<float> dev( n );
   for ( size_t i = 0; i < n; i++ )
      dev[i] = std::abs( v[i] - median );
   std::nth_element( dev.begin(), dev.begin() + n / 2, dev.end() );
   return dev[n / 2];
}

// Sn estimator (Rousseeuw & Croux 1993) — more robust than MAD for asymmetric noise.
// Uses a fast O(n log n) approximation: for a random subsample, compute median of
// |xi - xj| for each xi, then take the median of those medians.
static double ComputeSn( const std::vector<float>& v, int maxSamples = 10000 )
{
   size_t n = v.size();
   if ( n < 2 ) return 0;

   // Subsample if too large
   std::vector<float> sub;
   if ( (int)n > maxSamples )
   {
      sub.resize( maxSamples );
      unsigned seed = 12345;
      for ( int i = 0; i < maxSamples; i++ )
      {
         seed = seed * 1103515245 + 12345;
         sub[i] = v[seed % n];
      }
   }
   else
      sub = v;

   size_t m = sub.size();
   std::sort( sub.begin(), sub.end() );

   // For each element, compute median |xi - xj|
   std::vector<float> medDiffs( m );
   for ( size_t i = 0; i < m; i++ )
   {
      std::vector<float> diffs( m - 1 );
      int k = 0;
      for ( size_t j = 0; j < m; j++ )
         if ( j != i )
            diffs[k++] = std::abs( sub[i] - sub[j] );
      std::nth_element( diffs.begin(), diffs.begin() + k / 2, diffs.begin() + k );
      medDiffs[i] = diffs[k / 2];
   }

   // Sn = 1.1926 * median(medDiffs)
   std::nth_element( medDiffs.begin(), medDiffs.begin() + m / 2, medDiffs.end() );
   return 1.1926 * medDiffs[m / 2];
}

// ─── Statistics for a single channel ────────────────────────────────────────

struct ChannelStats
{
   double min, max;
   double mean, median;
   double mad, sigma_mad;  // MAD and σ estimated from MAD (σ ≈ 1.4826 * MAD)
   double sn;              // Sn estimator
   double snr_mad;         // S/N using MAD-based noise
   double snr_sn;          // S/N using Sn noise
   double p05, p10, p25, p50, p75, p90, p95, p99;
   double bgLevel;         // estimated background (median of bottom 50%)
   double bgMAD;           // background uniformity
   double starSignal;      // p99 - background (proxy for bright star/nebula signal)
   double dynamicRange;    // log2(max / noise_floor)
   int    totalPixels;
   int    clippedLow;      // pixels at 0
   int    clippedHigh;     // pixels at or near 1
};

static ChannelStats ComputeChannelStats( const Image& image, int channel )
{
   ChannelStats s{};
   int w = image.Width(), h = image.Height();
   int n = w * h;
   s.totalPixels = n;

   std::vector<float> data( n );
   for ( int y = 0; y < h; y++ )
      for ( int x = 0; x < w; x++ )
         data[y * w + x] = image( x, y, channel );

   // Sort for percentiles
   std::vector<float> sorted = data;
   std::sort( sorted.begin(), sorted.end() );

   s.min = sorted.front();
   s.max = sorted.back();
   s.median = sorted[n / 2];
   s.mean = 0;
   for ( auto v : data ) s.mean += v;
   s.mean /= n;

   // Percentiles
   s.p05 = sorted[int( 0.05 * ( n - 1 ) )];
   s.p10 = sorted[int( 0.10 * ( n - 1 ) )];
   s.p25 = sorted[int( 0.25 * ( n - 1 ) )];
   s.p50 = sorted[int( 0.50 * ( n - 1 ) )];
   s.p75 = sorted[int( 0.75 * ( n - 1 ) )];
   s.p90 = sorted[int( 0.90 * ( n - 1 ) )];
   s.p95 = sorted[int( 0.95 * ( n - 1 ) )];
   s.p99 = sorted[int( 0.99 * ( n - 1 ) )];

   // Clipping
   s.clippedLow = 0;
   s.clippedHigh = 0;
   for ( auto v : data )
   {
      if ( v <= 0 ) s.clippedLow++;
      if ( v >= 0.999f ) s.clippedHigh++;
   }

   // Noise: MAD
   s.mad = ComputeMAD( data );
   s.sigma_mad = 1.4826 * s.mad;

   // Noise: Sn estimator
   s.sn = ComputeSn( data );

   // Background: use lower 50% of pixels
   int bgN = n / 2;
   std::vector<float> bgPixels( sorted.begin(), sorted.begin() + bgN );
   s.bgLevel = bgPixels[bgN / 2];
   std::vector<float> bgDev( bgN );
   for ( int i = 0; i < bgN; i++ )
      bgDev[i] = std::abs( bgPixels[i] - (float)s.bgLevel );
   std::nth_element( bgDev.begin(), bgDev.begin() + bgN / 2, bgDev.end() );
   s.bgMAD = bgDev[bgN / 2];

   // Star/nebula signal above background
   s.starSignal = s.p99 - s.bgLevel;

   // S/N ratios (signal = median, noise = σ estimate)
   s.snr_mad = s.sigma_mad > 0 ? s.median / s.sigma_mad : 0;
   s.snr_sn = s.sn > 0 ? s.median / s.sn : 0;

   // Dynamic range in stops
   double noiseFloor = std::max( s.sigma_mad, 1e-10 );
   s.dynamicRange = std::log2( std::max( s.max, 1e-10 ) / noiseFloor );

   return s;
}

// ─── Print results ──────────────────────────────────────────────────────────

static void PrintChannelStats( const char* name, const ChannelStats& s )
{
   std::cout << "  --- " << name << " ---\n";
   std::cout << "    Signal:  median=" << std::scientific << std::setprecision( 4 ) << s.median
             << "  mean=" << s.mean << "  peak=" << s.max << "\n";
   std::cout << "    Noise:   MAD=" << s.mad
             << "  σ(MAD)=" << s.sigma_mad
             << "  Sn=" << s.sn << "\n";
   std::cout << std::fixed;
   std::cout << "    S/N:     " << std::setprecision( 1 ) << s.snr_mad
             << " (MAD)  " << s.snr_sn << " (Sn)\n";
   std::cout << "    Background: level=" << std::scientific << std::setprecision( 4 ) << s.bgLevel
             << "  uniformity(MAD)=" << s.bgMAD << "\n";
   std::cout << "    Star signal (p99-bg): " << s.starSignal << "\n";
   std::cout << std::fixed << "    Dynamic range: " << std::setprecision( 1 ) << s.dynamicRange << " stops\n";
   std::cout << "    Percentiles: p5=" << std::scientific << std::setprecision( 3 ) << s.p05
             << "  p25=" << s.p25 << "  p50=" << s.p50
             << "  p75=" << s.p75 << "  p95=" << s.p95 << "  p99=" << s.p99 << "\n";
   std::cout << std::fixed << "    Clipped: " << s.clippedLow << " low ("
             << std::setprecision( 1 ) << ( 100.0 * s.clippedLow / s.totalPixels ) << "%)  "
             << s.clippedHigh << " high ("
             << ( 100.0 * s.clippedHigh / s.totalPixels ) << "%)\n";
}

// ─── File processing ────────────────────────────────────────────────────────

static bool EndsWithCI( const std::string& s, const char* suffix )
{
   std::string suf( suffix );
   if ( s.size() < suf.size() ) return false;
   std::string tail = s.substr( s.size() - suf.size() );
   for ( auto& c : tail ) c = tolower( c );
   return tail == suf;
}

static void AnalyzeFile( const std::string& path )
{
   std::cout << "======================================================================\n";
   std::cout << "File: " << path << "\n";
   std::cout << "======================================================================\n\n";

   Image image;
   bool isFITS = EndsWithCI( path, ".fits" ) || EndsWithCI( path, ".fit" );
   bool isXISF = EndsWithCI( path, ".xisf" );

   if ( isXISF )
   {
      XISFReader reader;
      reader.Open( String::UTF8ToUTF16( path.c_str() ) );
      if ( reader.NumberOfImages() == 0 )
      {
         std::cerr << "  ERROR: No images in XISF file\n\n";
         return;
      }
      reader.SelectImage( 0 );
      reader.ReadImage( image );

      // Print key XISF properties if present
      PropertyArray props = reader.ReadImageProperties();
      for ( const auto& p : props )
      {
         IsoString id = p.Id();
         if ( id == "Instrument:Telescope:FocalLength" ||
              id == "Instrument:Sensor:XPixelSize" ||
              id == "Instrument:ExposureTime" ||
              id == "Observation:Time:Start" ||
              id == "PCL:CFASourceChannel" )
         {
            std::cout << "  " << id.c_str() << " = " << p.Value().ToString().ToUTF8() << "\n";
         }
      }
      reader.Close();
   }
   else if ( isFITS )
   {
      FITSReader reader;
      reader.Open( String::UTF8ToUTF16( path.c_str() ) );
      if ( reader.NumberOfImages() == 0 )
      {
         std::cerr << "  ERROR: No images in FITS file\n\n";
         return;
      }
      reader.SetIndex( 0 );
      reader.ReadImage( image );

      // Print key FITS keywords
      FITSKeywordArray keywords = reader.ReadFITSKeywords();
      for ( const auto& kw : keywords )
      {
         IsoString name = kw.name.Trimmed();
         if ( name == "EXPTIME" || name == "EXPOSURE" || name == "NAXIS1" ||
              name == "NAXIS2" || name == "BITPIX" || name == "NCOMBINE" ||
              name == "STACKCNT" || name == "DATE-OBS" || name == "FILTER" ||
              name == "GAIN" || name == "CCD-TEMP" || name == "XPIXSZ" )
         {
            std::cout << "  " << name.c_str() << " = " << IsoString( kw.value.Trimmed() ).c_str() << "\n";
         }
      }
      reader.Close();
   }
   else
   {
      std::cerr << "  ERROR: Unsupported file format (need .xisf or .fits)\n\n";
      return;
   }

   int w = image.Width(), h = image.Height(), ch = image.NumberOfChannels();
   std::cout << "\n  Dimensions: " << w << " x " << h << " x " << ch;
   if ( ch == 1 ) std::cout << " (mono)";
   else if ( ch == 3 ) std::cout << " (RGB)";
   std::cout << "\n  Pixels per channel: " << w * h
             << " (" << std::setprecision( 1 ) << std::fixed
             << ( double( w ) * h * ch * 4 ) / ( 1024 * 1024 ) << " MB float32)\n\n";

   const char* chNames[] = { "Red", "Green", "Blue", "Alpha" };
   const char* monoNames[] = { "Luminance" };

   ChannelStats stats[4];
   for ( int c = 0; c < std::min( ch, 4 ); c++ )
   {
      stats[c] = ComputeChannelStats( image, c );
      PrintChannelStats( ch == 1 ? monoNames[0] : chNames[c], stats[c] );
      std::cout << "\n";
   }

   // Combined luminance stats for RGB
   if ( ch >= 3 )
   {
      Image lumImg( w, h, ColorSpace::Gray );
      for ( int y = 0; y < h; y++ )
         for ( int x = 0; x < w; x++ )
            lumImg( x, y, 0 ) = 0.2126f * image( x, y, 0 )
                              + 0.7152f * image( x, y, 1 )
                              + 0.0722f * image( x, y, 2 );
      ChannelStats lumStats = ComputeChannelStats( lumImg, 0 );
      PrintChannelStats( "Luminance (computed)", lumStats );
      std::cout << "\n";

      // Channel balance
      std::cout << "  --- Channel Balance ---\n";
      double refMed = stats[1].median; // green as reference
      if ( refMed > 0 )
      {
         std::cout << "    R/G median ratio: " << std::setprecision( 4 ) << ( stats[0].median / refMed ) << "\n";
         std::cout << "    B/G median ratio: " << std::setprecision( 4 ) << ( stats[2].median / refMed ) << "\n";
      }
      double refNoise = stats[1].sigma_mad;
      if ( refNoise > 0 )
      {
         std::cout << "    R/G noise ratio:  " << std::setprecision( 4 ) << ( stats[0].sigma_mad / refNoise ) << "\n";
         std::cout << "    B/G noise ratio:  " << std::setprecision( 4 ) << ( stats[2].sigma_mad / refNoise ) << "\n";
      }
   }

   // Summary
   std::cout << "\n  === SUMMARY ===\n";
   if ( ch >= 3 )
   {
      double avgSNR = ( stats[0].snr_mad + stats[1].snr_mad + stats[2].snr_mad ) / 3;
      double avgStar = ( stats[0].starSignal + stats[1].starSignal + stats[2].starSignal ) / 3;
      std::cout << "    Avg S/N (MAD): " << std::setprecision( 1 ) << avgSNR << "\n";
      std::cout << "    Avg star signal (p99-bg): " << std::scientific << std::setprecision( 3 ) << avgStar << "\n";
      std::cout << "    Avg dynamic range: " << std::fixed << std::setprecision( 1 )
                << ( stats[0].dynamicRange + stats[1].dynamicRange + stats[2].dynamicRange ) / 3 << " stops\n";
   }
   else
   {
      std::cout << "    S/N (MAD): " << std::setprecision( 1 ) << stats[0].snr_mad << "\n";
      std::cout << "    Star signal (p99-bg): " << std::scientific << std::setprecision( 3 ) << stats[0].starSignal << "\n";
      std::cout << "    Dynamic range: " << std::fixed << std::setprecision( 1 ) << stats[0].dynamicRange << " stops\n";
   }
   std::cout << std::defaultfloat << "\n";
}

// ─── Module stubs ───────────────────────────────────────────────────────────

class SNRModule : public MetaModule
{
public:
   SNRModule() : MetaModule() {}
   const char* Version() const override { return "1.0.0"; }
   IsoString Name() const override { return "xisf_snr"; }
   String Description() const override { return "XISF S/N Metrics"; }
   String Company() const override { return String(); }
   String Author() const override { return "test"; }
   String Copyright() const override { return String(); }
   String TradeMarks() const override { return String(); }
   String OriginalFileName() const override { return "xisf_snr"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2026; m=4; d=1; }
};

extern "C" void* IdentifyPixInsightModule()
{
   static SNRModule module;
   return &module;
}

// ─── Main ───────────────────────────────────────────────────────────────────

int main( int argc, char** argv )
{
   if ( argc < 2 )
   {
      std::cerr << "Usage: xisf_snr <file.xisf|file.fits> [file2 ...]\n";
      std::cerr << "\nMeasures S/N ratio, noise, dynamic range and other quality metrics.\n";
      std::cerr << "Compare stacks from different sources to identify signal/noise differences.\n";
      return 1;
   }

   for ( int i = 1; i < argc; i++ )
      AnalyzeFile( argv[i] );

   return 0;
}
