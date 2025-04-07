//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Resample.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Resample_h
#define __PCL_Resample_h

/// \file pcl/Resample.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>
#include <pcl/ImageResolution.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
   \namespace pcl::ResizeMode
   \brief     Resizing modes for the Resample process.

   <table border="1" cellpadding="4" cellspacing="0">
   <tr><td>ResizeMode::RelativeDimensions</td>  <td>Resize relative to current image dimensions</td></tr>
   <tr><td>ResizeMode::AbsolutePixels</td>      <td>Resize to absolute dimensions in pixels</td></tr>
   <tr><td>ResizeMode::AbsoluteCentimeters</td> <td>Resize to absolute dimensions in centimeters</td></tr>
   <tr><td>ResizeMode::AbsoluteInches</td>      <td>Resize to absolute dimensions in inches</td></tr>
   <tr><td>ResizeMode::ForceArea</td>           <td>Force the total number of pixels and keep existing aspect ratio</td></tr>
   <tr><td>ResizeMode::Default</td>             <td>Default resizing mode, equal to ResizeMode::RelativeDimensions</td></tr>
   </table>
*/
namespace ResizeMode
{
   enum value_type
   {
      RelativeDimensions,  // Resize relative to current image dimensions
      AbsolutePixels,      // ... to absolute dimensions in pixels
      AbsoluteCentimeters, // ... to absolute dimensions in centimeters
      AbsoluteInches,      // ... to absolute dimensions in inches
      ForceArea,           // Force number of pixels, keep aspect ratio
      NumberOfResizeModes,
      Default = RelativeDimensions
   };
}

// ----------------------------------------------------------------------------

/*!
   \namespace pcl::AbsoluteResizeMode
   \brief     Absolute resizing modes for the Resample process.

   Absolute resize modes are applicable only when the main resize mode is
   ResizeMode::AbsolutePixels, ResizeMode::AbsoluteCentimeters or
   ResizeMode::AbsoluteInches.

   <table border="1" cellpadding="4" cellspacing="0">
   <tr><td>AbsoluteResizeMode::ForceWidthAndHeight</td> <td>Force both image dimensions</td></tr>
   <tr><td>AbsoluteResizeMode::ForceWidth</td>          <td>Force width</td> preserve existing aspect ratio</td></tr>
   <tr><td>AbsoluteResizeMode::ForceHeight</td>         <td>Force height</td> preserve existing aspect ratio</td></tr>
   <tr><td>AbsoluteResizeMode::Default</td>             <td>Default absolute resizing mode</td> equal to AbsoluteResizeMode::ForceWidthAndHeight</td></tr>
   </table>
*/
namespace AbsoluteResizeMode
{
   enum value_type
   {
      ForceWidthAndHeight, // Force both dimensions
      ForceWidth,          // Force width, preserve aspect ratio
      ForceHeight,         // Force height, preserve aspect ratio
      NumberOfAbsoluteResizeModes,
      Default = ForceWidthAndHeight
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Resample
 * \brief %Image resampling algorithm.
 *
 * ### TODO: Write a detailed description for %Resample
 */
class PCL_CLASS Resample : public InterpolatingGeometricTransformation,
                           public ImageResolution,
                           public ParallelProcess
{
public:

   /*!
    * Represents a resizing mode.
    */
   using resize_mode = ResizeMode::value_type;

   /*!
    * Represents an absolute resizing mode.
    */
   using abs_resize_mode = AbsoluteResizeMode::value_type;

   /*!
    * Constructs a default %Resample object using the specified pixel
    * interpolation \a p and a single scaling ratio for both axes.
    *
    * The specified pixel interpolation object \a p must remain valid while
    * this object exists.
    *
    * The resizing mode is set relative to current image dimensions.
    */
   Resample( PixelInterpolation& p, double scale = 1.0 )
      : InterpolatingGeometricTransformation( p )
      , m_xSize( scale )
      , m_ySize( scale )
   {
   }

   /*!
    * Constructs a default %Resample object using the specified pixel
    * interpolation \a p and separate scaling ratios for the X and Y axes,
    * \a scaleX and \a scaleY respectively.
    *
    * The specified pixel interpolation object \a p must remain valid while
    * this object exists.
    *
    * The resizing mode is set relative to current image dimensions.
    */
   Resample( PixelInterpolation& p, double scaleX, double scaleY )
      : InterpolatingGeometricTransformation( p )
      , m_xSize( scaleX )
      , m_ySize( scaleY )
   {
   }

   /*!
    * Copy constructor.
    */
   Resample( const Resample& ) = default;

   /*!
    * Gets the resampled image \a width and \a height.
    *
    * Use GetSizes(), XSize() and YSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void GetSizes( double& width, double& height ) const
   {
      width = m_xSize; height = m_ySize;
   }

   /*!
    * Returns the resampled image width.
    *
    * Use GetSizes(), XSize() and YSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   double XSize() const
   {
      return m_xSize;
   }

   /*!
    * Returns the resampled image height.
    *
    * Use GetSizes(), XSize() and YSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   double YSize() const
   {
      return m_ySize;
   }

   /*!
    * Sets the resampled image \a width and \a height.
    *
    * Use SetSizes(), SetXSize() and SetYSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void SetSizes( double width, double height )
   {
      m_xSize = width; m_ySize = height;
   }

   /*!
    * Sets the resampled image \a width.
    *
    * Use SetSizes(), SetXSize() and SetYSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void SetXSize( double width )
   {
      m_xSize = width;
   }

   /*!
    * Sets the resampled image \a height.
    *
    * Use SetSizes(), SetXSize() and SetYSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void SetYSize( double height )
   {
      m_ySize = height;
   }

   /*!
    * Gets the scaling factors \a sx and \a sy for the X and Y axes,
    * respectively.
    *
    * Use GetScalingFactors(), XScale() and YScale() semantics to define a
    * %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void GetScalingFactors( double& sx, double& sy ) const
   {
      GetSizes( sx, sy );
   }

   /*!
    * Returns the horizontal scaling factor.
    *
    * Use GetScalingFactors(), XScale() and YScale() semantics to define a
    * %Resample instance that resizes images relative to their current
    * dimensions.
    */
   double XScale() const
   {
      return XSize();
   }

   /*!
    * Returns the vertical scaling factor.
    *
    * Use GetScalingFactors(), XScale() and YScale() semantics to define a
    * %Resample instance that resizes images relative to their current
    * dimensions.
    */
   double YScale() const
   {
      return YSize();
   }

   /*!
    * Sets the scaling factors \a sx and \a sy for the X and Y axes,
    * respectively.
    *
    * Use SetScalingFactors(), SetXScale() and SetYScale() semantics to define
    * a %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void SetScalingFactors( double sx, double sy )
   {
      SetSizes( sx, sy );
   }

   /*!
    * Sets the horizontal scaling factor.
    *
    * Use SetScalingFactors(), SetXScale() and SetYScale() semantics to define
    * a %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void SetXScale( double sx )
   {
      SetXSize( sx );
   }

   /*!
    * Sets the vertical scaling factor.
    *
    * Use SetScalingFactors(), SetXScale() and SetYScale() semantics to define
    * a %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void SetYScale( double sy )
   {
      SetYSize( sy );
   }

   /*!
    * Returns the current resize mode.
    */
   resize_mode Mode() const
   {
      return m_mode;
   }

   /*!
    * Returns true iff this %Resample object resizes images relative to their
    * current dimensions, i.e. when Mode() is equal to
    * ResizeMode::RelativeDimensions.
    */
   bool IsRelative() const
   {
      return m_mode == ResizeMode::RelativeDimensions;
   }

   /*!
    * Returns true iff this %Resample object resizes images to fixed dimensions,
    * i.e. when Mode() is not equal to ResizeMode::RelativeDimensions.
    */
   bool IsAbsolute() const
   {
      return !IsRelative();
   }

   /*!
    * Sets the current resize mode.
    */
   void SetMode( resize_mode mode )
   {
      m_mode = mode;
   }

   /*!
    * Returns the current absolute resizing mode. The absolute resizing mode is
    * only applied when Mode() is not equal to ResizeMode::RelativeDimensions.
    */
   abs_resize_mode AbsMode() const
   {
      return m_absMode;
   }

   /*!
    * Sets the absolute resizing mode. The absolute resizing mode is only
    * applied when Mode() is not equal to ResizeMode::RelativeDimensions.
    */
   void SetAbsMode( abs_resize_mode absMode )
   {
      m_absMode = absMode;
   }

   /*!
    */
   void GetNewSizes( int& width, int& height ) const override;

   /*!
    */
   bool SupportsGammaCorrection() const override
   {
      return true;
   }

protected:

   double          m_xSize = 1.0;
   double          m_ySize = 1.0;
   resize_mode     m_mode = ResizeMode::Default;
   abs_resize_mode m_absMode = AbsoluteResizeMode::Default;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Resample_h

// ----------------------------------------------------------------------------
// EOF pcl/Resample.h - Released 2025-04-07T08:52:44Z
