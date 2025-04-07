//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// FITSPreferencesDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSPreferencesDialog_h
#define __FITSPreferencesDialog_h

#include "FITSOptionsDialog.h"
#include "FITSFormat.h"

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/PushButton.h>
#include <pcl/CheckBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ComboBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSPreferencesDialog: public Dialog
{
public:

   FITSPreferencesDialog( const FITSFormat::OutOfRangePolicyOptions&, const FITSImageOptions& );

   FITSFormat::OutOfRangePolicyOptions outOfRange;
   FITSImageOptions                    fitsOptions;

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

   GroupBox          MiscellaneousOptions_GroupBox;
   VerticalSizer     MiscellaneousOptions_Sizer;
      HorizontalSizer   CoordinateOrigin_Sizer;
         Label             CoordinateOrigin_Label;
         ComboBox          CoordinateOrigin_ComboBox;
      HorizontalSizer   IntegerSignedness_Sizer;
         Label             IntegerSignedness_Label;
         ComboBox          IntegerSignedness_ComboBox;
      HorizontalSizer   UseRowOrderKeywords_Sizer;
         CheckBox          UseRowOrderKeywords_CheckBox;
      HorizontalSizer   WriteScalingKeywordsForSignedData_Sizer;
         CheckBox          WriteScalingKeywordsForSignedData_CheckBox;
      HorizontalSizer   SignedIntegersArePhysical_Sizer;
         CheckBox          SignedIntegersArePhysical_CheckBox;
      HorizontalSizer   CleanupHeaders_Sizer;
         CheckBox          CleanupHeaders_CheckBox;

   HorizontalSizer   BottomSection_Sizer;
      PushButton        Reset_PushButton;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF FITSPreferencesDialog.h - Released 2025-04-07T08:53:45Z
