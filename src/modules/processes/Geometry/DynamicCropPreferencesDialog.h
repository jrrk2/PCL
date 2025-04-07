//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropPreferencesDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __DynamicCropPreferencesDialog_h
#define __DynamicCropPreferencesDialog_h

#include <pcl/Color.h>
#include <pcl/Dialog.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/Slider.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class DynamicCropPreferencesDialog : public Dialog
{
public:

   DynamicCropPreferencesDialog();

private:

   RGBA savedColor;

   VerticalSizer  Global_Sizer;

   GroupBox          FillColor_GroupBox;
   HorizontalSizer   FillColor_Sizer;
      RadioButton       Black_RadioButton;
      RadioButton       White_RadioButton;
      Label             Alpha_Label;
      HorizontalSlider  Alpha_Slider;

   HorizontalSizer   Buttons_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

      void Update();

   void __ValueUpdated( Slider& sender, int value );
   void __Click( Button& sender, bool checked );
   void __Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicCropPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF DynamicCropPreferencesDialog.h - Released 2025-04-07T08:53:56Z
