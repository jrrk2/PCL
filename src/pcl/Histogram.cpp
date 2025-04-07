//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Histogram.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/Histogram.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_HistogramEngine
{
public:

   static void Compute( Histogram& H, const ImageVariant& image, int maxThreads = 0 )
   {
      H.Allocate();
      H.m_histogram = 0;

      Rect r = H.m_rect;
      if ( !r.IsRect() )
         r = image.SelectedRectangle();
      else if ( !image.Clip( r ) )
         return;
      int width = r.Width();
      int height = r.Height();

      if ( H.m_channel < 0 )
         H.m_channel = image.SelectedChannel();
      else if ( !image.IsValidChannelIndex( H.m_channel ) )
         return;

      size_type N = size_type( width ) * size_type( height );
      if ( N == 0 )
         return;
      size_type N1 = N >> 1;
      size_type N2 = N - N1;

      if ( image->Status().IsInitializationEnabled() )
         image->Status().Initialize( "Histogram generation", N );

      double min = 0, max = 1;
      if ( image.IsFloatSample() )
      {
         Array<size_type> L1;
         {
            Thread::PerformanceAnalysisData data;
            data.algorithm = PerformanceAnalysisAlgorithm::MinMax;
            data.length = N;
            data.overheadLimit = 32768;
            data.itemSize = image.BytesPerSample();
            data.floatingPoint = image.IsFloatSample();
            L1 = Thread::OptimalThreadLoads( height,
                                             height/Thread::OptimalNumberOfThreads( data )/*overheadLimit*/,
                                             H.m_parallel ? H.m_maxProcessors : 1 );
         }
         bool useAffinity = L1.Length() > 1 && Thread::IsRootThread();
         ReferenceArray<RealMinMaxThread> threads;
         for ( int i = 0, n = 0; i < int( L1.Length() ); n += int( L1[i++] ) )
            threads.Add( new RealMinMaxThread( image, r, H.m_channel, n, n + int( L1[i] ) ) );

         if ( threads.Length() > 1 )
         {
            int n = 0;
            for ( RealMinMaxThread& thread : threads )
               thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
            for ( RealMinMaxThread& thread : threads )
               thread.Wait();
         }
         else
            threads[0].Run();

         min = threads[0].min;
         max = threads[0].max;
         for ( int i = 1; i < int( L1.Length() ); ++i )
         {
            if ( threads[i].min < min )
               min = threads[i].min;
            if ( max < threads[i].max )
               max = threads[i].max;
         }

         threads.Destroy();

         if ( 1 + (max - min) == 1 )
         {
            image->Status() += N1 + N2;
            return;
         }
      }

      image->Status() += N1;

      Array<size_type> L2;
      if ( maxThreads <= 0 )
      {
         Thread::PerformanceAnalysisData data;
         data.algorithm = PerformanceAnalysisAlgorithm::HistogramGeneration;
         data.length = N;
         data.overheadLimit = 32768;
         data.itemSize = image.BytesPerSample();
         data.floatingPoint = image.IsFloatSample();
         L2 = Thread::OptimalThreadLoads( height,
                                          height/Thread::OptimalNumberOfThreads( data )/*overheadLimit*/,
                                          H.m_parallel ? H.m_maxProcessors : 1 );
      }
      else
      {
         // Performance analysis
         L2 = Thread::OptimalThreadLoads( height, 1/*overheadLimit*/, maxThreads );
      }

      bool useAffinity = L2.Length() > 1 && Thread::IsRootThread();
      ReferenceArray<HistogramThread> threads;
      for ( int i = 0, n = 0; i < int( L2.Length() ); n += int( L2[i++] ) )
         threads.Add( new HistogramThread( image, r, H.m_channel, H.m_histogram.Length(), min, max, n, n + int( L2[i] ) ) );
      if ( L2.Length() > 1 )
      {
         int n = 0;
         for ( HistogramThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( HistogramThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      for ( int i = 0; i < int( L2.Length() ); ++i )
         H.m_histogram += threads[i].histogram;

      threads.Destroy();

      image->Status() += N2;

      H.UpdatePeakLevel();
   }

private:

   class RealMinMaxThread : public Thread
   {
   public:

      double min, max;

      RealMinMaxThread( const ImageVariant& image, const Rect& rect, int channel, int startRow, int endRow )
         : m_image( image )
         , m_rect( rect )
         , m_channel( channel )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
      }

      void Run() override
      {
         if ( m_image.IsFloatSample() )
            switch ( m_image.BitsPerSample() )
            {
            case 32 : RealMinMax( static_cast<const Image&>( *m_image ), m_rect, m_channel, min, max, m_startRow, m_endRow ); break;
            case 64 : RealMinMax( static_cast<const DImage&>( *m_image ), m_rect, m_channel, min, max, m_startRow, m_endRow ); break;
            }
      }

   private:

      const ImageVariant& m_image;
      const Rect&         m_rect;
            int           m_channel;
            int           m_startRow, m_endRow;

      template <class P> static
      void RealMinMax( const GenericImage<P>& image, const Rect& r, int c, double& min, double& max, int y0, int y1 )
      {
         int w = r.Width();
         min = max = image( r.LeftTop(), c );
         for ( int y = r.y0+y0, y01 = r.y0+y1; y < y01; ++y )
         {
            const typename P::sample* f  = image.ScanLine( y, c ) + r.x0;
            const typename P::sample* fw = f + w;
            do
            {
               if ( *f < min )
                  min = *f;
               else if ( max < *f )
                  max = *f;
            }
            while ( ++f < fw );
         }
      }
   };

   class HistogramThread : public Thread
   {
   public:

      Histogram::histogram_type histogram;

      HistogramThread( const ImageVariant& image, const Rect& rect, int channel,
                       int resolution, double min, double max, int startRow, int endRow )
         : histogram( 0, resolution )
         , m_image( image )
         , m_rect( rect )
         , m_channel( channel )
         , m_min( min )
         , m_max( max )
         , m_startRow( startRow )
         , m_endRow( endRow )
      {
      }

      void Run() override
      {
         if ( m_image.IsFloatSample() )
            switch ( m_image.BitsPerSample() )
            {
            case 32: RealHistogram( static_cast<const Image&>( *m_image ), m_rect, m_channel, histogram, m_min, m_max, m_startRow, m_endRow );
               break;
            case 64: RealHistogram( static_cast<const DImage&>( *m_image ), m_rect, m_channel, histogram, m_min, m_max, m_startRow, m_endRow );
               break;
            }
         else
            switch ( m_image.BitsPerSample() )
            {
            case  8: IntegerHistogram( static_cast<const UInt8Image&>( *m_image ), m_rect, m_channel, histogram, m_startRow, m_endRow );
               break;
            case 16: IntegerHistogram( static_cast<const UInt16Image&>( *m_image ), m_rect, m_channel, histogram, m_startRow, m_endRow );
               break;
            case 32: IntegerHistogram( static_cast<const UInt32Image&>( *m_image ), m_rect, m_channel, histogram, m_startRow, m_endRow );
               break;
            }
      }

   private:

      const ImageVariant& m_image;
      const Rect&         m_rect;
            int           m_channel;
            double        m_min, m_max;
            int           m_startRow, m_endRow;

      template <class P> static
      void IntegerHistogram( const GenericImage<P>& image,
                             const Rect& r, int c, Histogram::histogram_type& histogram, int y0, int y1 )
      {
         if ( size_type( P::MaxSampleValue() ) == size_type( histogram.Length()-1 ) )
         {
            // The image and the histogram use the same sample range (e.g. UInt16Image and a 16-bit histogram).
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[int( *i )];
         }
         else
         {
            // The image and the histogram use different sample ranges.
            double k = double( histogram.Length()-1 )/P::MaxSampleValue();
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[pcl::RoundInt( *i * k )];
         }
      }

      template <class P> static
      void RealHistogram( const GenericImage<P>& image,
                          const Rect& r, int c, Histogram::histogram_type& histogram, double min, double max, int y0, int y1 )
      {
         if ( min >= 0 && min <= 1 && max >= 0 && max <= 1 )
         {
            // Normalized real image.
            int k = histogram.Length() - 1;
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[pcl::RoundInt( *i * k )];
         }
         else
         {
            // Unnormalized real image.
            double k = (histogram.Length() - 1)/(max - min);
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[pcl::RoundInt( k*(*i - min) )];
         }
      }
   };
};

// ----------------------------------------------------------------------------

const pcl::Image& Histogram::operator <<( const pcl::Image& image )
{
   ImageVariant v( const_cast<pcl::Image*>( &image ) );
   PCL_HistogramEngine::Compute( *this, v );
   return image;
}

const pcl::DImage& Histogram::operator <<( const pcl::DImage& image )
{
   ImageVariant v( const_cast<pcl::DImage*>( &image ) );
   PCL_HistogramEngine::Compute( *this, v );
   return image;
}

const pcl::UInt8Image& Histogram::operator <<( const pcl::UInt8Image& image )
{
   ImageVariant v( const_cast<pcl::UInt8Image*>( &image ) );
   PCL_HistogramEngine::Compute( *this, v );
   return image;
}

const pcl::UInt16Image& Histogram::operator <<( const pcl::UInt16Image& image )
{
   ImageVariant v( const_cast<pcl::UInt16Image*>( &image ) );
   PCL_HistogramEngine::Compute( *this, v );
   return image;
}

const pcl::UInt32Image& Histogram::operator <<( const pcl::UInt32Image& image )
{
   ImageVariant v( const_cast<pcl::UInt32Image*>( &image ) );
   PCL_HistogramEngine::Compute( *this, v );
   return image;
}

const pcl::ImageVariant& Histogram::operator <<( const pcl::ImageVariant& v )
{
   PCL_HistogramEngine::Compute( *this, v );
   return v;
}

// ----------------------------------------------------------------------------

/*
 * Performance analysis
 */
void PCL_PA_HistogramGeneration_F32( const Image& image, Histogram& H, int maxThreads )
{
   ImageVariant v( const_cast<pcl::Image*>( &image ) );
   PCL_HistogramEngine::Compute( H, v, maxThreads );
}
void PCL_PA_HistogramGeneration_F64( const DImage& image, Histogram& H, int maxThreads )
{
   ImageVariant v( const_cast<pcl::DImage*>( &image ) );
   PCL_HistogramEngine::Compute( H, v, maxThreads );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Histogram.cpp - Released 2025-04-07T08:53:32Z
