//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MeanFilter.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MeanFilter_h
#define __PCL_MeanFilter_h

/// \file pcl/MeanFilter.h

#include <pcl/Defs.h>

#include <pcl/KernelFilter.h>
#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MeanFilter
 * \brief Square block average kernel filter
 *
 * A %MeanFilter object is a specialized KernelFilter whose elements are
 * identical and form a square symmetrically distributed around the origin:
 *
 * M(x,y) = 1/(s^2)
 *
 * where s is the filter size, which is the same on the horizontal and vertical
 * axes. This filter is separable.
 *
 * %MeanFilter is a building block of some important algorithms on the
 * PixInsight/PCL platform, such as the Multiscale Local Normalization,
 * algorithm, which uses these kernels to compute local variances.
 *
 * \sa KernelFilter, VariableShapeFilter, MoffatFilter, LinearFilter,
 * GaussianFilter
 */
class PCL_CLASS MeanFilter : public KernelFilter
{
public:

   /*!
    * Constructs an empty %MeanFilter object.
    */
   MeanFilter() = default;

   /*!
    * Constructs a %MeanFilter object given the odd kernel size \a n >= 3, or
    * an empty kernel filter if \a n = 0. Assigns an optional \a name to the
    * new filter object.
    */
   MeanFilter( int n, const String& name = String() )
   {
      Initialize( n );
      Rename( name );
   }

   /*!
    * Copy constructor.
    */
   MeanFilter( const MeanFilter& ) = default;

   /*!
    * Move constructor.
    */
   MeanFilter( MeanFilter&& ) = default;

   /*!
    */
   KernelFilter* Clone() const override
   {
      return new MeanFilter( *this );
   }

   /*!
    * Returns a separable filter equivalent to this Gaussian kernel filter.
    *
    * A block average filter is always separable, so this member function
    * should always return a valid SeparableFilter object.
    */
   SeparableFilter AsSeparableFilter( float tolerance = __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE ) const override
   {
      FVector v = coefficients.RowVector( Size()>>1 );
      return SeparableFilter( v, v );
   }

   /*!
    * Returns true iff this filter is separable.
    *
    * A block average filter is separable. As reimplemented in %MeanFilter,
    * this member function returns true.
    */
   bool IsSeparable() const override
   {
      return true;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MeanFilter& operator =( const MeanFilter& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MeanFilter& operator =( MeanFilter&& ) = default;

   /*!
    * Recalculates filter coefficients for the given odd kernel size \a n >= 3.
    */
   void Resize( int n ) override
   {
      Initialize( n );
   }

private:

   void Initialize( int n )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 && (n & 1) != 0 )
      KernelFilter::Resize( n );
      if ( !IsEmpty() )
         coefficients = 1.0/NumberOfCoefficients();
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MeanFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/MeanFilter.h - Released 2025-04-07T08:52:44Z
