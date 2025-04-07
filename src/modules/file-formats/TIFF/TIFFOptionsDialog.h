//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// TIFFOptionsDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __TIFFOptionsDialog_h
#define __TIFFOptionsDialog_h

#include "TIFF.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>
#include <pcl/TextBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class TIFFOptionsDialog : public Dialog
{
public:

   TIFFOptionsDialog( const pcl::ImageOptions&, const pcl::TIFFImageOptions& );

   pcl::ImageOptions     options;
   pcl::TIFFImageOptions tiffOptions;

private:

   HorizontalSizer   Global_Sizer;

   VerticalSizer     LeftPanel_Sizer;
      GroupBox          SampleFormat_GroupBox;
      VerticalSizer     SampleFormat_Sizer;
         RadioButton       UInt8_RadioButton;
         RadioButton       UInt16_RadioButton;
         RadioButton       UInt32_RadioButton;
         RadioButton       Float_RadioButton;
         RadioButton       Double_RadioButton;
      GroupBox          Compression_GroupBox;
      VerticalSizer     Compression_Sizer;
         RadioButton       NoCompression_RadioButton;
         RadioButton       ZIP_RadioButton;
         RadioButton       LZW_RadioButton;
      GroupBox          EmbeddedData_GroupBox;
      VerticalSizer     EmbeddedData_Sizer;
         CheckBox          ICCProfile_CheckBox;
      GroupBox          Miscellaneous_GroupBox;
      VerticalSizer     Miscellaneous_Sizer;
         CheckBox          Planar_CheckBox;
         CheckBox          AssociatedAlpha_CheckBox;
         CheckBox          PremultipliedAlpha_CheckBox;

   VerticalSizer     RightPanel_Sizer;
      GroupBox          ImageDescription_GroupBox;
      HorizontalSizer   ImageDescription_Sizer;
         TextBox           ImageDescription_TextBox;
      GroupBox          ImageCopyright_GroupBox;
      HorizontalSizer   ImageCopyright_Sizer;
         TextBox           ImageCopyright_TextBox;
      GroupBox          SoftwareDescription_GroupBox;
      HorizontalSizer   SoftwareDescription_Sizer;
         TextBox           SoftwareDescription_TextBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __TIFFOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF TIFFOptionsDialog.h - Released 2025-04-07T08:53:45Z
