//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SeparableMedianFilter.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_SeparableMedianFilter_h
#define __PCL_SeparableMedianFilter_h

/// \file pcl/SeparableMedianFilter.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/Matrix.h> // PCL_VALID_KERNEL_SIZE()
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SeparableMedianFilter
 * \brief A two-dimensional separable filter that approximates a median filter.
 *
 * ### TODO: Write a detailed description for %SeparableMedianFilter.
 */
class PCL_CLASS SeparableMedianFilter : public ImageTransformation,
                                        public ParallelProcess
{
public:

   /*!
    * Constructs a %SeparableMedianFilter object with the specified size.
    *
    * \param size    Size of the separable median filter in pixels. Must be an
    *                odd integer &gt;= 3 (3, 5, 7, ...).
    */
   SeparableMedianFilter( int size = 3 )
      : m_size( PCL_VALID_KERNEL_SIZE( size ) )
   {
      PCL_PRECONDITION( size >= 3 && size&1 == 1 )
   }

   /*!
    * Copy constructor.
    */
   SeparableMedianFilter( const SeparableMedianFilter& ) = default;

   /*!
    * Destroys a %SeparableMedianFilter object.
    */
   ~SeparableMedianFilter() override
   {
   }

   /*!
    * Returns the current size in pixels of this separable median filter.
    */
   int Size() const
   {
      return m_size;
   }

   /*!
    * Sets a new \a size in pixels for this separable median filter object. The
    * specified size must be an odd integer &gt; 3 (3, 5, 7, ...).
    */
   void SetSize( int size )
   {
      PCL_PRECONDITION( size >= 3 && size&1 == 1 )
      m_size = PCL_VALID_KERNEL_SIZE( size );
   }

protected:

   int  m_size; // filter size

   /*
    * In-place 2-D separable median approximation filter.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SeparableMedianFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/SeparableMedianFilter.h - Released 2025-04-07T08:52:44Z
