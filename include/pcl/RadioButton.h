//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/RadioButton.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_RadioButton_h
#define __PCL_RadioButton_h

/// \file pcl/RadioButton.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Button.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class RadioButton
 * \brief Client-side interface to a PixInsight %RadioButton control.
 *
 * ### TODO: Write a detailed description for %RadioButton
 */
class PCL_CLASS RadioButton : public Button
{
public:

   /*!
    * Constructs a %RadioButton with the specified \a text, as a child control
    * of \a parent.
    */
   RadioButton( const String& text = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %RadioButton control.
    */
   ~RadioButton() override
   {
   }

   /*!
    * Returns false, since radio buttons are not pushable buttons.
    */
   bool IsPushable() const override
   {
      return false;
   }

   /*!
    * Returns true, since radio buttons are checkable buttons.
    */
   bool IsCheckable() const override
   {
      return true;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_RadioButton_h

// ----------------------------------------------------------------------------
// EOF pcl/RadioButton.h - Released 2025-04-07T08:52:44Z
