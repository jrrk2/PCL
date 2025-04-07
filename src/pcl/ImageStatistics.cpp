//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageStatistics.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ImageStatistics.h>
#include <pcl/Matrix.h>
#include <pcl/Selection.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_ImageStatisticsEngine
{
public:

   template <class P> static
   void Compute( const GenericImage<P>& image, ImageStatistics::Data& data, bool /*parallel*/, int /*maxProcessors*/ )
   {
      data.AssignStatisticalData( ImageStatistics::Data() );

      size_type N = image.NumberOfSelectedPixels();
      if ( N == 0 )
         return;

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Computing image statistics", N );

      size_type NS = N/8;
      size_type NN = N - 7*NS;

      Rect rect = image.SelectedRectangle();
      int channel = image.SelectedChannel();

      data.minimum = data.maximum = 0;
      data.minPos = data.maxPos = Point( 0 );

      if ( data.rejectLow || data.rejectHigh )
      {
         // Rejection bounds in the native range
         double s0 = 0, s1 = 0;
         if ( data.rejectLow )
            s0 = P::MinSampleValue() + data.low*(P::MaxSampleValue() - P::MinSampleValue());
         if ( data.rejectHigh )
            s1 = P::MinSampleValue() + data.high*(P::MaxSampleValue() - P::MinSampleValue());

         Array<double> v;
         v.Reserve( N ); // clearly, optimize for speed

         typename GenericImage<P>::const_roi_sample_iterator i( image, rect, channel );
         P::FromSample( data.minimum, *i );
         data.maximum = data.minimum;

         if ( data.noExtremes )
         {
            if ( data.rejectLow )
            {
               if ( data.rejectHigh )
               {
                  for ( ; i; ++i )
                     if ( *i > s0 )
                        if ( *i < s1 )
                        {
                           double f; P::FromSample( f, *i );
                           v << f;
                        }
               }
               else
               {
                  for ( ; i; ++i )
                     if ( *i > s0 )
                     {
                        double f; P::FromSample( f, *i );
                        v << f;
                     }
               }
            }
            else
            {
               for ( ; i; ++i )
                  if ( *i < s1 )
                  {
                     double f; P::FromSample( f, *i );
                     v << f;
                  }
            }
         }
         else // !data.noExtremes
         {
            bool extremesSeen = false;
            if ( data.rejectLow )
            {
               if ( data.rejectHigh )
               {
                  for ( int y = rect.y0; y < rect.y1; ++y )
                     for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                        if ( *i > s0 )
                           if ( *i < s1 )
                           {
                              double f; P::FromSample( f, *i );
                              v << f;

                              if ( extremesSeen )
                              {
                                 if ( f < data.minimum )
                                 {
                                    data.minimum = f;
                                    data.minPos.x = x;
                                    data.minPos.y = y;
                                 }
                                 else if ( f > data.maximum )
                                 {
                                    data.maximum = f;
                                    data.maxPos.x = x;
                                    data.maxPos.y = y;
                                 }
                              }
                              else
                              {
                                 data.minimum = data.maximum = f;
                                 data.minPos.x = data.maxPos.x = x;
                                 data.minPos.y = data.maxPos.y = y;
                                 extremesSeen = true;
                              }
                           }
               }
               else
               {
                  for ( int y = rect.y0; y < rect.y1; ++y )
                     for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                        if ( *i > s0 )
                        {
                           double f; P::FromSample( f, *i );
                           v << f;

                           if ( extremesSeen )
                           {
                              if ( f < data.minimum )
                              {
                                 data.minimum = f;
                                 data.minPos.x = x;
                                 data.minPos.y = y;
                              }
                              else if ( f > data.maximum )
                              {
                                 data.maximum = f;
                                 data.maxPos.x = x;
                                 data.maxPos.y = y;
                              }
                           }
                           else
                           {
                              data.minimum = data.maximum = f;
                              data.minPos.x = data.maxPos.x = x;
                              data.minPos.y = data.maxPos.y = y;
                              extremesSeen = true;
                           }
                        }
               }
            }
            else
            {
               for ( int y = rect.y0; y < rect.y1; ++y )
                  for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                     if ( *i < s1 )
                     {
                        double f; P::FromSample( f, *i );
                        v << f;

                        if ( extremesSeen )
                        {
                           if ( f < data.minimum )
                           {
                              data.minimum = f;
                              data.minPos.x = x;
                              data.minPos.y = y;
                           }
                           else if ( f > data.maximum )
                           {
                              data.maximum = f;
                              data.maxPos.x = x;
                              data.maxPos.y = y;
                           }
                        }
                        else
                        {
                           data.minimum = data.maximum = f;
                           data.minPos.x = data.maxPos.x = x;
                           data.minPos.y = data.maxPos.y = y;
                           extremesSeen = true;
                        }
                     }
            }
         }

         data.count = v.Length();

         if ( !data.noSumOfSquares )
            data.sumOfSquares = pcl::SumOfSquares( v.Begin(), v.End() );

         image.Status() += NS;

         if ( !data.noMean )
         {
            data.mean = pcl::Mean( v.Begin(), v.End() );

            image.Status() += NS;

            if ( !data.noVariance )
            {
               data.variance = pcl::Variance( v.Begin(), v.End(), data.mean );
               data.stdDev = Sqrt( data.variance );
            }

            image.Status() += NS;
         }
         else
         {
            image.Status() += 2*NS;
         }

         if ( !data.noMedian )
         {
            data.median = pcl::Median( v.Begin(), v.End() );

            image.Status() += NS;

            if ( !data.noAvgDev )
               data.avgDev = pcl::AvgDev( v.Begin(), v.End(), data.median );

            image.Status() += NS;

            if ( !data.noMAD )
            {
               data.MAD = pcl::MAD( v.Begin(), v.End(), data.median );

               if ( !data.noBWMV )
                  data.bwmv = pcl::BiweightMidvariance( v.Begin(), v.End(), data.median, data.MAD );
            }

            if ( !data.noPBMV )
               data.pbmv = pcl::BendMidvariance( v.Begin(), v.End(), data.median, 0.2 );

            image.Status() += NS;
         }
         else
         {
            image.Status() += 3*NS;
         }

         if ( !data.noSn )
            data.Sn = pcl::Sn( v.Begin(), v.End() );

         image.Status() += NS;

         if ( !data.noQn )
            data.Qn = pcl::Qn( v.Begin(), v.End() );

         image.Status() += NN;
      }
      else
      {
         data.count = N;

         DMatrix V( image, rect, channel );

         if ( !data.noExtremes )
         {
            double* i = V.Begin();
            data.minimum = data.maximum = *i;
            for ( int y = rect.y0; y < rect.y1; ++y )
               for ( int x = rect.x0; x < rect.x1; ++x, ++i )
                  if ( *i < data.minimum )
                  {
                     data.minimum = *i;
                     data.minPos.x = x;
                     data.minPos.y = y;
                  }
                  else if ( *i > data.maximum )
                  {
                     data.maximum = *i;
                     data.maxPos.x = x;
                     data.maxPos.y = y;
                  }
         }

         if ( !data.noSumOfSquares )
            data.sumOfSquares = pcl::SumOfSquares( V.Begin(), V.End() );

         image.Status() += NS;

         if ( !data.noMean )
         {
            data.mean = pcl::Mean( V.Begin(), V.End() );

            image.Status() += NS;

            if ( !data.noVariance )
            {
               data.variance = pcl::Variance( V.Begin(), V.End(), data.mean );
               data.stdDev = Sqrt( data.variance );
            }

            image.Status() += NS;
         }
         else
         {
            image.Status() += 2*NS;
         }

         if ( !data.noMedian )
         {
            data.median = pcl::Median( V.Begin(), V.End() );

            image.Status() += NS;

            if ( !data.noAvgDev )
               data.avgDev = pcl::AvgDev( V.Begin(), V.End(), data.median );

            image.Status() += NS;

            if ( !data.noMAD )
            {
               data.MAD = pcl::MAD( V.Begin(), V.End(), data.median );

               if ( !data.noBWMV )
                  data.bwmv = pcl::BiweightMidvariance( V.Begin(), V.End(), data.median, data.MAD );
            }

            if ( !data.noPBMV )
               data.pbmv = pcl::BendMidvariance( V.Begin(), V.End(), data.median, 0.2 );

            image.Status() += NS;
         }
         else
         {
            image.Status() += 3*NS;
         }

         if ( !data.noSn )
            data.Sn = pcl::Sn( V.Begin(), V.End() );

         image.Status() += NS;

         if ( !data.noQn )
            data.Qn = pcl::Qn( V.Begin(), V.End() );

         image.Status() += NN;
      }
   }
};

// ----------------------------------------------------------------------------

const Image& ImageStatistics::operator <<( const Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const DImage& ImageStatistics::operator <<( const DImage& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const UInt8Image& ImageStatistics::operator <<( const UInt8Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const UInt16Image& ImageStatistics::operator <<( const UInt16Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

const UInt32Image& ImageStatistics::operator <<( const UInt32Image& image )
{
   PCL_ImageStatisticsEngine::Compute( image, m_data, m_parallel, m_maxProcessors );
   return image;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageStatistics.cpp - Released 2025-04-07T08:53:32Z
