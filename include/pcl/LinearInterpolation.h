//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/LinearInterpolation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_LinearInterpolation_h
#define __PCL_LinearInterpolation_h

/// \file pcl/LinearInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/UnidimensionalInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class LinearInterpolation
 * \brief Linear interpolation algorithm.
 *
 * The linear interpolation algorithm interpolates from two adjacent mapped
 * source data items. It evaluates a linear function defined by the two
 * adjacent points.
 *
 * \sa AkimaInterpolation, CubicSplineInterpolation
 */
template <typename T = double>
class PCL_CLASS LinearInterpolation : public UnidimensionalInterpolation<T>
{
public:

   using vector_type = typename UnidimensionalInterpolation<T>::vector_type;

   /*!
    * Constructs a %LinearInterpolation object.
    */
   LinearInterpolation() = default;

   /*!
    * Copy constructor.
    */
   LinearInterpolation( const LinearInterpolation& ) = default;

   /*!
    * Destroys a %LinearInterpolation object.
    */
   ~LinearInterpolation() override
   {
   }

   /*!
    * Returns true iff this interpolation is valid, i.e. if it has been
    * correctly initialized and is ready to interpolate function values.
    */
   bool IsValid() const override
   {
      return this->m_y.Length() > 1;
   }

   /*!
    * Interpolated value at \a x location.
    */
   double operator()( double x ) const override
   {
      PCL_PRECONDITION( IsValid() )

      if ( this->m_x )
      {
         // Interpolation with monotonically increasing, explicit x[i].

         PCL_CHECK( this->m_x.Length() >= this->m_y.Length() )
         int i0 = 0;
         int i1 = this->m_x.Length() - 1;
         while ( i1-i0 > 1 )
         {
            int im = (i0 + i1) >> 1;
            if ( x < this->m_x[im] )
               i1 = im;
            else
               i0 = im;
         }
         double x0( this->m_x[i0] );
         double y0( this->m_y[i0] );
         double dx = double( this->m_x[i1] ) - x0;
         if ( 1 + dx != 1 ) // if no infinite slope (to within roundoff)
            y0 += (x - x0)*(double( this->m_y[i1] ) - y0)/dx;
         return y0;
      }
      else
      {
         // Interpolation with implicit x[i] = i for i = {0,1,...,n-1}.

         if ( x <= 0 )
            return this->m_y[0];
         if ( x >= this->m_y.Length()-1 )
            return this->m_y[this->m_y.Length()-1];
         int i0 = TruncInt( x );
         double y0( this->m_y[i0] );
         return y0 + (x - i0)*(double( this->m_y[i0+1] ) - y0);
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_LinearInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/LinearInterpolation.h - Released 2025-04-07T08:52:44Z
