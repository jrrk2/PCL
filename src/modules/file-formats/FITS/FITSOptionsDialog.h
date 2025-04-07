//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// FITSOptionsDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSOptionsDialog_h
#define __FITSOptionsDialog_h

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>

#include "FITS.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSOptionsDialog : public Dialog
{
public:

   FITSOptionsDialog( const ImageOptions&, const FITSImageOptions& );

   ImageOptions     options;
   FITSImageOptions fitsOptions;

private:

   VerticalSizer     Global_Sizer;

   GroupBox          SampleFormat_GroupBox;
   HorizontalSizer   SampleFormat_Sizer;
      VerticalSizer     SampleFormatLeft_Sizer;
         RadioButton       UInt8_RadioButton;
         RadioButton       Int16_RadioButton;
         RadioButton       Int32_RadioButton;
         CheckBox          UnsignedIntegers_Checkbox;
      VerticalSizer     SampleFormatRight_Sizer;
         RadioButton       Float_RadioButton;
         RadioButton       Double_RadioButton;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF FITSOptionsDialog.h - Released 2025-04-07T08:53:45Z
