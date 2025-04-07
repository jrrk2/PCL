//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CanvasColor.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_CanvasColor_h
#define __PCL_CanvasColor_h

/// \file pcl/CanvasColor.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class CanvasColor
 * \brief A set of color values used to fill free or unused areas of images
 *
 * Some geometric image transformations, such as rotations and translations for
 * example, generate images with uncovered regions. %CanvasColor defines a set
 * of color components to initialize pixels on such unused areas.
 *
 * %CanvasColor stores pixel values in a normalized floating-point format
 * suitable to be used for any pixel sample data type. When using %CanvasColor
 * with integer-sampled images, color components should be kept normalized in
 * the normalized [0,1] range.
 */
class PCL_CLASS CanvasColor
{
public:

   /*!
    * Constructs a %CanvasColor object with zero (black) color components.
    */
   CanvasColor() = default;

   /*!
    * Copy constructor.
    */
   CanvasColor( const CanvasColor& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   CanvasColor& operator =( const CanvasColor& ) = default;

   /*!
    * Retrieves the current RGB sample values in this %CanvasColor object.
    *
    * \param[out] v0,v1,v2    References to variables that will receive current
    *                   sample values for the red/gray, green and blue
    *                   channels, respectively.
    */
   void GetCanvasColor( float& v0, float& v1, float& v2 ) const
   {
      v0 = canvasColor[0]; v1 = canvasColor[1]; v2 = canvasColor[2];
   }

   /*!
    * Retrieves the current RGBA sample values in this %CanvasColor object.
    *
    * \param[out] v0,v1,v2,v3    References to variables that will receive
    *                   current sample values for the red/gray, green, blue and
    *                   alpha channels, respectively.
    */
   void GetCanvasColor( float& v0, float& v1, float& v2, float& v3 ) const
   {
      v0 = canvasColor[0]; v1 = canvasColor[1]; v2 = canvasColor[2]; v3 = canvasColor[3];
   }

   /*!
    * Retrieves the current sample values in this %CanvasColor object.
    *
    * \param[out] v  Starting address of an array where current sample values
    *                will be copied. This array must provide storage for at
    *                least four elements.
    */
   void GetCanvasColor( float* v ) const
   {
      v[0] = canvasColor[0]; v[1] = canvasColor[1]; v[2] = canvasColor[2]; v[3] = canvasColor[3];
   }

   /*!
    * Sets current RGB sample values for this %CanvasColor object.
    *
    * \param v0,v1,v2      New red/gray, green and blue sample values.
    *
    * \note The alpha component will be set to one (white, opaque).
    */
   void SetCanvasColor( float v0, float v1, float v2 )
   {
      canvasColor[0] = v0; canvasColor[1] = v1; canvasColor[2] = v2; canvasColor[3] = 1;
   }

   /*!
    * Sets current RGB sample values for this %CanvasColor object.
    *
    * \param v0,v1,v2,v3   New red/gray, green, blue and alpha sample values.
    *
    * \note The alpha component will be set to one (white, opaque).
    */
   void SetCanvasColor( float v0, float v1, float v2, float v3 )
   {
      canvasColor[0] = v0; canvasColor[1] = v1; canvasColor[2] = v2; canvasColor[3] = v3;
   }

   /*!
    * Sets current sample values for this %CanvasColor object.
    *
    * \param v    Starting address of an array from which new sample values
    *             will be obtained. At least four elements are required.
    */
   void SetCanvasColor( const float* v )
   {
      canvasColor[0] = v[0]; canvasColor[1] = v[1]; canvasColor[2] = v[2]; canvasColor[3] = v[3];
   }

   /*!
    * Sets all sample values of this %CanvasColor object equal to an specified
    * value \a v.
    */
   void SetCanvasColor( float v )
   {
      canvasColor[0] = canvasColor[1] = canvasColor[2] = canvasColor[3] = v;
   }

protected:

   // Normalized floating-point sample values. Note that to the purpose of this
   // structure float provides enough accuracy, even for DImage.
   float canvasColor[ 4 ] = {}; // red/gray, green, blue, alpha
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_CanvasColor_h

// ----------------------------------------------------------------------------
// EOF pcl/CanvasColor.h - Released 2025-04-07T08:52:44Z
