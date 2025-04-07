//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/RedundantMultiscaleTransform.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Exception.h>
#include <pcl/RedundantMultiscaleTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Inverse transform (reconstruction)
 */

class MTReconstruction
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const RedundantMultiscaleTransform& T )
   {
      for ( int j = T.m_numberOfLayers; ; --j )
      {
         if ( T.m_transform[j] && T.m_layerEnabled[j] )
         {
            StatusMonitor status = image.Status();
            bool statusInitialized = false;

            image.SetStatusCallback( 0 );

            try
            {
               if ( image.IsEmpty() || image.IsEmptySelection() )
                  image.Assign( T.m_transform[j] );
               else
                  image.Apply( T.m_transform[j] );

               size_type N = image.NumberOfSelectedSamples();

               image.Status() = status;
               if ( image.Status().IsInitializationEnabled() )
               {
                  image.Status().Initialize( "Multiscale reconstruction", T.m_numberOfLayers*N );
                  image.Status().DisableInitialization();
                  statusInitialized = true;
               }

               image.Status() += (T.m_numberOfLayers - j)*N;

               for ( ; --j >= 0; )
                  if ( T.m_transform[j] && T.m_layerEnabled[j] )
                     image += T.m_transform[j];
                  else
                     image.Status() += N;

               if ( statusInitialized )
                  image.Status().EnableInitialization();
            }
            catch ( ... )
            {
               if ( statusInitialized )
                  image.Status().EnableInitialization();
               throw;
            }

            break;
         }

         if ( j == 0 )
         {
            image.Zero();
            break;
         }
      }
   }
}; // MTReconstruction

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void RedundantMultiscaleTransform::Apply( Image& image ) const
{
   MTReconstruction::Apply( image, *this );
}

// ----------------------------------------------------------------------------

void RedundantMultiscaleTransform::Apply( DImage& image ) const
{
   MTReconstruction::Apply( image, *this );
}

// ----------------------------------------------------------------------------

#define IMPLEMENT_RECONSTRUCTION                      \
   F.Status() = image.Status();                       \
   MTReconstruction::Apply( F, *this );               \
   if ( image.IsEmpty() || image.IsEmptySelection() ) \
      image.Assign( F );                              \
   else                                               \
      image.Apply( F ).Status() = F.Status();

void RedundantMultiscaleTransform::Apply( pcl::ComplexImage& image ) const
{
   pcl::Image F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::DComplexImage& image ) const
{
   pcl::DImage F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::UInt8Image& image ) const
{
   pcl::Image F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::UInt16Image& image ) const
{
   pcl::Image F;
   IMPLEMENT_RECONSTRUCTION
}

void RedundantMultiscaleTransform::Apply( pcl::UInt32Image& image ) const
{
   pcl::DImage F;
   IMPLEMENT_RECONSTRUCTION
}

#undef IMPLEMENT_RECONSTRUCTION

// ----------------------------------------------------------------------------

void RedundantMultiscaleTransform::ValidateLayerIndex( int j ) const
{
   if ( j < 0 || j > m_numberOfLayers )
      throw Error( "Invalid access to undefined multiscale transform layer." );
}

// ----------------------------------------------------------------------------

void RedundantMultiscaleTransform::ValidateLayerAccess( int j ) const
{
   ValidateLayerIndex( j );
   if ( m_transform.IsEmpty() || !m_transform[j] )
      throw Error( "Invalid access to nonexistent multiscale transform layer." );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/RedundantMultiscaleTransform.cpp - Released 2025-04-07T08:53:32Z
