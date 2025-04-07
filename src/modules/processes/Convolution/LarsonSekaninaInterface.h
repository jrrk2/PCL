//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// LarsonSekaninaInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LarsonSekaninaInterface_h
#define __LarsonSekaninaInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/NumericControl.h>
#include <pcl/GroupBox.h>
#include <pcl/SectionBar.h>
#include <pcl/Control.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>

#include "LarsonSekaninaInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class LarsonSekaninaInterface : public ProcessInterface
{
public:

   LarsonSekaninaInterface();
   virtual ~LarsonSekaninaInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   LarsonSekaninaInstance instance;

   struct GUIData
   {
      GUIData( LarsonSekaninaInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Filter_SectionBar;
      Control           Filter_Control;
      VerticalSizer     Filter_Sizer;
         HorizontalSizer   Radius_Sizer;
            NumericEdit       Radius_NumericEdit;
            VerticalSizer     RadiusSliders_Sizer;
            Slider               RadiusCoarse_Slider;
            Slider               RadiusFine_Slider;
         HorizontalSizer   Angle_Sizer;
            NumericEdit       Angle_NumericEdit;
            VerticalSizer     AngleSliders_Sizer;
            Slider               AngleCoarse_Slider;
            Slider               AngleFine_Slider;
         NumericEdit       X_NumericEdit;
         NumericEdit       Y_NumericEdit;
         HorizontalSizer   Interpolation_Sizer;
           Label             Interpolation_Label;
           ComboBox          Interpolation_ComboBox;

      SectionBar        ApplyMode_SectionBar;
      Control           ApplyMode_Control;
      VerticalSizer     ApplyMode_Sizer;
         NumericControl    Amount_NumericControl;
         NumericControl    Threshold_NumericControl;
         NumericControl    Deringing_NumericControl;
         HorizontalSizer   UseLuminance_Sizer;
            CheckBox          UseLuminance_CheckBox;
            CheckBox          HighPassMode_CheckBox;

      SectionBar        Range_SectionBar;
      Control           Range_Control;
      VerticalSizer     Range_Sizer;
         NumericControl    RangeLow_NumericControl;
         NumericControl    RangeHigh_NumericControl;
         HorizontalSizer   Disable_Sizer;
            CheckBox          Disable_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateFilterControls();
   void UpdateApplyModeControls();
   void UpdateRangeControls();

   void __RadiusEdit_ValueUpdated( NumericEdit& sender, double value );
   void __Radius_ValueUpdated( Slider& sender, int value );
   void __AngleEdit_ValueUpdated( NumericEdit& sender, double value );
   void __Angle_ValueUpdated( Slider& sender, int value );
   void __Coords_ValueUpdated( NumericEdit& sender, double value );
   void __Interpolation_ItemSelected( ComboBox& sender, int itemIndex );

   void __Amount_ValueUpdated( NumericEdit& sender, double value );
   void __Threshold_ValueUpdated( NumericEdit& sender, double value );
   void __Deringing_ValueUpdated( NumericEdit& sender, double value );
   void __UseLuminance_Click( Button& sender, bool checked );
   void __HighPassMode_Click( Button& sender, bool checked );

   void __Range_ValueUpdated( NumericEdit& sender, double value );
   void __Disable_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern LarsonSekaninaInterface* TheLarsonSekaninaInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LarsonSekaninaInterface_h

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaInterface.h - Released 2025-04-07T08:53:55Z
