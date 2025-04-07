//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/FastRotation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_FastRotation_h
#define __PCL_FastRotation_h

/// \file pcl/FastRotation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup fast_rotations Non-interpolating Rotations and Specular&nbsp;\
 * Transformations.
 *
 * <em>Fast rotations</em> are non-interpolating geometric transformations:
 * they carry out image rotation and specular projection operations exclusively
 * by copying and swapping pixels.
 *
 * Since no pixel interpolation is performed, there is absolutely no data
 * degradation after an arbitrary number of consecutive fast rotations.
 */

// ----------------------------------------------------------------------------

/*!
 * \class Rotate180
 * \brief Rotates images by 180 degrees
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS Rotate180 : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class Rotate90CW
 * \brief Rotates images by 90 degrees (clockwise)
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS Rotate90CW : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      pcl::Swap( w, h ); // Permute
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class Rotate90CCW
 * \brief Rotates images by -90 degrees (counter-clockwise)
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS Rotate90CCW : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      pcl::Swap( w, h ); // Permute
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class HorizontalMirror
 * \brief Mirrors images horizontally
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS HorizontalMirror : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class VerticalMirror
 * \brief Mirrors images vertically
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS VerticalMirror : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FastRotation_h

// ----------------------------------------------------------------------------
// EOF pcl/FastRotation.h - Released 2025-04-07T08:52:44Z
