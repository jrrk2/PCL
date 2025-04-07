//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ColorSpace.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ColorSpace_h
#define __PCL_ColorSpace_h

/// \file pcl/ColorSpace.h

#include <pcl/Defs.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ColorSpace
 * \brief     Supported color spaces
 *
 * Current versions of the PixInsight platform support the following color
 * spaces for images:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ColorSpace::Unknown</td> <td>Corresponds to an unknown or unsupported color space</td></tr>
 * <tr><td>ColorSpace::Gray</td>    <td>Grayscale monochrome space</td></tr>
 * <tr><td>ColorSpace::RGB</td>     <td>RGB color space</td></tr>
 * <tr><td>ColorSpace::CIEXYZ</td>  <td>CIE XYZ color space</td></tr>
 * <tr><td>ColorSpace::CIELab</td>  <td>CIE L*a*b* color space</td></tr>
 * <tr><td>ColorSpace::CIELch</td>  <td>CIE L*c*h* color space</td></tr>
 * <tr><td>ColorSpace::HSV</td>     <td>HSV color space: Hue, Saturation, Value</td></tr>
 * <tr><td>ColorSpace::HSI</td>     <td>HSI color space: Hue, Saturation, Intensity</td></tr>
 * </table>
 */
namespace ColorSpace
{
   enum value_type
   {
      Unknown = -1,  // Corresponds to an unknown or unsupported color space
      Gray = 0,      // Grayscale monochrome space
      RGB,           // RGB color space
      CIEXYZ,        // CIE XYZ color space
      CIELab,        // CIE L*a*b* color space
      CIELch,        // CIE L*c*h* color space
      HSV,           // HSV color space: Hue, Saturation, Value
      HSI,           // HSI color space: Hue, Saturation, Intensity
      NumberOfColorSpaces
   };

   /*!
    * Returns the number of nominal channels in the specified color space.
    */
   inline int NumberOfNominalChannels( int colorSpace )
   {
      return (colorSpace == Gray) ? 1 : 3;
   }

   /*!
    * Returns a string representing the name of a color space.
    *
    * \param colorSpace    A supported color space, identified by its
    *                      corresponding symbolic constant.
    */
   String Name( int colorSpace );

   /*!
    * Returns the identifier of a nominal channel in a specified color space.
    *
    * \param colorSpace    A supported color space, identified by its
    *                      corresponding symbolic constant.
    *
    * \param channel       The index >= 0 of a nominal channel or component.
    */
   String ChannelId( int colorSpace, int channel );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ColorSpace_h

// ----------------------------------------------------------------------------
// EOF pcl/ColorSpace.h - Released 2025-04-07T08:52:44Z
