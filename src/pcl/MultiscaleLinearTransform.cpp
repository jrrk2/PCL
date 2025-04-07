//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MultiscaleLinearTransform.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/AutoLock.h>
#include <pcl/AutoPointer.h>
#include <pcl/Convolution.h>
#include <pcl/Exception.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MeanFilter.h>
#include <pcl/MultiscaleLinearTransform.h>
#include <pcl/SeparableConvolution.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Transform (decomposition)
 */

class MLTDecomposition
{
public:

   template <class P> static
   void Apply( const GenericImage<P>& image, MultiscaleLinearTransform& T )
   {
      bool statusInitialized = false;
      StatusMonitor& status = (StatusMonitor&)image.Status();
      try
      {
         if ( status.IsInitializationEnabled() )
         {
            status.Initialize( "Multiscale linear transform", image.NumberOfSelectedSamples()*T.m_numberOfLayers );
            status.DisableInitialization();
            statusInitialized = true;
         }

         GenericImage<P> cj0( image );
         cj0.Status().Clear();

         for ( int j = 1, j0 = 0; ; ++j, ++j0 )
         {
            GenericImage<P> cj( cj0 );
            cj.Status() = status;

            LinearFilterLayer( cj, T.FilterSize( j0 ), T.m_useMeanFilters, T.m_parallel, T.m_maxProcessors );

            status = cj.Status();
            cj.Status().Clear();

            if ( T.m_layerEnabled[j0] )
            {
               cj0 -= cj;
               T.m_transform[j0] = Image( cj0 );
            }

            if ( j == T.m_numberOfLayers )
            {
               if ( T.m_layerEnabled[j] )
                  T.m_transform[j] = Image( cj );
               break;
            }

            cj0 = cj;
         }

         if ( statusInitialized )
            status.EnableInitialization();
      }
      catch ( ... )
      {
         T.DestroyLayers();
         if ( statusInitialized )
            status.EnableInitialization();
         throw;
      }
   }

private:

   template <class P> static
   void LinearFilterLayer( GenericImage<P>& cj, int n, bool meanFilter, bool parallel, int maxProcessors )
   {
      AutoPointer<KernelFilter> H( meanFilter ?
         static_cast<KernelFilter*>( new MeanFilter( n ) ) :
         static_cast<KernelFilter*>( new GaussianFilter( n ) ) );

      if ( n >= SeparableConvolution::FasterThanNonseparableFilterSize( cj.Width(), cj.Height() ) )
      {
         SeparableFilter S( H->AsSeparableFilter() );
         SeparableConvolution C( S );
         C.EnableParallelProcessing( parallel, maxProcessors );
         C >> cj;
      }
      else
      {
         Convolution C( *H );
         C.EnableParallelProcessing( parallel, maxProcessors );
         C >> cj;
      }
   }
};

#define TRANSFORM_ENTRY             \
   DestroyLayers();                 \
   if ( image.IsEmptySelection() )  \
      return;                       \

void MultiscaleLinearTransform::Transform( const Image& image )
{
   TRANSFORM_ENTRY
   MLTDecomposition::Apply( image, *this );
}

void MultiscaleLinearTransform::Transform( const DImage& image )
{
   TRANSFORM_ENTRY
   MLTDecomposition::Apply( image, *this );
}

void MultiscaleLinearTransform::Transform( const ComplexImage& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const DComplexImage& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const UInt8Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const UInt16Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const UInt32Image& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

#undef TRANSFORM_ENTRY

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MultiscaleLinearTransform.cpp - Released 2025-04-07T08:53:32Z
