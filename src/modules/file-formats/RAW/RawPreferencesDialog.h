//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawPreferencesDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RawPreferencesDialog_h
#define __RawPreferencesDialog_h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "RawPreferences.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RawPreferencesDialog : public Dialog
{
public:

   RawPreferencesDialog( RawPreferences& );

private:

   RawPreferences&   preferences;
   RawPreferences    savedPreferences;

   VerticalSizer     Global_Sizer;

      GroupBox          OutputOptions_GroupBox;
      VerticalSizer     OutputOptions_Sizer;
         HorizontalSizer   CreateSuperPixels_Sizer;
            CheckBox          CreateSuperPixels_CheckBox;
         HorizontalSizer   OutputRawRGB_Sizer;
            CheckBox          OutputRawRGB_CheckBox;
         HorizontalSizer   OutputCFA_Sizer;
            CheckBox          OutputCFA_CheckBox;
         HorizontalSizer   NoAutoFlip_Sizer;
            CheckBox          NoAutoFlip_CheckBox;
         HorizontalSizer   NoAutoCrop_Sizer;
            CheckBox          NoAutoCrop_CheckBox;
         HorizontalSizer   NoBlackPointCorrection_Sizer;
            CheckBox          NoBlackPointCorrection_CheckBox;
         HorizontalSizer   NoClipHighlights_Sizer;
            CheckBox          NoClipHighlights_CheckBox;
         HorizontalSizer   ForceFocalLength_Sizer;
            CheckBox          ForceFocalLength_CheckBox;
         NumericEdit       FocalLength_NumericEdit;
         HorizontalSizer   ForceAperture_Sizer;
            CheckBox          ForceAperture_CheckBox;
         NumericEdit       Aperture_NumericEdit;
         NumericControl    NoiseThreshold_NumericControl;

      GroupBox          Interpolation_GroupBox;
      VerticalSizer     Interpolation_Sizer;
         HorizontalSizer   Bilinear_Sizer;
            RadioButton       Bilinear_RadioButton;
         HorizontalSizer   VNG_Sizer;
            RadioButton       VNG_RadioButton;
         HorizontalSizer   PPG_Sizer;
            RadioButton       PPG_RadioButton;
         HorizontalSizer   AHD_Sizer;
            RadioButton       AHD_RadioButton;
         HorizontalSizer   DCB_Sizer;
            RadioButton       DCB_RadioButton;
         HorizontalSizer   DCBIterations_Sizer;
            Label             DCBIterations_Label;
            SpinBox           DCBIterations_SpinBox;
         HorizontalSizer   DCBRefinement_Sizer;
            CheckBox          DCBRefinement_CheckBox;
         HorizontalSizer   DHT_Sizer;
            RadioButton       DHT_RadioButton;
         HorizontalSizer   AAHD_Sizer;
            RadioButton       AAHD_RadioButton;
         HorizontalSizer   HalfSize_Sizer;
            RadioButton       HalfSize_RadioButton;
         HorizontalSizer   InterpolateAs4Colors_Sizer;
            CheckBox          InterpolateAs4Colors_CheckBox;
         HorizontalSizer   FBDDNoiseReduction_Sizer;
            Label             FBDDNoiseReduction_Label;
            SpinBox           FBDDNoiseReduction_SpinBox;

      GroupBox          WhiteBalance_GroupBox;
      VerticalSizer     WhiteBalance_Sizer;
         HorizontalSizer   UseAutoWhiteBalance_Sizer;
            CheckBox          UseAutoWhiteBalance_CheckBox;
         HorizontalSizer   UseCameraWhiteBalance_Sizer;
            CheckBox          UseCameraWhiteBalance_CheckBox;
         HorizontalSizer   NoWhiteBalance_Sizer;
            CheckBox          NoWhiteBalance_CheckBox;

      HorizontalSizer   BottomRow_Sizer;
         PushButton        PureRaw_PushButton;
         PushButton        PureRGB_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void NumericControl_ValueUpdated( NumericControl& sender, double value );
   void SpinBox_ValueUpdated( SpinBox& sender, int value );
   void Button_Click( Button& sender, bool checked );
   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RawPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF RawPreferencesDialog.h - Released 2025-04-07T08:53:45Z
