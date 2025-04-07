//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// TIFFPreferencesDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __TIFFPreferencesDialog_h
#define __TIFFPreferencesDialog_h

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>
#include <pcl/TextBox.h>
#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>

#include "TIFFFormat.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class TIFFPreferencesDialog: public Dialog
{
public:

   TIFFPreferencesDialog(
      const TIFFFormat::OutOfRangePolicyOptions&,
      const TIFFFormat::EmbeddingOverrides&, const TIFFImageOptions& );

   TIFFFormat::OutOfRangePolicyOptions outOfRange;
   TIFFFormat::EmbeddingOverrides      overrides;
   TIFFImageOptions                    tiffOptions;

private:

   VerticalSizer     Global_Sizer;

   GroupBox          ReadRange_GroupBox;
   VerticalSizer     ReadRange_Sizer;
      NumericEdit       LowerRange_NumericEdit;
      NumericEdit       UpperRange_NumericEdit;
   GroupBox          ReadRangeOptions_GroupBox;
   VerticalSizer     ReadRangeOptions_Sizer;
      HorizontalSizer   ReadRangeMode_Sizer;
         Label             ReadRangeMode_Label;
         ComboBox          ReadRangeMode_ComboBox;
      HorizontalSizer   ReadRescaleMode_Sizer;
         Label             ReadRescaleMode_Label;
         ComboBox          ReadRescaleMode_ComboBox;
   HorizontalSizer   BottomSection_Sizer;
      VerticalSizer     LeftPanel_Sizer;
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
         GroupBox          ImageCopyright_GroupBox;
         HorizontalSizer   ImageCopyright_Sizer;
            TextBox           ImageCopyright_TextBox;
         GroupBox          SoftwareDescription_GroupBox;
         HorizontalSizer   SoftwareDescription_Sizer;
            TextBox           SoftwareDescription_TextBox;
         HorizontalSizer   Buttons_Sizer;
            PushButton        Reset_PushButton;
            PushButton        OK_PushButton;
            PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __TIFFPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF TIFFPreferencesDialog.h - Released 2025-04-07T08:53:45Z
