//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/IntegerResample.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_IntegerResample_h
#define __PCL_IntegerResample_h

/// \file pcl/IntegerResample.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::IntegerDownsampleMode
 * \brief Downsampling operations for IntegerResample.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>IntegerDownsampleMode::Average</td> <td>Downsample by averaging source pixels</td></tr>
 * <tr><td>IntegerDownsampleMode::Median</td>  <td>Downsample by calculating the median of source pixels</td></tr>
 * <tr><td>IntegerDownsampleMode::Maximum</td> <td>Downsample by the maximum source pixel value</td></tr>
 * <tr><td>IntegerDownsampleMode::Minimum</td> <td>Downsample by the minimum source pixel value</td></tr>
 * </table>
 */
namespace IntegerDownsampleMode
{
   enum value_type
   {
      Average,
      Median,
      Maximum,
      Minimum
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class IntegerResample
 * \brief A geometric transformation to resample images by integer ratios.
 *
 * ### TODO: Write a detailed description for %IntegerResample.
 */
class PCL_CLASS IntegerResample : public GeometricTransformation,
                                  public ParallelProcess
{
public:

   /*!
    * Represents a downsample operation.
    */
   using downsample_mode = IntegerDownsampleMode::value_type;

   /*!
    * Constructs an %IntegerResample object with zoom factor \a z and
    * downsampling mode \a m.
    */
   IntegerResample( int zoom = 0, downsample_mode mode = IntegerDownsampleMode::Average )
      : m_zoomFactor( zoom )
      , m_downsampleMode( mode )
   {
   }

   /*!
    * Copy constructor.
    */
   IntegerResample( const IntegerResample& ) = default;

   /*!
    * Returns the current zoom factor of this %IntegerResample object.
    */
   int ZoomFactor() const
   {
      return m_zoomFactor;
   }

   /*!
    * Sets the \a zoom factor for this %IntegerResample object.
    *
    * Positive zoom factors are interpreted as magnifying factors; negative
    * zoom factors are reduction factors. For example:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr align="center"><td><b>Zoom Factor</b></td>  <td><b>Resampling ratio (original pixels : resampled pixels)</b></td></tr>
    * <tr align="center"><td>-3</td> <td>1:3 (1/3 reduction)</td></tr>
    * <tr align="center"><td>-2</td> <td>1:2 (1/2 reduction)</td></tr>
    * <tr align="center"><td>1</td>  <td>1:1 (actual pixels)</td></tr>
    * <tr align="center"><td>2</td>  <td>2:1 (x2 magnification)</td></tr>
    * <tr align="center"><td>3</td>  <td>3:1 (x3 magnification)</td></tr>
    * </table>
    *
    * \note Zoom factors 0 and -1 should not be used explicitly; they are
    * reserved for internal use, and there is no guarantee that future versions
    * of PCL will continue accepting them.
    */
   void SetZoomFactor( int zoom )
   {
      m_zoomFactor = (zoom < -1 || zoom > +1) ? zoom : +1;
   }

   /*!
    * Returns the pixel scaling factor applied by this instance, or the
    * equivalent image resampling ratio. See SetZoomFactor() for the
    * correspondence between zoom factors and resampling ratios.
    */
   double ScalingFactor() const
   {
      if ( m_zoomFactor == 0 )
         return 1;
      return (m_zoomFactor < 0) ? 1.0/(-m_zoomFactor) : double( m_zoomFactor );
   }

   /*!
    * Returns the current downsample mode of this %IntegerResample object.
    */
   downsample_mode DownsampleMode() const
   {
      return m_downsampleMode;
   }

   /*!
    * Sets the downsample \e mode for this %IntegerResample object.
    *
    * The downsample mode defines an statistical operation that the integer
    * resampling algorithms applies to the set of original pixels that produce
    * each pixel in the subsampled image. The default mode is
    * IntegerDownsampleMode::Average, which calculates the mean of source
    * pixels. The Median, Minimum and Maximum modes turn the integer
    * downsampling algorithm into a <em>morphological downsampling</em>
    * operation with very interesting properties and applications.
    */
   void SetDownsampleMode( downsample_mode mode )
   {
      m_downsampleMode = mode;
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

   /*
    * Zoom factor: > 1 -> zoom in, < 1 -> zoom out, from -1 to +1 -> no change.
    */
   int m_zoomFactor = 0;

   /*
    * Downsample mode: average, median, maximum, minimum; see enumeration above.
    */
   downsample_mode m_downsampleMode = IntegerDownsampleMode::Average;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_IntegerResample_h

// ----------------------------------------------------------------------------
// EOF pcl/IntegerResample.h - Released 2025-04-07T08:52:44Z
