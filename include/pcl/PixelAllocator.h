//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PixelAllocator.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_PixelAllocator_h
#define __PCL_PixelAllocator_h

/// \file pcl/PixelAllocator.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/SharedPixelData.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class P> class PCL_CLASS GenericImage;

// ----------------------------------------------------------------------------

/*!
 * \class PixelAllocator
 * \brief Manages transparent allocation and deallocation of shared and local
 *        pixel data.
 *
 * %PixelAllocator is responsible for allocation and deallocation of pixel data
 * blocks in PCL. The template argument P corresponds to an instantiation of
 * GenericPixelTraits for a specific pixel sample type.
 *
 * Unless you are implementing geometrical transformations, or processes that
 * perform direct substitutions of pixel data blocks in images, such as a whole
 * channel, you usually should not have to use the %PixelAllocator template
 * class directly in your code. The SharedPixelData and GenericImage classes
 * can perform all the necessary allocations transparently.
 *
 * When you have to allocate or deallocate pixel data blocks, however, you
 * should \e never use the \c new and \c delete operators, or the \c malloc,
 * \c calloc and \c free standard C functions. You must use %PixelAllocator for
 * all allocations and deallocations of pixel data \e exclusively. Among other
 * good reasons for this, pixel data blocks cannot be allocated in your
 * module's local heap if they are being used with shared images, i.e. images
 * living in the PixInsight core application.
 *
 * The \e only safe way to allocate and deallocate pixel data in PCL is by
 * using %PixelAllocator's member functions. Note that you cannot allocate
 * %PixelAllocator directly. For an existing image, you get a reference to a
 * %PixelAllocator instance by calling the Allocator() member function of
 * %GenericImage. This member function returns a reference to the image's
 * internal allocator, which will perform all pixel allocation tasks that you
 * may need transparently, irrespective of whether the object represents a
 * local or shared image.
 *
 * \sa GenericPixelTraits, GenericImage, SharedPixelData
 */
template <class P>
class PCL_CLASS PixelAllocator : public SharedPixelData
{
public:

   /*!
    * Represents the pixel traits class used by this instantiation of
    * %PixelAllocator.
    *
    * The \c pixel_traits type should be an instantiation of
    * GenericPixelTraits. It identifies a class implementing basic storage and
    * functional primitives optimized for a particular pixel sample type.
    */
   using pixel_traits = P;

   /*!
    * Represents the data type used to store pixel sample values in this
    * template instantiation of %PixelAllocator.
    */
   using sample = typename pixel_traits::sample;

   /*!
    * Returns true iff this allocator and another instance are working for the
    * same shared image, or if both of them are working for local images.
    *
    * All local images share a unique internal allocator, but each shared image
    * has its own, independent allocator object. This responds to the
    * multithreaded nature of the PixInsight core application.
    */
   bool operator ==( const PixelAllocator<P>& x ) const noexcept
   {
      return SharedPixelData::operator ==( x );
   }

   /*!
    * Allocates a contiguous block of memory where at least \a n > 0 pixel
    * samples can be optimally stored.
    *
    * Returns the starting address of the allocated block.
    *
    * This member function throws a \c std::bad_alloc exception if there is not
    * enough memory available to allocate the required contiguous block.
    */
   sample* AllocatePixels( size_type n ) const
   {
      PCL_PRECONDITION( n != 0 )
      return reinterpret_cast<sample*>( SharedPixelData::Allocate( n*sizeof( sample ) ) );
   }

   /*!
    * Allocates a contiguous block of memory where at least \a width *
    * \a height pixel samples can be optimally stored.
    *
    * Returns the starting address of the allocated block. This is a
    * convenience alias for:
    *
    * \code
    * AllocatePixels( size_type( width )*size_type( height ) );
    * \endcode
    */
   sample* AllocatePixels( int width, int height ) const
   {
      return AllocatePixels( size_type( width )*size_type( height ) );
   }

   /*!
    * Allocates an array where at least \a n > 0 <em>channel slots</em> can be
    * optimally stored. A channel slot is a pointer to a contiguous block of
    * pixel samples, where an image can store a single channel.
    *
    * The allocated array is initialized to zero. Returns the starting address
    * of the allocated array.
    *
    * This member function throws a \c std::bad_alloc exception if there is not
    * enough memory available to allocate the required contiguous block.
   */
   sample** AllocateChannelSlots( size_type n ) const
   {
      PCL_PRECONDITION( n != 0 )
      sample** cslots = reinterpret_cast<sample**>( SharedPixelData::Allocate( n*sizeof( sample* ) ) );
      ::memset( cslots, 0, n*sizeof( sample* ) );
      return cslots;
   }

   /*!
    * Deallocates a previously allocated memory block. The deallocated block
    * becomes available for subsequent allocations.
    *
    * \warning <em>Do not</em> use this function to deallocate memory that has
    * not been allocated by \e this %PixelAllocator object. This includes
    * memory blocks allocated by the global \c new operator, or by other
    * %PixelAllocator objects. Failure to follow this rule will lead to severe
    * heap corruption for the calling module.
    */
   template <typename T>
   void Deallocate( T* p ) const
   {
      PCL_PRECONDITION( p != nullptr )
      SharedPixelData::Deallocate( reinterpret_cast<void*>( p ) );
   }

private:

   PixelAllocator() = default;

   PixelAllocator( void* handle )
      : SharedPixelData( handle, P::BitsPerSample(), P::IsFloatSample(), P::IsComplexSample() )
   {
   }

   PixelAllocator( const PixelAllocator<P>& x )
      : SharedPixelData( x )
   {
   }

   PixelAllocator( int width, int height, int numberOfChannels, int colorSpace )
      : SharedPixelData( width, height, numberOfChannels, P::BitsPerSample(), P::IsFloatSample(), colorSpace )
   {
   }

   PixelAllocator& operator =( const PixelAllocator<P>& x )
   {
      (void)SharedPixelData::operator =( x );
      return *this;
   }

   sample** GetSharedData() const
   {
      return reinterpret_cast<sample**>( SharedPixelData::GetSharedData() );
   }

   void SetSharedData( sample** ptrToShared )
   {
      SharedPixelData::SetSharedData( reinterpret_cast<void**>( ptrToShared ) );
   }

   friend class pcl::GenericImage<P>;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PixelAllocator_h

// ----------------------------------------------------------------------------
// EOF pcl/PixelAllocator.h - Released 2025-04-07T08:52:44Z
