//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// JPEGOptionsDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEGOptionsDialog_h
#define __JPEGOptionsDialog_h

#include "JPEG.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/CheckBox.h>
#include <pcl/Label.h>
#include <pcl/SpinBox.h>
#include <pcl/Slider.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class JPEGOptionsDialog : public Dialog
{
public:

   JPEGOptionsDialog( const ImageOptions&, const JPEGImageOptions& );

   ImageOptions      options;
   JPEGImageOptions  jpegOptions;

protected:

   VerticalSizer     Global_Sizer;

   GroupBox          JPEGCompression_GroupBox;
   VerticalSizer     Compression_Sizer;
      HorizontalSizer   CompressionRow1_Sizer;
         Label             Quality_Label;
         SpinBox           Quality_SpinBox;
         HorizontalSlider  Quality_Slider;
      HorizontalSizer   CompressionRow2_Sizer;
         CheckBox          OptimizedCoding_CheckBox;
      HorizontalSizer   CompressionRow3_Sizer;
         CheckBox          ArithmeticCoding_CheckBox;
      HorizontalSizer   CompressionRow4_Sizer;
         CheckBox          Progressive_CheckBox;
   GroupBox          EmbeddedData_GroupBox;
   HorizontalSizer   EmbeddedData_Sizer;
      HorizontalSizer   ICCProfile_Sizer;
         CheckBox          ICCProfile_CheckBox;
   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void SpinBox_ValueUpdated( SpinBox& sender, int value );
   void Slider_ValueUpdated( Slider& sender, int value );

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEGOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF JPEGOptionsDialog.h - Released 2025-04-07T08:53:45Z
