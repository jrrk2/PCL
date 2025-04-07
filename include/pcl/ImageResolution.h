//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageResolution.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ImageResolution_h
#define __PCL_ImageResolution_h

/// \file pcl/ImageResolution.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageResolution
 * \brief %Image resolution data
 *
 * ### TODO: Write a detailed description for %ImageResolution.
 */
class PCL_CLASS ImageResolution
{
public:

   /*!
    * Constructs a default %ImageResolution object.
    */
   ImageResolution() = default;

   /*!
    * Copy constructor.
    */
   ImageResolution( const ImageResolution& ) = default;

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   ImageResolution& operator =( const ImageResolution& ) = default;

   /*!
    * Gets resolution values for both \a x and \a y axes in pixels per
    * resolution unit.
    *
    * \sa HorizontalResolution(), VerticalResolution()
    */
   void GetResolution( double& x, double& y )
   {
      x = xResolution; y = yResolution;
   }

   /*!
    * Returns the horizontal (X-axis) resolution in pixels per resolution unit.
    *
    * \sa VerticalResolution(), GetResolution()
    */
   double HorizontalResolution() const
   {
      return xResolution;
   }

   /*!
    * Returns the vertical (Y-axis) resolution in pixels per resolution unit.
    *
    * \sa HorizontalResolution(), GetResolution()
    */
   double VerticalResolution() const
   {
      return yResolution;
   }

   /*!
    * Sets resolution values for both \a x and \a y axes in pixels per
    * resolution unit.
    *
    * \sa GetResolution(), SetResolution( double )
    */
   void SetResolution( double x, double y )
   {
      PCL_PRECONDITION( x >= 0 )
      PCL_PRECONDITION( y >= 0 )
      xResolution = pcl::Max( 0.0, x ); yResolution = pcl::Max( 0.0, y );
   }

   /*!
    * Returns the resolution for both axes, in pixels per resolution unit.
    *
    * This member function is more meaningful for contexts where separate
    * horizontal and vertical resolutions are not supported.
    *
    * \sa GetResolution(), SetResolution( double ), SetResolution( double, double )
    */
   double Resolution() const
   {
      return xResolution;
   }

   /*!
    * Sets the resolution for both axes, in pixels per resolution unit.
    *
    * This member function is more meaningful for contexts where separate
    * horizontal and vertical resolutions are not supported.
    *
    * \sa Resolution(), HorizontalResolution(), VerticalResolution(), GetResolution()
    */
   void SetResolution( double r )
   {
      PCL_PRECONDITION( r >= 0 )
      xResolution = yResolution = pcl::Max( 0.0, r );
   }

   /*!
    * Returns true iff this object express resolution in pixels per centimeter;
    * false if it express resolution in pixels per inch.
    *
    * \sa SetMetricResolution()
    */
   bool IsMetricResolution() const
   {
      return metric;
   }

   /*!
    * Sets the resolution unit for this %ImageResolution object.
    *
    * \param m    If true, enables resolution in pixels per centimeter (metric
    *             resolution). If false, enables resolution in pixels per inch.
    *
    * \sa IsMetricResolution()
    */
   void SetMetricResolution( bool m )
   {
      metric = m;
   }

protected:

   /*
    * Resolution in pixels per resolution unit (centimeters or inches).
    */
   double xResolution = 72;
   double yResolution = 72;

   /*
    * Flag true when resolution values are expressed in pixels per centimeter.
    * Pixels per inch otherwise.
    */
   bool   metric = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageResolution_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageResolution.h - Released 2025-04-07T08:52:44Z
