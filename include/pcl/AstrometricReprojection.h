//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/AstrometricReprojection.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_AstrometricReprojection_h
#define __PCL_AstrometricReprojection_h

/// \file pcl/AstrometricReprojection.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AstrometricMetadata.h>
#include <pcl/GeometricTransformation.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AstrometricReprojection
 * \brief Astrometric image reprojection.
 *
 * ### TODO: Write a detailed description for %AstrometricReprojection
 */
class PCL_CLASS AstrometricReprojection : public InterpolatingGeometricTransformation,
                                          public ParallelProcess
{
public:

   /*!
    * Constructs a new %AstrometricReprojection object.
    *
    * \param interpolation    Reference to a pixel interpolation for
    *                         reprojection of source pixels.
    *
    * \param targetSolution   The astrometric solution of the target image.
    *                         This object provides image-to-celestial
    *                         coordinate transformations for reverse projection
    *                         of interpolated source pixels on a target image.
    *
    * \param sourceSolution   The astrometric solution of the source image.
    *                         This object provides celestial-to-image
    *                         coordinate transformations for interpolation of
    *                         source image pixels.
    *
    * \param sourceImage      The source image that will be reprojected.
    *
    * \param onTargetBounds   If true, the reprojection will be calculated for
    *                         the entire target image bounds. If false, the
    *                         reprojection will be calculated exclusively
    *                         within the reprojected rectangular region in
    *                         target solution coordinates. The default value is
    *                         false.
    *
    * Setting \a onTargetBounds=true can be useful to accelerate reprojections
    * performed on entire images, avoiding the performance penalty caused by
    * the calculation of reprojected regions, when one knows in advance that
    * the target solution is contained by the source solution. However, with
    * \a onTargetBounds=false (default value), partial reprojections can be
    * significantly accelerated by avoiding unnecessary calculations for pixels
    * outside reprojected regions. This is typically the case of mosaic frame
    * reprojections.
    *
    * All objects specified in a call to this constructor must remain valid
    * while this instance exists.
    */
   AstrometricReprojection( PixelInterpolation& interpolation,
                            const AstrometricMetadata& targetSolution,
                            const AstrometricMetadata& sourceSolution,
                            const ImageVariant& sourceImage,
                            bool onTargetBounds = false )
      : InterpolatingGeometricTransformation( interpolation )
      , m_targetSolution( targetSolution )
      , m_sourceSolution( sourceSolution )
      , m_sourceImage( sourceImage )
      , m_onTargetBounds( onTargetBounds )
   {
   }

   /*!
    * Copy constructor.
    */
   AstrometricReprojection( const AstrometricReprojection& ) = default;

   /*!
    * Returns a reference to the astrometric solution of the target image. The
    * returned object provides image-to-celestial coordinate transformations
    * for reverse projection of interpolated source pixels on a target image.
    */
   const AstrometricMetadata& TargetSolution() const
   {
      return m_targetSolution;
   }

   /*!
    * Returns a reference to the astrometric solution of the source image. The
    * returned object provides celestial-to-image coordinate transformations
    * for interpolation of source image pixels.
    */
   const AstrometricMetadata& SourceSolution() const
   {
      return m_sourceSolution;
   }

   /*!
    * Returns a reference to the source image that will be reprojected.
    */
   const ImageVariant& SourceImage() const
   {
      return m_sourceImage;
   }

   /*!
    * Returns true iff this astrometric reprojection has been constructed to
    * calculate reprojections on entire target images. See the class
    * constructor for more information on partial and complete reprojections.
    */
   bool OnTargetBounds() const
   {
      return m_onTargetBounds;
   }

   /*!
    * Returns the rectangle in target solution coordinates that encloses the
    * boundaries of the reprojected source solution.
    */
   const DRect& TargetRect() const
   {
      if ( !m_targetRect.IsRect() )
         m_targetRect = TargetRect( m_targetSolution, m_sourceSolution, m_sourceSolution.Bounds() );
      return m_targetRect;
   }

   /*!
    * Returns a reprojected rectangle in source solution coordinates.
    *
    * \param targetSolution   The target astrometric solution.
    *
    * \param sourceSolution   The source astrometric solution.
    *
    * \param sourceRect       The source rectangular region.
    *
    * This function throws an Error exception in the event of invalid
    * coordinate transformations.
    */
   static DRect TargetRect( const AstrometricMetadata& targetSolution,
                            const AstrometricMetadata& sourceSolution,
                            const Rect& sourceRect );

   /*!
    * Returns true iff the specified point in target solution coordinates lies
    * inside the specified source solution coordinates after astrometric
    * reprojection.
    *
    * \param targetSolution   The target astrometric solution.
    *
    * \param sourceSolution   The source astrometric solution.
    *
    * \param targetPt         The target point in target solution coordinates.
    *
    * \param sourceRect       The source rectangular region in source solution
    *                         coordinates.
    */
   static bool TargetPointInsideSourceRect( const AstrometricMetadata& targetSolution,
                                            const AstrometricMetadata& sourceSolution,
                                            const DPoint& targetPt,
                                            const DRect& sourceRect );

   /*!
    * Returns true iff the specified rectangular region in target solution
    * coordinates lies inside the specified source solution coordinates after
    * astrometric reprojection.
    *
    * \param targetSolution   The target astrometric solution.
    *
    * \param sourceSolution   The source astrometric solution.
    *
    * \param targetRect       The target rectangular region in target solution
    *                         coordinates.
    *
    * \param sourceRect       The source rectangular region in source solution
    *                         coordinates.
    */
   static bool TargetRectInsideSourceRect( const AstrometricMetadata& targetSolution,
                                           const AstrometricMetadata& sourceSolution,
                                           const DRect& targetRect,
                                           const DRect& sourceRect );

   /*!
    */
   void GetNewSizes( int& width, int& height ) const override
   {
   }

   /*!
    * Returns the total number of black (zero) source pixels reprojected in the
    * last process execution. This is useful to detect reprojection of empty
    * source image regions.
    *
    * Returns zero if this instante has not yet been executed.
    */
   size_type ZeroCount() const
   {
      return m_zeroCount;
   }

protected:

   const   AstrometricMetadata& m_targetSolution;
   const   AstrometricMetadata& m_sourceSolution;
   const   ImageVariant&        m_sourceImage;
           bool                 m_onTargetBounds = false;
   mutable DRect                m_targetRect = 0;
   mutable size_type            m_zeroCount = 0;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;

   friend class PCL_AstrometricReprojectionEngine;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AstrometricReprojection_h

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricReprojection.h - Released 2025-04-07T08:52:44Z
