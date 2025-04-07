//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CheckBox.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_CheckBox_h
#define __PCL_CheckBox_h

/// \file pcl/CheckBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Button.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class CheckBox
 * \brief Client-side interface to a PixInsight %CheckBox control
 *
 * ### TODO: Write a detailed description for %CheckBox
 */
class PCL_CLASS CheckBox : public Button
{
public:

   /*!
    * Constructs a %CheckBox with the specified \a text, as a child control of
    * \a parent.
    */
   CheckBox( const String& text = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %CheckBox control.
    */
   ~CheckBox() override
   {
   }

   /*!
    * Returns \c false, since check boxes are not pushable buttons.
    */
   bool IsPushable() const override
   {
      return false;
   }

   /*!
    * Returns \c true, since check boxes are checkable buttons.
    */
   bool IsCheckable() const override
   {
      return true;
   }

   /*!
    * Returns \c true only if this check box can have three states.
    */
   bool IsTristateMode() const;

   /*!
    * Enables or disables \e tristate \e mode. In tristate mode, a check box
    * can have three states: checked, unchecked, and \e semi-checked (also
    * called \e third \e state).
    */
   void SetTristateMode( bool = true );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_CheckBox_h

// ----------------------------------------------------------------------------
// EOF pcl/CheckBox.h - Released 2025-04-07T08:52:44Z
