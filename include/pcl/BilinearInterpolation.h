//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/BilinearInterpolation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_BilinearInterpolation_h
#define __PCL_BilinearInterpolation_h

/// \file pcl/BilinearInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/BidimensionalInterpolation.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width      this->m_width
#define m_height     this->m_height
#define m_fillBorder this->m_fillBorder
#define m_fillValue  this->m_fillValue
#define m_data       this->m_data

// ----------------------------------------------------------------------------

/*!
 * \class BilinearInterpolation
 * \brief Bilinear interpolation algorithm.
 *
 * The bilinear interpolation algorithm interpolates from the nearest four
 * mapped source data items. It builds and evaluates two linear interpolation
 * polynomials, one for each plane direction.
 *
 * \sa NearestNeighborInterpolation, BicubicSplineInterpolation,
 * BicubicBSplineInterpolation, BicubicFilterInterpolation,
 */
template <typename T>
class PCL_CLASS BilinearInterpolation : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a %BilinearInterpolation instance.
    */
   BilinearInterpolation() = default;

   /*!
    * Copy constructor.
    */
   BilinearInterpolation( const BilinearInterpolation& ) = default;

   /*!
    * Virtual destructor.
    */
   ~BilinearInterpolation() override
   {
   }

   /*!
    * Returns an interpolated value at {\a x,\a y} location.
    *
    * \param x,y  Coordinates of the interpolation point (horizontal,vertical).
    */
   double operator()( double x, double y ) const override
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )

      int j0 = pcl::Range( TruncInt( x ), 0, m_width-1 );
      int i0 = pcl::Range( TruncInt( y ), 0, m_height-1 );

      int j1 = j0 + 1;
      int i1 = i0 + 1;

      double p00, p10, p01, p11;
      const T* fp = m_data + (int64( i0 )*m_width + j0);

      p00 = *fp;
      p10 = (j1 < m_width) ? fp[1] : (m_fillBorder ? m_fillValue : *fp);

      if ( i1 < m_height )
         fp += m_width;
      else if ( m_fillBorder )
      {
         p01 = p11 = m_fillValue;
         goto __1;
      }

      p01 = *fp;
      p11 = (j1 < m_width) ? fp[1] : (m_fillBorder ? m_fillValue : *fp);

__1:
      double dx = x - j0, dx1 = 1 - dx;
      double dy = y - i0, dy1 = 1 - dy;
      return p00*dx1*dy1 + p10*dx*dy1 + p01*dx1*dy + p11*dx*dy;
   }
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_fillBorder
#undef m_fillValue
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BilinearInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/BilinearInterpolation.h - Released 2025-04-07T08:52:44Z
