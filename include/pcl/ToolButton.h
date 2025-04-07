//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ToolButton.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ToolButton_h
#define __PCL_ToolButton_h

/// \file pcl/ToolButton.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Button.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ToolButton
 * \brief Client-side interface to a PixInsight %ToolButton control.
 *
 * ### TODO: Write a detailed description for %ToolButton
 */
class PCL_CLASS ToolButton : public Button
{
public:

   /*!
    * Constructs a %ToolButton with the specified \a text, \a icon and \a
    * checkable state, as a child control of \a parent.
    */
   ToolButton( const String& text = String(),
               const pcl::Bitmap& icon = Bitmap::Null(),
               bool checkable = false,
               Control& parent = Control::Null() );

   /*!
    * Destroys a %ToolButton control.
    */
   ~ToolButton() override
   {
   }

   /*!
    * Returns \c true, since tool buttons are pushable buttons.
    */
   bool IsPushable() const override
   {
      return true;
   }

   /*!
    * Returns true iff this %ToolButton is \a e checkable.
    *
    * \sa SetCheckable()
    */
   bool IsCheckable() const override;

   /*!
    * Enables or disables the <em>checkable state</em> of this %ToolButton.
    *
    * \sa IsCheckable()
    */
   void SetCheckable( bool = true );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ToolButton_h

// ----------------------------------------------------------------------------
// EOF pcl/ToolButton.h - Released 2025-04-07T08:52:44Z
