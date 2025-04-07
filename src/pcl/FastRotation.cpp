//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/FastRotation.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/FastRotation.h>

// ----------------------------------------------------------------------------

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_FastRotationEngine
{
public:

   template <class P> static
   void Rotate180( GenericImage<P>& image )
   {
      image.EnsureUnique();

      size_type N = image.NumberOfPixels();
      int n = image.NumberOfChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Rotate 180 degrees", n*N );

      for ( int c = 0; c < n; ++c, image.Status() += N )
         for ( int y0 = 0, y1 = image.Height()-1; y0 <= y1; ++y0, --y1 )
         {
            typename P::sample* __restrict__ f0 = image.ScanLine( y0, c );
            typename P::sample* __restrict__ f1 = image.ScanLine( y1, c );

            if ( y0 != y1 )
            {
               int x0 = 0, x1 = image.Width() - 1;
               PCL_IVDEP
               PCL_UNROLL( 16 )
               while ( x0 < x1 )
               {
                  pcl::Swap( f0[x0], f1[x1] );
                  pcl::Swap( f0[x1], f1[x0] );
                  ++x0;
                  --x1;
               }
               if ( x0 == x1 )
                  pcl::Swap( f0[x0], f1[x0] );
            }
            else
            {
               typename P::sample* __restrict__ f = f0;
               typename P::sample* __restrict__ g = f0 + image.Width() - 1;
               PCL_IVDEP
               PCL_UNROLL( 24 )
               while ( f < g )
                  pcl::Swap( *f++, *g-- );
            }
         }
   }

   template <class P> static
   void Rotate90CW( GenericImage<P>& image )
   {
      image.EnsureUnique();

      int w = image.Width();
      int h = image.Height();
      int h1 = h - 1;
      int n = image.NumberOfChannels();
      size_type N = image.NumberOfPixels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();
      StatusMonitor status = image.Status();

      typename P::sample** f0 = nullptr;

      try
      {
         if ( image.Status().IsInitializationEnabled() )
            status.Initialize( "Rotate 90 degrees, clockwise", n*N );

         f0 = image.ReleaseData();

         typename GenericImage<P>::sample_array tmp( N );

         for ( int c = 0; c < n; ++c, status += N )
         {
            typename P::sample* __restrict__ f = f0[c];
            typename P::sample* __restrict__ t = tmp.Begin();
            ::memcpy( t, f, N*P::BytesPerSample() );
            for ( int y = 0; y < h; ++y )
               for ( int x = 0, h1y = h1 - y; x < w; ++x, ++t )
                  f[x*h + h1y] = *t;
         }

         image.ImportData( f0, h, w, n, cs0 ).Status() = status;
      }
      catch ( ... )
      {
         if ( f0 != nullptr )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != nullptr )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
            image.FreeData();
         }
         throw;
      }
   }

   template <class P> static
   void Rotate90CCW( GenericImage<P>& image )
   {
      image.EnsureUnique();

      int w = image.Width();
      int h = image.Height();
      int w1 = w - 1;
      int n = image.NumberOfChannels();
      size_type N = image.NumberOfPixels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();
      StatusMonitor status = image.Status();

      typename P::sample** f0 = nullptr;

      try
      {
         if ( image.Status().IsInitializationEnabled() )
            status.Initialize( "Rotate 90 degrees, counter-clockwise", n*N );

         typename GenericImage<P>::sample_array tmp( N );

         f0 = image.ReleaseData();

         for ( int c = 0; c < n; ++c, status += N )
         {
            typename P::sample* __restrict__ f = f0[c];
            typename P::sample* __restrict__ t = tmp.Begin();
            ::memcpy( t, f, N*P::BytesPerSample() );
            for ( int y = 0; y < h; ++y )
               for ( int x = 0; x < w; ++x, ++t )
                  f[(w1 - x)*h + y] = *t;
         }

         image.ImportData( f0, h, w, n, cs0 ).Status() = status;
      }
      catch ( ... )
      {
         if ( f0 != nullptr )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != nullptr )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
            image.FreeData();
         }
         throw;
      }
   }

   template <class P> static
   void HorizontalMirror( GenericImage<P>& image )
   {
      image.EnsureUnique();

      size_type N = image.NumberOfPixels();
      int n = image.NumberOfChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Horizontal mirror", n*N );

      for ( int c = 0; c < n; ++c, image.Status() += N )
         for ( int y = 0; y < image.Height(); ++y )
         {
            typename P::sample* __restrict__ f = image.ScanLine( y, c );
            typename P::sample* __restrict__ g = f + image.Width() - 1;
            PCL_IVDEP
            PCL_UNROLL( 24 )
            while ( f < g )
               pcl::Swap( *f++, *g-- );
         }
   }

   template <class P> static
   void VerticalMirror( GenericImage<P>& image )
   {
      image.EnsureUnique();

      size_type N = image.NumberOfPixels();
      int n = image.NumberOfChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Vertical mirror", n*N );

      for ( int c = 0; c < n; ++c, image.Status() += N )
      {
         PCL_IVDEP
         for ( int y0 = 0, y1 = image.Height()-1; y0 < y1; ++y0, --y1 )
         {
            typename P::sample* __restrict__ f0 = image.ScanLine( y0, c );
            typename P::sample* __restrict__ f1 = image.ScanLine( y1, c );
            PCL_IVDEP
            PCL_UNROLL( 24 )
            for ( int x0 = 0, x1 = image.Width(); x0 < x1; ++x0 )
               pcl::Swap( *f0++, *f1++ );
         }
      }
   }
};

// ----------------------------------------------------------------------------

void Rotate180::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

// ----------------------------------------------------------------------------

void Rotate90CW::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

// ----------------------------------------------------------------------------

void Rotate90CCW::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

// ----------------------------------------------------------------------------

void HorizontalMirror::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

// ----------------------------------------------------------------------------

void VerticalMirror::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FastRotation.cpp - Released 2025-04-07T08:53:32Z
