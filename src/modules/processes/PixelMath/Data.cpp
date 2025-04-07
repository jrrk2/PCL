//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// Data.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "Data.h"
#include "ImageCache.h"
#include "PixelMathInstance.h"

#include <pcl/AstrometricMetadata.h>
#include <pcl/Image.h>
#include <pcl/ImageWindow.h>
#include <pcl/PixelInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ReferencedWindow
{
public:

   ReferencedWindow( const String& imageId )
      : m_id( imageId )
   {
   }

   ReferencedWindow( const ReferencedWindow& ) = default;

   void Attach() const
   {
      ++m_refCount;
   }

   void Detach() const
   {
      if ( --m_refCount == 0 )
         if ( m_locked )
         {
            View view = Window().MainView();
            if ( !view.CanWrite() )
               view.UnlockForWrite();
            m_locked = false;
         }
   }

   bool IsGarbage() const
   {
      return m_refCount <= 0;
   }

   ImageWindow Window() const
   {
      return ImageWindow::WindowById( m_id );
   }

   ImageVariant* NewImage() const
   {
      ImageWindow window = Window();
      if ( window.IsNull() ) // ?!
         return nullptr;

      View view = window.MainView();
      if ( !m_locked )
         if ( view.CanWrite() )
         {
            view.LockForWrite();
            m_locked = true;
         }

      Attach();

      return new ImageVariant( view.Image() );
   }

   bool operator ==( const ReferencedWindow& x ) const
   {
      return m_id == x.m_id;
   }

   bool operator <( const ReferencedWindow& x ) const
   {
      return m_id < x.m_id;
   }

private:

   String       m_id;
   mutable int  m_refCount = 0;
   mutable bool m_locked = false;
};

static SortedArray<ReferencedWindow> s_referencedWindows;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageReference::ImageReference( const String& id, int p, bool isWindow )
   : ObjectReference( XPR_IMAGEREF, p, id )
   , m_isWindow( isWindow )
{
}

// ----------------------------------------------------------------------------

ImageReference::ImageReference( const ImageReference& x )
   : ObjectReference( x )
   , m_isWindow( x.m_isWindow )
   , m_byReference( x.m_byReference )
   , m_astrometryRequired( x.m_astrometryRequired )
   , m_image( (x.m_image != nullptr) ? new ImageVariant( *x.m_image ) : nullptr )
{
   if ( m_isWindow )
   {
      if ( x.m_window )
         m_window = new ImageWindow( *x.m_window.Ptr() );

      if ( m_image != nullptr )
      {
         SortedArray<ReferencedWindow>::const_iterator r = s_referencedWindows.Search( Id() );
         if ( r != s_referencedWindows.End() )
            r->Attach();
      }
   }
}

// ----------------------------------------------------------------------------

template <class P> static
void DestroyInterpolators( void** interpolators, P* )
{
   for ( int i = 0; i < 3; ++i )
      if ( interpolators[i] != nullptr )
         delete reinterpret_cast<PixelInterpolation::Interpolator<P>*>( interpolators[i] ), interpolators[i] = nullptr;
}

static
void DestroyInterpolators( void** interpolators, const ImageVariant& v )
{
   if ( v.IsFloatSample() )
      switch ( v.BitsPerSample() )
      {
      case 32: DestroyInterpolators( interpolators, (FloatPixelTraits*)0 ); break;
      case 64: DestroyInterpolators( interpolators, (DoublePixelTraits*)0 ); break;
      }
   else
      switch ( v.BitsPerSample() )
      {
      case  8: DestroyInterpolators( interpolators, (UInt8PixelTraits*)0 ); break;
      case 16: DestroyInterpolators( interpolators, (UInt16PixelTraits*)0 ); break;
      case 32: DestroyInterpolators( interpolators, (UInt32PixelTraits*)0 ); break;
      }
}

ImageReference::~ImageReference()
{
   if ( m_image != nullptr )
   {
      if ( m_isWindow )
      {
         SortedArray<ReferencedWindow>::const_iterator r = s_referencedWindows.Search( Id() );
         if ( r != s_referencedWindows.End() )
         {
            r->Detach();
            if ( r->IsGarbage() )
               s_referencedWindows.Remove( s_referencedWindows.MutableIterator( r ) );
         }
      }

      DestroyInterpolators( m_interpolators, *m_image );

      delete m_image, m_image = nullptr;
   }
}

// ----------------------------------------------------------------------------

bool ImageReference::FindImage()
{
   delete m_image, m_image = nullptr;

   if ( m_isWindow )
   {
      SortedArray<ReferencedWindow>::const_iterator r = s_referencedWindows.Search( Id() );
      if ( r == s_referencedWindows.End() )
      {
         s_referencedWindows << Id();
         r = s_referencedWindows.Search( Id() );
      }
      m_window = new ImageWindow( r->Window() );
      m_image = r->NewImage();
   }

   return m_image != nullptr;
}

// ----------------------------------------------------------------------------

bool ImageReference::HasAstrometricSolution() const
{
   if ( m_window )
      return m_window->HasAstrometricSolution();
   return false; // ?!
}

// ----------------------------------------------------------------------------

template <class P> static
void InitImageInterpolators( void** interpolators, PixelInterpolation* interpolation, const GenericImage<P>& image )
{
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      interpolators[c] = (void*)interpolation->NewInterpolator<P>( image.PixelData( c ), image.Width(), image.Height() );
}

static
void InitImageInterpolators( void** interpolators, PixelInterpolation* interpolation, const ImageVariant* image )
{
   if ( image->IsFloatSample() )
      switch ( image->BitsPerSample() )
      {
      case 32: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::Image&>( **image ) ); break;
      case 64: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::DImage&>( **image ) ); break;
      }
   else
      switch ( image->BitsPerSample() )
      {
      case  8: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::UInt8Image&>( **image ) ); break;
      case 16: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::UInt16Image&>( **image ) ); break;
      case 32: InitImageInterpolators( interpolators, interpolation, static_cast<const pcl::UInt32Image&>( **image ) ); break;
      }
}

void ImageReference::InitInterpolators( PixelInterpolation* interpolation )
{
   InitImageInterpolators( m_interpolators, interpolation, m_image );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool InternalImageReference::FindImage()
{
   if ( m_image != nullptr )
      delete m_image, m_image = nullptr;

   ImageVariant image = TheImageCache->Image( IsoString( Id() ) );
   if ( image )
   {
      m_image = new ImageVariant( image );
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Data.cpp - Released 2025-04-07T08:53:56Z
