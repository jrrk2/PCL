//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/InterlacedTransformation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_InterlacedTransformation_h
#define __PCL_InterlacedTransformation_h

/// \file pcl/InterlacedTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ImageTransformation.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class InterlacedTransformation
 * \brief Interlaced image transformation in the spatial domain.
 *
 * %InterlacedTransformation represents an <em>interlaced image
 * transformation</em> in PCL. An interlaced transformation can be applied to a
 * subset of the pixels in a target image by acting exclusively on selected
 * rows and columns of pixels, distributed at regular intervals.
 *
 * The distance between two consecutive rows or columns of transformed pixels
 * is the <em>interlacing distance</em> that characterizes an interlaced
 * transformation. Intermediate pixels falling between selected rows or columns
 * are ignored during the transformation.
 *
 * In PCL, convolutions in the spatial domain and morphological transformations
 * have been implemented as interlaced transformations. A good example of
 * interlaced image transformation algorithm is the starlet transform (see the
 * StarletTransform class), which uses interlaced convolutions to perform a
 * multiscale decomposition.
 */
class PCL_CLASS InterlacedTransformation : public virtual ImageTransformation
{
public:

   /*!
    * Constructs an %InterlacedTransformation object with the specified
    * <em>interlacing distance</em> \a n > 0 in pixels.
    */
   InterlacedTransformation( int n = 1 )
      : m_distance( pcl::Max( 1, n ) )
   {
      PCL_PRECONDITION( n > 0 )
   }

   /*!
    * Copy constructor.
    */
   InterlacedTransformation( const InterlacedTransformation& ) = default;

   /*!
    * Destroys an %InterlacedTransformation object.
    */
   ~InterlacedTransformation() override
   {
   }

   /*!
    * Returns true iff this transformation is currently interlaced.
    *
    * The transformation is interlaced if the current interlacing distance is
    * greater than one pixel.
    *
    * \sa InterlacingDistance()
    */
   bool IsInterlaced() const
   {
      return m_distance > 1;
   }

   /*!
    * Returns the current interlacing distance of this transformation in
    * pixels.
    *
    * \sa SetInterlacingDistance(), IsInterlaced()
    */
   int InterlacingDistance() const
   {
      return m_distance;
   }

   /*!
    * Sets the current interlacing distance of this transformation to the
    * specified value \a n > 0 in pixels.
    *
    * The interlacing distance is the distance in pixels between two
    * consecutive rows or columns of transformed pixels. Intermediate pixels
    * falling between selected rows or columns are ignored.
    *
    * \sa InterlacingDistance(), DisableInterlacing(), IsInterlaced()
    */
   void SetInterlacingDistance( int n )
   {
      PCL_PRECONDITION( n > 0 )
      m_distance = pcl::Max( 1, n );
   }

   /*!
    * Disables interlacing for this transformation.
    *
    * This is a convenience function, equivalent to:
    *
    * \code SetInterlacingDistance( 1 ); \endcode
    */
   void DisableInterlacing()
   {
      SetInterlacingDistance( 1 );
   }

private:

   /*
    * Interlacing distance in pixels
    */
   int m_distance = 1;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_InterlacedTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/InterlacedTransformation.h - Released 2025-04-07T08:52:44Z
