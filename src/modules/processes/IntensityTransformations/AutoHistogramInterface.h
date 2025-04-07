//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AutoHistogramInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AutoHistogramInterface_h
#define __AutoHistogramInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/RadioButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>

#include "AutoHistogramInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class AutoHistogramInterface : public ProcessInterface
{
public:

   AutoHistogramInterface();
   virtual ~AutoHistogramInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const pcl::DPoint&, double R, double G, double B, double A ) override;

private:

   AutoHistogramInstance m_instance;

   struct GUIData
   {
      GUIData( AutoHistogramInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        ClipParameters_SectionBar;
      Control           ClipParameters_Control;
      VerticalSizer     ClipParameters_Sizer;
         HorizontalSizer   ClipMode_Sizer;
            RadioButton       ClipTogether_RadioButton;
            RadioButton       ClipSeparate_RadioButton;
         HorizontalSizer   ClipValues_Sizer;
            GroupBox          ClipLow_GroupBox;
            VerticalSizer     ClipLow_Sizer;
               NumericControl    ClipLowRK_NumericControl;
               NumericControl    ClipLowG_NumericControl;
               NumericControl    ClipLowB_NumericControl;
            GroupBox          ClipHigh_GroupBox;
            VerticalSizer     ClipHigh_Sizer;
               NumericControl    ClipHighRK_NumericControl;
               NumericControl    ClipHighG_NumericControl;
               NumericControl    ClipHighB_NumericControl;

      SectionBar      StretchParameters_SectionBar;
      Control         StretchParameters_Control;
      VerticalSizer   StretchParameters_Sizer;
         HorizontalSizer   StretchMethod_Sizer;
            Label             StretchMethod_Label;
            ComboBox          StretchMethod_ComboBox;
         HorizontalSizer   StretchMode_Sizer;
            RadioButton       StretchTogether_RadioButton;
            RadioButton       StretchSeparate_RadioButton;
         HorizontalSizer   TargetMedian_Sizer;
            VerticalSizer     TargetMedianValues_Sizer;
               NumericControl    TargetMedianRK_NumericControl;
               NumericControl    TargetMedianG_NumericControl;
               NumericControl    TargetMedianB_NumericControl;
            Control           ColorSample_Control;
         HorizontalSizer   CaptureReadouts_Sizer;
            CheckBox          CaptureReadouts_CheckBox;
         PushButton        SetToActiveImage_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __Click( Button& sender, bool checked );
   void __CheckSection( SectionBar& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ValueUpdated( NumericEdit& sender, double value );
   void __Paint( Control& sender, const Rect& updateRect );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AutoHistogramInterface* TheAutoHistogramInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AutoHistogramInterface_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramInterface.h - Released 2025-04-07T08:53:56Z
