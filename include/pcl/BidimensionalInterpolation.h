//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/BidimensionalInterpolation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_BidimensionalInterpolation_h
#define __PCL_BidimensionalInterpolation_h

/// \file pcl/BidimensionalInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class BidimensionalInterpolation
 * \brief A generic interface to two-dimensional interpolation algorithms.
 *
 * %BidimensionalInterpolation is an abstract base class for all
 * two-dimensional interpolation algorithm implementations in PCL.
 */
template <typename T>
class PCL_CLASS BidimensionalInterpolation
{
public:

   /*!
    * Constructs a %BidimensionalInterpolation object.
    */
   BidimensionalInterpolation() = default;

   /*!
    * Copy constructor.
    */
   BidimensionalInterpolation( const BidimensionalInterpolation& ) = default;

   /*!
    * Destroys a %BidimensionalInterpolation object.
    */
   virtual ~BidimensionalInterpolation()
   {
      Clear();
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param data    Two-dimensional matrix of function values stored in
    *                row-order. Must remain valid and accessible while this
    *                object is used to compute interpolated function values.
    *
    * \param width   Horizontal dimension (most rapidly varying coordinate) of
    *                the data array.
    *
    * \param height  Vertical dimension (most slowly varying coordinate) of the
    *                data array.
    */
   virtual void Initialize( const T* data, int width, int height )
   {
      if ( data == nullptr )
         throw Error( "Null data pointer in BidimensionalInterpolation::Initialize()" );
      if ( width <= 0 || height <= 0 )
         throw Error( "Invalid dimensions in BidimensionalInterpolation::Initialize()" );
      m_data = data;
      m_width = width;
      m_height = height;
   }

   /*!
    * Returns an interpolated function value at \a x, \a y coordinates.
    */
   virtual double operator()( double x, double y ) const = 0;

   /*!
    * Clears auxiliary/intermediate data (and/or whatever requires clean up).
    * Derived classes overriding this function should call their base class
    * version via explicit downcast.
    */
   virtual void Clear()
   {
      m_data = nullptr;
      m_width = m_height = 0;
   }

   /*!
    * Returns a pointer to the interpolated data array.
    */
   const T* BeingInterpolated() const
   {
      return m_data;
   }

   /*!
    * Returns the width (number of columns) of the interpolated data matrix.
    */
   int Width() const
   {
      return m_width;
   }

   /*!
    * Returns the height (number of rows) of the interpolated data matrix.
    */
   int Height() const
   {
      return m_height;
   }

   /*!
    * Enables (or disables) border filling.
    *
    * When border filling is enabled, a user-defined constant fill value is
    * used to interpolate at locations near the borders of the data matrix.
    *
    * When border filling is disabled, existing border values are extended to
    * interpolate at border locations (Neumann boundary conditions).
    */
   void EnableBorderFilling( bool enable = true )
   {
      m_fillBorder = enable;
   }

   /*!
    * Disables (or enables) border filling.
    *
    * This is a convenience member function, equivalent to:
    *
    * EnableBorderFilling( !disable );
    */
   void DisableBorderFilling( bool disable = true )
   {
      m_fillBorder = !disable;
   }

   /*!
    * Returns true iff border filling is enabled for this interpolation. See the
    * documentation for EnableBorderFilling() for more information.
    */
   bool IsBorderFillingEnabled() const
   {
      return m_fillBorder;
   }

   /*!
    * Sets the border fill value \a v. See the documentation for
    * EnableBorderFilling() for more information about border filling and
    * boundary conditions.
    *
    * \param v    Border fill value. It is the responsibility of the caller to
    *             ensure that the specified value is within the valid range of
    *             the data type used by this interpolation.
    */
   void SetBorderFillValue( double v )
   {
      m_fillValue = v;
   }

   /*!
    * Returns the current border fill value for this interpolation. See the
    * documentation for EnableBorderFilling() for more information about border
    * filling and boundary conditions.
    */
   double BorderFillValue() const
   {
      return m_fillValue;
   }

protected:

   const T* m_data       = nullptr; // functional data being interpolated
   int      m_width      = 0;       // width of the data table
   int      m_height     = 0;       // height of the data table
   double   m_fillValue  = 0;       // fill value, when m_fillBorder = true
   bool     m_fillBorder = false;   // don't apply Neumann boundary conditions
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_BidimensionalInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/BidimensionalInterpolation.h - Released 2025-04-07T08:52:44Z
