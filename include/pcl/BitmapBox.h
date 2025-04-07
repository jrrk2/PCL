//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/BitmapBox.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_BitmapBox_h
#define __PCL_BitmapBox_h

/// \file pcl/BitmapBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Bitmap.h>
#include <pcl/Frame.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class BitmapBox
 * \brief Client-side interface to a PixInsight %BitmapBox control.
 *
 * ### TODO: Write a detailed description for %BitmapBox
 */
class PCL_CLASS BitmapBox : public Frame
{
public:

   /*!
    * Constructs a %BitmapBox control that draws a bitmap \a bmp and is a child
    * of \a parent.
    */
   BitmapBox( const Bitmap& bmp = Bitmap::Null(), Control& parent = Control::Null() );

   /*!
    * Destroys a %BitmapBox object.
    */
   ~BitmapBox() override
   {
   }

   /*!
    * Returns the bitmap currently drawn by this %BitmapBox control.
    */
   Bitmap CurrentBitmap() const;

   /*!
    * Sets the bitmap \a bmp for this %BitmapBox control.
    */
   void SetBitmap( const Bitmap& bmp );

   /*!
    * Removes the bitmap in this %BitmapBox control. Does not destroy the
    * bitmap (unless it becomes unreferenced); only disassociates it from this
    * %BitmapBox control.
    */
   void Clear()
   {
      SetBitmap( Bitmap::Null() );
   }

   /*!
    * Returns the margin in pixels that this %BitmapBox control reserves around
    * its bitmap.
    */
   int Margin() const;

   /*!
    * Sets a new margin \a m in pixels for this %BitmapBox control.
    */
   void SetMargin( int m );

   /*!
    * Sets the margin of this %BitmapBox control to zero pixels.
    */
   void ClearMargin()
   {
      SetMargin( 0 );
   }

   /*! #
    */
   bool IsAutoFitEnabled() const;

   /*! #
    */
   void EnableAutoFit( bool = true );

   /*! #
    */
   void DisableAutoFit( bool disable = true )
   {
      EnableAutoFit( !disable );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_BitmapBox_h

// ----------------------------------------------------------------------------
// EOF pcl/BitmapBox.h - Released 2025-04-07T08:52:44Z
