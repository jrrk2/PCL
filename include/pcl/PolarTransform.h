//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PolarTransform.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_PolarTransform_h
#define __PCL_PolarTransform_h

/// \file pcl/PolarTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PolarTransformBase
 * \brief Base class of polar transforms.
 *
 * %PolarTransformBase implements properties and functionality common to polar
 * transform classes in PCL.
 *
 * A polar transform remaps pixel values in a two-dimensional image to encode
 * polar coordinates on both image axes. Polar angle measured counter-clockwise
 * with respect to the geometric center of the image is encoded linearly on the
 * vertical (Y) axis. Radial distance to the center is encoded (linearly or
 * not, depending on each particular transform) on the horizontal (X) axis.
 *
 * Polar transforms have important applications in FFT-based image registration
 * algorithms (e.g. as part of Fourier-Mellin transforms).
 *
 * \sa PolarTransform, LogPolarTransform, FFTRegistrationEngine
 */
class PCL_CLASS PolarTransformBase : public InterpolatingGeometricTransformation,
                                     public ParallelProcess
{
public:

   /*!
    * Constructs a %PolarTransformBase object using the specified \a p pixel
    * interpolation. The referenced interpolation object \a p must remain valid
    * while this %PolarTransformBase object exists.
    */
   PolarTransformBase( PixelInterpolation& p )
      : InterpolatingGeometricTransformation( p )
   {
   }

   /*!
    * Copy constructor.
    */
   PolarTransformBase( const PolarTransformBase& ) = default;

   /*!
    * Returns the initial rotation angle in radians. The default initial
    * rotation angle is zero.
    */
   float InitialRotation() const
   {
      return m_initialAngle;
   }

   /*!
    * Sets the initial rotation \a angle in radians.
    */
   void SetInitialRotation( float angle )
   {
      PCL_PRECONDITION( angle >= 0 && angle <= pcl::Const<float>::_2pi() )
      if ( (m_initialAngle = angle) > m_finalAngle )
         pcl::Swap( m_initialAngle, m_finalAngle );
   }

   /*!
    * Returns the final rotation angle in radians. The default final rotation
    * angle is 2*pi (360 degrees).
    */
   float FinalRotation() const
   {
      return m_finalAngle;
   }

   /*!
    * Sets the final rotation \a angle in radians.
    */
   void SetFinalRotation( float angle )
   {
      PCL_PRECONDITION( angle >= 0 && angle <= pcl::Const<float>::_2pi() )
      if ( (m_finalAngle = angle) < m_initialAngle )
         pcl::Swap( m_initialAngle, m_finalAngle );
   }

   /*!
    * Sets the initial and final rotation angles, \a initialAngle and
    * \a finalAngle respectively, in radians. If the specified initial angle is
    * greater than the final angle, both values are swapped.
    */
   void SetRotationRange( float initialAngle, float finalAngle )
   {
      PCL_PRECONDITION( initialAngle >= 0 && initialAngle <= pcl::Const<float>::_2pi() )
      PCL_PRECONDITION( finalAngle >= 0 && finalAngle <= pcl::Const<float>::_2pi() )
      m_initialAngle = initialAngle;
      m_finalAngle = finalAngle;
      if ( m_finalAngle < m_initialAngle )
         pcl::Swap( m_initialAngle, m_finalAngle );
   }

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

   /*!
    * \internal
    */
   virtual bool IsLogPolar() const
   {
      return false;
   }

protected:

   float m_initialAngle = 0; // in radians
   float m_finalAngle = pcl::Const<float>::_2pi();
};

// ----------------------------------------------------------------------------

/*!
 * \class PolarTransform
 * \brief In-place transformation to polar coordinates.
 *
 * This class implements a Cartesian to polar coordinate transform with the
 * following properties:
 *
 * \li The origin of polar coordinates is located at the geometric center of
 * the image.
 *
 * \li Rotation angles are encoded linearly on the vertical axis, growing from
 * bottom to top.
 *
 * \li Distance to the origin is encoded linearly on the horizontal axis,
 * growing from left to right.
 *
 * \li Transform pixels mapped outside the source image (in Cartesian
 * coordinates) are set to zero.
 *
 * \sa LogPolarTransform, PolarTransformBase, FFTRegistrationEngine
 */
class PCL_CLASS PolarTransform : public PolarTransformBase
{
public:

   /*!
    * Constructs a %PolarTransform object using the specified pixel
    * interpolation \a p. The referenced interpolation object \a p must remain
    * valid while this %PolarTransform object exists.
    */
   PolarTransform( PixelInterpolation& p )
      : PolarTransformBase( p )
   {
   }

   /*!
    * Copy constructor.
    */
   PolarTransform( const PolarTransform& ) = default;

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class LogPolarTransform
 * \brief In-place transformation to log-polar coordinates.
 *
 * This class implements a Cartesian to log-polar coordinate transform with the
 * following properties:
 *
 * \li The origin of polar coordinates is located at the geometric center of
 * the image.
 *
 * \li Rotation angles are encoded linearly on the vertical axis, growing from
 * bottom to top.
 *
 * \li Distance to the origin is encoded logarithmically on the horizontal
 * axis, growing from left to right. Natural (base e) logarithms are used.
 *
 * \li Transform pixels mapped outside the source image (in Cartesian
 * coordinates) are set to zero.
 *
 * \sa PolarTransform, PolarTransformBase, FFTRegistrationEngine
 */
class PCL_CLASS LogPolarTransform : public PolarTransformBase
{
public:

   /*!
    * Constructs a %LogPolarTransform object using the specified pixel
    * interpolation \a p. The referenced interpolation object \a p must remain
    * valid while this %LogPolarTransform object exists.
    */
   LogPolarTransform( PixelInterpolation& p )
      : PolarTransformBase( p )
   {
   }

   /*!
    * Copy constructor.
    */
   LogPolarTransform( const LogPolarTransform& ) = default;

   /*!
    * \internal
    */
   bool IsLogPolar() const override
   {
      return true;
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PolarTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/PolarTransform.h - Released 2025-04-07T08:52:44Z
