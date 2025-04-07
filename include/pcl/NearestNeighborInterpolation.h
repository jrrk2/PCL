//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/NearestNeighborInterpolation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_NearestNeighborInterpolation_h
#define __PCL_NearestNeighborInterpolation_h

/// \file pcl/NearestNeighborInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/BidimensionalInterpolation.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width  this->m_width
#define m_height this->m_height
#define m_data   this->m_data

// ----------------------------------------------------------------------------

/*!
 * \class NearestNeighborInterpolation
 * \brief Two-dimensional nearest neighbor interpolation algorithm
 *
 * Nearest neighbor interpolation selects the value of the nearest data point
 * from the source 2-D matrix. This interpolation always returns existing data
 * values in the source matrix, since it doesn't consider the values of other
 * neighboring points.
 *
 * \sa BidimensionalInterpolation, BilinearInterpolation,
 * BicubicSplineInterpolation, BicubicBSplineInterpolation,
 * BicubicFilterInterpolation,
 */
template <typename T>
class PCL_CLASS NearestNeighborInterpolation : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a %NearestNeighborInterpolation instance.
    */
   NearestNeighborInterpolation() = default;

   /*!
    * Copy constructor.
    */
   NearestNeighborInterpolation( const NearestNeighborInterpolation& ) = default;

   /*!
    * Returns an interpolated value at {\a x,\a y} location.
    *
    * \param x,y  Coordinates of the interpolation point (horizontal,vertical).
    */
   double operator()( double x, double y ) const override
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )
      return double( *(m_data + (Range( RoundIntArithmetic( y ), 0, m_height-1 )*int64( m_width ) +
                                 Range( RoundIntArithmetic( x ), 0, m_width-1 ))) );
   }
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_NearestNeighborInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/NearestNeighborInterpolation.h - Released 2025-04-07T08:52:44Z
