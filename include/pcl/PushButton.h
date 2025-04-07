//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PushButton.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_PushButton_h
#define __PCL_PushButton_h

/// \file pcl/PushButton.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Button.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PushButton
 * \brief Client-side interface to a PixInsight %PushButton control.
 *
 * ### TODO: Write a detailed description for %PushButton
 */
class PCL_CLASS PushButton : public Button
{
public:

   /*!
    * Constructs a %PushButton with the specified \a text and \a icon, as a
    * child control of \a parent.
    */
   PushButton( const String& text = String(),
               const pcl::Bitmap& icon = Bitmap::Null(),
               Control& parent = Control::Null() );

   /*!
    * Destroys a %PushButton control.
    */
   ~PushButton() override
   {
   }

   /*!
    * Returns true, since push buttons are pushable buttons.
    */
   bool IsPushable() const override
   {
      return true;
   }

   /*!
    * Returns false, since push buttons are not checkable buttons.
    */
   bool IsCheckable() const override
   {
      return false;
   }

   /*!
    * Returns true iff this button has been selected as the <em>default
    * button</em> of its parent dialog.
    *
    * The default button is activated when the user presses the Return or Enter
    * keys while a modal dialog has the keyboard focus (and these keys are not
    * captured by other sibling controls).
    */
   bool IsDefault() const;

   /*!
    * Selectes this button as the <em>default button</em> of its parent dialog.
    * If this button is not a child control of a modal dialog, caling this
    * member function has no effect.
    */
   void SetDefault( bool = true );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_PushButton_h

// ----------------------------------------------------------------------------
// EOF pcl/PushButton.h - Released 2025-04-07T08:52:44Z
