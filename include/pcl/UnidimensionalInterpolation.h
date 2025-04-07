//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/UnidimensionalInterpolation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_UnidimensionalInterpolation_h
#define __PCL_UnidimensionalInterpolation_h

/// \file pcl/UnidimensionalInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class UnidimensionalInterpolation
 * \brief A generic interface to one-dimensional interpolation algorithms.
 *
 * %UnidimensionalInterpolation is an abstract base class for all
 * one-dimensional interpolation algorithm implementations in PCL.
 */
template <typename T>
class PCL_CLASS UnidimensionalInterpolation
{
public:

   /*!
    * Represents a vector of independent and dependent variable values.
    */
   using vector_type = GenericVector<T>;

   /*!
    * Constructs a %UnidimensionalInterpolation object.
    */
   UnidimensionalInterpolation() = default;

   /*!
    * Copy constructor.
    */
   UnidimensionalInterpolation( const UnidimensionalInterpolation& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~UnidimensionalInterpolation()
   {
      Clear();
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param x    %Vector of x-values:\n
    *             \n
    *       \li If this vector is not empty: Must be a set of monotonically\n
    *       increasing, distinct values: x[0] < x[1] < ... < x[n-1].\n
    *       \li If this vector is empty: The interpolation will use implicit
    *       x[i] = i for i = {0,1,...,n-1}.
    *
    * \param y    %Vector of function values for i = {0,1,...,n-1}.
    */
   virtual void Initialize( const vector_type& x, const vector_type& y )
   {
      if ( !y || x && x.Length() < y.Length() )
         throw Error( "Invalid vector length in UnidimensionalInterpolation::Initialize()" );
      m_x = x;
      m_y = y;
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param x    %Array of x-values:\n
    *             \n
    *       \li If \a x != 0: Must be a list of monotonically increasing,
    *       distinct values: x[0] < x[1] < ... < x[n-1].\n
    *       \li If \a x == 0: The interpolation will use implicit
    *       x[i] = i for i = {0,1,...,n-1}.
    *
    * \param y    %Array of function values for i = {0,1,...,n-1}.
    *
    * \param n    Number of data points. The minimum number of required data
    *             points depends on the interpolation algorithm. In general,
    *             all algorithms require at least \a n >= 2.
    *
    * \deprecated This member function has been deprecated. For newly produced
    * code, use Initialize( const vector_type&, const vector_type& ).
    */
   void Initialize( const T* x, const T* y, int n )
   {
      Initialize( vector_type( x, n ), vector_type( y, n ) );
   }

   /*!
    * Returns an interpolated function value at \a x location.
    */
   virtual double operator()( double x ) const = 0;

   /*!
    * Clears auxiliary or intermediate interpolation data. Derived classes
    * overriding this function should call their base class version via
    * explicit downcast.
    */
   virtual void Clear()
   {
      m_x.Clear();
      m_y.Clear();
   }

   /*!
    * Returns true iff this interpolation is valid, i.e. if it has been
    * correctly initialized and is ready to interpolate function values.
    */
   virtual bool IsValid() const
   {
      return m_y;
   }

   /*!
    * Returns a reference to the x-value data vector. Returns an empty vector
    * if this interpolation uses implicit x[i] = i for i = {0,1,...,n-1}.
    */
   const vector_type& X() const
   {
      return m_x;
   }

   /*!
    * Returns a reference to the interpolated function value vector.
    */
   const vector_type& Y() const
   {
      return m_y;
   }

   /*!
    * Returns true iff this interpolation uses implicit
    * x[i] = i for i = {0,1,...,n-1}.
    */
   bool UsingImplicitX() const
   {
      return !m_x && m_y;
   }

   /*!
    * Returns the length of the interpolated function value vector.
    */
   int Length() const
   {
      return m_y.Length();
   }

protected:

   vector_type m_x;  // x-vector (empty if implicit grid x-values)
   vector_type m_y;  // y-vector
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_UnidimensionalInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/UnidimensionalInterpolation.h - Released 2025-04-07T08:52:44Z
