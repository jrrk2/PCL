//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SharedPixelData.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_SharedPixelData_h
#define __PCL_SharedPixelData_h

/// \file pcl/SharedPixelData.h

#include <pcl/Defs.h>

#include <pcl/ColorSpace.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SharedPixelData
 * \brief Handles transparent, type-independent allocation of local and shared
 *        pixel data blocks.
 *
 * \internal This is a low-level class internally used by PCL classes and
 * functions. It is not intended to be used directly by user-level modules.
 * Unless you are a PixInsight core or PCL maintainer, you should not need or
 * attempt to use or instantiate this class.
 */
class PCL_CLASS SharedPixelData
{
public:

   using color_space = ColorSpace::value_type;

   /*!
    * Constructs a %SharedPixelData object that represents a local image.
    */
   SharedPixelData() = default;

   /*!
    * References an existing shared image with the specified handle and sample
    * data type.
    *
    * \param handle           Handle to a shared image.
    *
    * \param bitsPerSample    Number of bits per pixel sample.
    *
    * \param floatSample      Whether this image stores floating point pixel
    *                         data, or integer pixel data.
    *
    * \param complexSample    Whether this image stores complex pixel data or
    *                         real pixel data.
    *
    * \note This constructor throws an Error exception if the specified handle
    * does not correspond to an existing shared image, or if the existing
    * shared image does not have the specified sample data type.
    */
   SharedPixelData( void* handle, int bitsPerSample, bool floatSample, bool complexSample );

   /*!
    * Creates a new shared image with the specified geometry, sample data type
    * and color space.
    *
    * \param width            %Image width in pixels.
    *
    * \param height           %Image height in pixels.
    *
    * \param numberOfChannels Number of channels, including nominal channels or
    *                         color components and alpha channels.
    *
    * \param bitsPerSample    Number of bits per pixel sample.
    *
    * \param floatSample      Whether this image stores floating point pixel
    *                         data, or integer pixel data.
    *
    * \param colorSpace       Color space where pixels are represented.
    */
   SharedPixelData( int width, int height, int numberOfChannels,
                    int bitsPerSample, bool floatSample, int colorSpace );

   /*!
    * Copy constructor.
    *
    * If the argument object \a x represents a shared image, this constructor
    * references the same shared image. If \a x is a local image, this function
    * just copies the null handle and has no further effect.
    */
   SharedPixelData( const SharedPixelData& x )
      : m_handle( x.m_handle )
   {
      Attach();
   }

   /*!
    * Virtual destructor.
    *
    * If this object represents a shared image, it is dereferenced. If the
    * shared image becomes unreferenced, it will be garbage-collected and
    * eventually destroyed by the PixInsight core application.
    *
    * If this object represents a local image, this destructor has no effect.
    */
   virtual ~SharedPixelData()
   {
      Detach();
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * If this object represents a shared image, it is dereferenced. If the
    * specified object \a x represents a shared image, then this object will
    * reference it; otherwhise this operator just copies the null handle
    * without further effect.
    */
   SharedPixelData& operator =( const SharedPixelData& x )
   {
      if ( m_handle != x.m_handle )
      {
         Detach();
         m_handle = x.m_handle;
         Attach();
      }
      return *this;
   }

   /*!
    * Returns true iff this object represents the same shared image as another
    * object \a x, or if both objects represent local images.
    */
   bool operator ==( const SharedPixelData& x ) const noexcept
   {
      return m_handle == x.m_handle;
   }

   /*!
    * Returns the handle to the shared image represented by this object, or
    * zero (a null pointer) if this object represents a local image.
    */
   void* Handle() const noexcept
   {
      return m_handle;
   }

   /*!
    * Returns true if this object represents a shared image; false if it
    * represents a local image. Shared images live in the PixInsight core
    * application. Local images live in the calling module.
    */
   bool IsShared() const noexcept
   {
      return m_handle != nullptr;
   }

   /*!
    * Returns true iff the shared image represented by this object is not
    * uniquely referenced, i.e. if there exist other %SharedPixelData objects
    * representing the same shared image. Returns false if the shared image is
    * only referenced by this object, or if this object represents a local
    * image.
    */
   bool IsAliased() const;

   /*!
    * Returns true iff this object is the owner of the represented shared image.
    * Returns false if this object does not own the shared image, or if this
    * object does not represent a shared image.
    */
   bool IsOwner() const;

private:

   void* m_handle = nullptr;

   void Attach();
   void Detach();

protected:

   void* Allocate( size_type sizeInBytes ) const;
   void Deallocate( void* ) const;

   void** GetSharedData() const;
   void SetSharedData( void** ptrToShared );

   void GetSharedGeometry( int& width, int& height, int& numberOfChannels );
   void SetSharedGeometry( int width, int height, int numberOfChannels );

   void GetSharedColor( color_space& colorSpace, RGBColorSystem& RGBWS );
   void SetSharedColor( color_space colorSpace, const RGBColorSystem& RGBWS );

   template <class P> friend class GenericImage;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SharedPixelData_h

// ----------------------------------------------------------------------------
// EOF pcl/SharedPixelData.h - Released 2025-04-07T08:52:44Z
