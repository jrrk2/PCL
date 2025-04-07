//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Interpolation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Interpolation_h
#define __PCL_Interpolation_h

/// \file pcl/Interpolation.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::InterpolationAlgorithm
 * \brief Standard pixel interpolation algorithms.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>InterpolationAlgorithm::NearestNeighbor</td>         <td>Nearest neighbor interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Bilinear</td>                <td>Bilinear interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::BicubicSpline</td>           <td>Bicubic spline interpolation (usually the default algorithm)</td></tr>
 * <tr><td>InterpolationAlgorithm::Bicubic</td>                 <td>A synonym for bicubic spline interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::BicubicBSpline</td>          <td>Bicubic B-spline interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Lanczos3</td>                <td>Lanczos-3 interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Lanczos4</td>                <td>Lanczos-4 interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Lanczos5</td>                <td>Lanczos-5 interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::MitchellNetravaliFilter</td> <td>Mitchell-Netravali cubic filter interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::CatmullRomSplineFilter</td>  <td>Catmull-Rom spline filter interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::CubicBSplineFilter</td>      <td>Cubic B-spline filter interpolation</td></tr>
 * <tr><td>InterpolationAlgorithm::Auto</td>                    <td>Used to indicate an automatic algorithm selection (often the default option)</td></tr>
 * <tr><td>InterpolationAlgorithm::Default</td>                 <td>Used to indicate a default interpolation algorithm (currently bicubic spline)</td></tr>
 * </table>
 */
namespace InterpolationAlgorithm
{
   enum value_type
   {
      Auto = -1,
      NearestNeighbor = 0,
      Bilinear,
      BicubicSpline,
      Bicubic = BicubicSpline,
      BicubicBSpline,
      Lanczos3,
      Lanczos4,
      Lanczos5,
      MitchellNetravaliFilter,
      CatmullRomSplineFilter,
      CubicBSplineFilter,
      NumberOfInterpolationAlgorithms,
      Default = BicubicSpline
   };
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Interpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/Interpolation.h - Released 2025-04-07T08:52:44Z
