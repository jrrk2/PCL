//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Translation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Translation_h
#define __PCL_Translation_h

/// \file pcl/Translation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Translation
 * \brief %Image translation algorithm.
 *
 * ### TODO: Write a detailed description for %Translation
 */
class PCL_CLASS Translation : public InterpolatingGeometricTransformation,
                              public ParallelProcess
{
public:

   /*!
    * Constructs a %Translation object using the specified pixel interpolation
    * \a p, with translation increments \a dx and \a dy in pixels.
    *
    * The specified pixel interpolation object \a p must remain valid while
    * this object exists.
    */
   Translation( PixelInterpolation& p, double dx = 0, double dy = 0 )
      : InterpolatingGeometricTransformation( p )
      , m_delta( dx, dy )
   {
   }

   /*!
    * Constructs a %Translation object using the specified pixel interpolation
    * \a p, with translation increments in pixels given by the components of a
    * point \a d in the plane.
    *
    * The specified pixel interpolation object \a p must remain valid while
    * this object exists.
    */
   Translation( PixelInterpolation& p, const DPoint& d )
      : InterpolatingGeometricTransformation( p )
      , m_delta( d )
   {
   }

   /*!
    * Copy constructor.
    */
   Translation( const Translation& ) = default;

   /*!
    * Returns the translation increments in pixels for both axes, as the
    * corresponding components of a point in the plane.
    */
   const DPoint& Delta() const
   {
      return m_delta;
   }

   /*!
    * Returns the horizontal translation increment in pixels.
    */
   double DeltaX() const
   {
      return m_delta.x;
   }

   /*!
    * Returns the vertical translation increment in pixels.
    */
   double DeltaY() const
   {
      return m_delta.y;
   }

   /*!
    * Sets the translation increments in pixels as the components of a point
    * \a d in the plane.
    */
   void SetDelta( const DPoint& d )
   {
      m_delta = d;
   }

   /*!
    * Sets the translation increments \a dx and \a dy in pixels.
    */
   void SetDelta( double dx, double dy )
   {
      m_delta.x = dx; m_delta.y = dy;
   }

   /*!
    * Returns the current vector of per-channel filling values for uncovered
    * image regions.
    *
    * See the documentation for SetFillValues() for more information.
    */
   const DVector& FillValues() const
   {
      return m_fillValues;
   }

   /*!
    * Sets a vector of per-channel filling values for uncovered image regions.
    *
    * Uncovered regions result each time an image is translated. Note that
    * uncovering can be \e partial for one row or column of pixels, when
    * fractional displacements are applied in the corresponding plane
    * directions.
    *
    * By default, there are no filling values defined (and hence the returned
    * vector is empty by default). When the %Translation instance is executed
    * and a filling value is not defined for a channel of the target image,
    * uncovered regions are filled with the minimum sample value in the native
    * range of the image (usually zero).
    */
   void SetFillValues( const DVector& fillValues )
   {
      m_fillValues = fillValues;
   }

   /*!
    */
   void GetNewSizes( int& width, int& height ) const override
   {
      // No changes
   }

   /*!
    */
   bool SupportsGammaCorrection() const override
   {
      return true;
   }

protected:

   DPoint  m_delta; // displacements in pixels
   DVector m_fillValues;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Translation_h

// ----------------------------------------------------------------------------
// EOF pcl/Translation.h - Released 2025-04-07T08:52:44Z
