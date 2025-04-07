//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.1.3
// ----------------------------------------------------------------------------
// XISFPreferencesDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef XISFPreferencesDialog_h
#define XISFPreferencesDialog_h

#include "XISFFormat.h"
#include "XISFOptionsDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFPreferencesDialog : public XISFOptionsDialogBase
{
public:

   XISFFormat::EmbeddingOverrides overrides;

   XISFPreferencesDialog( const XISFFormat::EmbeddingOverrides&, const XISFOptions& );
   virtual ~XISFPreferencesDialog() = default;

private:

   VerticalSizer     Global_Sizer;

   GroupBox          FITSCompatibility_GroupBox;
   VerticalSizer     FITSCompatibility_Sizer;
      HorizontalSizer   StoreFITSKeywords_Sizer;
         CheckBox          StoreFITSKeywords_CheckBox;
      HorizontalSizer   IgnoreFITSKeywords_Sizer;
         CheckBox          IgnoreFITSKeywords_CheckBox;
      HorizontalSizer   ImportFITSKeywords_Sizer;
         CheckBox          ImportFITSKeywords_CheckBox;

   GroupBox          DataAlignment_GroupBox;
   VerticalSizer     DataAlignment_Sizer;
      HorizontalSizer   AlignmentSize_Sizer;
         Label             AlignmentSize_Label;
         SpinBox           AlignmentSize_SpinBox;
      HorizontalSizer   MaxInlineSize_Sizer;
         Label             MaxInlineSize_Label;
         SpinBox           MaxInlineSize_SpinBox;

   GroupBox          EmbeddedData_GroupBox;
   VerticalSizer     EmbeddedData_Sizer;
      HorizontalSizer      Properties_Sizer;
         CheckBox             Properties_CheckBox;
      HorizontalSizer      ICCProfile_Sizer;
         CheckBox             ICCProfile_CheckBox;
      HorizontalSizer      Thumbnail_Sizer;
         CheckBox             Thumbnail_CheckBox;
      HorizontalSizer      DisplayFunction_Sizer;
         CheckBox             DisplayFunction_CheckBox;
      HorizontalSizer      RGBWorkingSpace_Sizer;
         CheckBox             RGBWorkingSpace_CheckBox;
      HorizontalSizer      ProcessingHistory_Sizer;
         CheckBox             ProcessingHistory_CheckBox;
      HorizontalSizer      PreviewRects_Sizer;
         CheckBox             PreviewRects_CheckBox;

   PushButton        Reset_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // XISFPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF XISFPreferencesDialog.h - Released 2025-04-07T08:53:45Z
