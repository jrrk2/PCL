//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.1.3
// ----------------------------------------------------------------------------
// XISFOptionsDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef XISFOptionsDialog_h
#define XISFOptionsDialog_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
//#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFOptionsDialogBase : public Dialog
{
public:

   XISFOptions options;

   XISFOptionsDialogBase( const XISFOptions&, const char* labelForWidth = nullptr );

protected:

   int m_labelWidth = 0;

   GroupBox          DataCompression_GroupBox;
   VerticalSizer     DataCompression_Sizer;
      HorizontalSizer   CompressionCodec_Sizer;
         Label             CompressionCodec_Label;
         ComboBox          CompressionCodec_ComboBox;
      HorizontalSizer   CompressionLevel_Sizer;
         Label             CompressionLevel_Label;
         SpinBox           CompressionLevel_SpinBox;
      HorizontalSizer   CompressionShuffle_Sizer;
         CheckBox          CompressionShuffle_CheckBox;

   GroupBox          Security_GroupBox;
   VerticalSizer     Security_Sizer;
      HorizontalSizer   Checksums_Sizer;
         Label             Checksums_Label;
         ComboBox          Checksums_ComboBox;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   int CompressionCodecToComboBoxItem( XISF::block_compression );
   XISF::block_compression ComboBoxItemToCompressionCodec( int );

   int ChecksumAlgorithmToComboBoxItem( XISF::block_checksum );
   XISF::block_checksum ComboBoxItemToChecksumAlgorithm( int );

   void GetBaseParameters();

   void Base_Button_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

class XISFOptionsDialog : public XISFOptionsDialogBase
{
public:

   ImageOptions imageOptions;
   IsoString    outputHints;

   XISFOptionsDialog( const ImageOptions&, const XISFOptions&, const IsoString& = IsoString() );

private:

   VerticalSizer     Global_Sizer;

   GroupBox          SampleFormat_GroupBox;
   VerticalSizer     SampleFormat_Sizer;
      HorizontalSizer   UInt8_Sizer;
         RadioButton       UInt8_RadioButton;
      HorizontalSizer   UInt16_Sizer;
         RadioButton       UInt16_RadioButton;
      HorizontalSizer   UInt32_Sizer;
         RadioButton       UInt32_RadioButton;
      HorizontalSizer   Float_Sizer;
         RadioButton       Float_RadioButton;
      HorizontalSizer   Double_Sizer;
         RadioButton       Double_RadioButton;

   /*
    * ### TODO: Core 1.8.x: Support hints in format queries.
    *
   GroupBox          OutputHints_GroupBox;
   HorizontalSizer   OutputHints_Sizer;
      Label             OutputHints_Label;
      Edit              OutputHints_Edit;
   */

   GroupBox          EmbeddedData_GroupBox;
   VerticalSizer     EmbeddedData_Sizer;
      HorizontalSizer      Properties_Sizer;
         CheckBox             Properties_CheckBox;
      HorizontalSizer      FITSKeywords_Sizer;
         CheckBox             FITSKeywords_CheckBox;
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

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // XISFOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF XISFOptionsDialog.h - Released 2025-04-07T08:53:45Z
