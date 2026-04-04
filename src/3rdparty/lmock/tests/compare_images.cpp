// compare_images.cpp -- Compare two images channel-by-channel
// Usage: compare_images <image_a.xisf|fits> <image_b.xisf|fits>

#include <pcl/XISF.h>
#include <pcl/MetaModule.h>
#include <pcl/api/APIInterface.h>
#include "FITS/FITS.h"
#include "../PCLMockAPI.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace pcl;

// MetaModule stub
class CompareModule : public MetaModule
{
public:
   CompareModule() : MetaModule() {}
   const char* Version() const override { return "1.0.0"; }
   IsoString Name() const override { return "compare_images"; }
   String Description() const override { return "Image Comparison"; }
   String Company() const override { return String(); }
   String Author() const override { return "test"; }
   String Copyright() const override { return String(); }
   String TradeMarks() const override { return String(); }
   String OriginalFileName() const override { return "compare_images"; }
   void GetReleaseDate( int& y, int& m, int& d ) const override { y=2026; m=4; d=4; }
};

extern "C" void* IdentifyPixInsightModule()
{
   static CompareModule module;
   return &module;
}

static bool EndsWithCI( const std::string& s, const char* suffix )
{
   size_t slen = s.size(), xlen = strlen( suffix );
   if ( slen < xlen ) return false;
   for ( size_t i = 0; i < xlen; ++i )
      if ( tolower( s[slen - xlen + i] ) != tolower( suffix[i] ) )
         return false;
   return true;
}

static double Median( std::vector<double>& v )
{
   if ( v.empty() ) return 0;
   size_t mid = v.size() / 2;
   std::nth_element( v.begin(), v.begin() + mid, v.end() );
   return v[mid];
}

struct ChannelStats
{
   double mean, median, stddev, min, max;
   double p01, p99; // 1st and 99th percentiles
};

static ChannelStats ComputeStats( const Image& img, int channel )
{
   int w = img.Width(), h = img.Height();
   int n = w * h;
   std::vector<double> vals( n );

   double sum = 0;
   double vmin = 1e30, vmax = -1e30;
   for ( int i = 0; i < n; ++i )
   {
      double v = img.Pixel( i % w, i / w, channel );
      vals[i] = v;
      sum += v;
      if ( v < vmin ) vmin = v;
      if ( v > vmax ) vmax = v;
   }

   ChannelStats s;
   s.mean = sum / n;
   s.min = vmin;
   s.max = vmax;

   // Median
   std::vector<double> sorted = vals;
   s.median = Median( sorted );

   // Stddev
   double varsum = 0;
   for ( double v : vals )
      varsum += ( v - s.mean ) * ( v - s.mean );
   s.stddev = std::sqrt( varsum / n );

   // Percentiles
   std::sort( vals.begin(), vals.end() );
   s.p01 = vals[size_t( n * 0.01 )];
   s.p99 = vals[size_t( n * 0.99 )];

   return s;
}

static Image LoadImage( const std::string& path )
{
   Image image;
   String pclPath = String::UTF8ToUTF16( path.c_str() );
   bool isFITS = EndsWithCI( path, ".fits" ) || EndsWithCI( path, ".fit" );

   if ( isFITS )
   {
      FITSReader reader;
      reader.Open( pclPath );
      reader.SetIndex( 0 );
      reader.ReadImage( image );
      reader.Close();
   }
   else
   {
      XISFReader reader;
      reader.Open( pclPath );
      reader.SelectImage( 0 );
      reader.ReadImage( image );
      reader.Close();
   }
   return image;
}

static void PrintStats( const char* label, const ChannelStats& s )
{
   std::cout << "  " << std::left << std::setw(6) << label
             << " mean=" << std::setprecision(6) << std::setw(12) << s.mean
             << " median=" << std::setw(12) << s.median
             << " stddev=" << std::setw(12) << s.stddev
             << " [" << std::setprecision(4) << s.p01 << " .. " << s.p99 << "]"
             << "\n";
}

int main( int argc, char** argv )
{
   if ( argc < 3 )
   {
      std::cerr << "Usage: compare_images <image_a> <image_b>\n";
      return 1;
   }

   // Initialize Mock PCL API
   {
      void* Module = IdentifyPixInsightModule();
      InitializePixInsightModule( (api_handle)Module, GetMockFunctionResolver(),
                                  PCL_API_Version, nullptr );
   }

   try
   {
      std::string pathA = argv[1];
      std::string pathB = argv[2];

      std::cout << "Loading A: " << pathA << "\n";
      Image imgA = LoadImage( pathA );
      std::cout << "  Dimensions: " << imgA.Width() << " x " << imgA.Height()
                << " x " << imgA.NumberOfChannels() << "\n";

      std::cout << "Loading B: " << pathB << "\n";
      Image imgB = LoadImage( pathB );
      std::cout << "  Dimensions: " << imgB.Width() << " x " << imgB.Height()
                << " x " << imgB.NumberOfChannels() << "\n\n";

      int chA = std::min( imgA.NumberOfChannels(), 3 );
      int chB = std::min( imgB.NumberOfChannels(), 3 );
      const char* chanNames[] = { "Red", "Green", "Blue" };

      std::cout << "=== Image A Statistics ===\n";
      ChannelStats statsA[3];
      for ( int c = 0; c < chA; ++c )
      {
         statsA[c] = ComputeStats( imgA, c );
         PrintStats( chanNames[c], statsA[c] );
      }

      std::cout << "\n=== Image B Statistics ===\n";
      ChannelStats statsB[3];
      for ( int c = 0; c < chB; ++c )
      {
         statsB[c] = ComputeStats( imgB, c );
         PrintStats( chanNames[c], statsB[c] );
      }

      // If same dimensions, compute per-pixel difference
      if ( imgA.Width() == imgB.Width() && imgA.Height() == imgB.Height() &&
           imgA.NumberOfChannels() == imgB.NumberOfChannels() )
      {
         int w = imgA.Width(), h = imgA.Height();
         int ch = std::min( imgA.NumberOfChannels(), 3 );
         int n = w * h;

         std::cout << "\n=== Pixel Difference (A - B) ===\n";
         for ( int c = 0; c < ch; ++c )
         {
            double sumDiff = 0, sumAbsDiff = 0, maxAbsDiff = 0;
            for ( int i = 0; i < n; ++i )
            {
               double va = imgA.Pixel( i % w, i / w, c );
               double vb = imgB.Pixel( i % w, i / w, c );
               double d = va - vb;
               sumDiff += d;
               sumAbsDiff += std::abs( d );
               if ( std::abs( d ) > maxAbsDiff ) maxAbsDiff = std::abs( d );
            }
            std::cout << "  " << std::left << std::setw(6) << chanNames[c]
                      << " meanDiff=" << std::setprecision(6) << std::setw(12) << sumDiff / n
                      << " MAE=" << std::setw(12) << sumAbsDiff / n
                      << " maxAbsDiff=" << maxAbsDiff << "\n";
         }

         // Color balance comparison: ratio of channel means
         std::cout << "\n=== Color Balance (mean ratios relative to Green) ===\n";
         if ( chA >= 3 && chB >= 3 )
         {
            std::cout << "  Image A:  R/G=" << std::setprecision(4) << statsA[0].mean / statsA[1].mean
                      << "  B/G=" << statsA[2].mean / statsA[1].mean << "\n";
            std::cout << "  Image B:  R/G=" << std::setprecision(4) << statsB[0].mean / statsB[1].mean
                      << "  B/G=" << statsB[2].mean / statsB[1].mean << "\n";
         }
      }
      else
      {
         std::cout << "\nImages have different dimensions - skipping pixel comparison.\n";

         // Still show color balance
         std::cout << "\n=== Color Balance (mean ratios relative to Green) ===\n";
         if ( chA >= 3 && chB >= 3 )
         {
            std::cout << "  Image A:  R/G=" << std::setprecision(4) << statsA[0].mean / statsA[1].mean
                      << "  B/G=" << statsA[2].mean / statsA[1].mean << "\n";
            std::cout << "  Image B:  R/G=" << std::setprecision(4) << statsB[0].mean / statsB[1].mean
                      << "  B/G=" << statsB[2].mean / statsB[1].mean << "\n";
         }
      }
   }
   catch ( const Exception& e )
   {
      std::cerr << "ERROR: " << e.Message().ToUTF8() << "\n";
      return 1;
   }

   return 0;
}
