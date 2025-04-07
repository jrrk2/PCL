//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// FITSRangeOptionsDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSRangeOptionsDialog_h
#define __FITSRangeOptionsDialog_h

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

class FITSRangeOptionsDialog : public Dialog
{
public:

   FITSRangeOptionsDialog( const FITSFormat::OutOfRangePolicyOptions&,
                           int bitsPerSample, double minSampleValue, double maxSampleValue );

   FITSFormat::OutOfRangePolicyOptions outOfRange;

private:

   VerticalSizer     Global_Sizer;

   Label             Info_Label;
   GroupBox          ReadRange_GroupBox;
   VerticalSizer     ReadRange_Sizer;
      NumericEdit       LowerRange_NumericEdit;
      NumericEdit       UpperRange_NumericEdit;
      HorizontalSizer   ReadRescaleMode_Sizer;
         Label             ReadRescaleMode_Label;
         ComboBox          ReadRescaleMode_ComboBox;
   HorizontalSizer   BottomSection_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSRangeOptionsDialog_h

// ----------------------------------------------------------------------------
// EOF FITSRangeOptionsDialog.h - Released 2025-04-07T08:53:45Z
