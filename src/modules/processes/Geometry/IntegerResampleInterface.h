//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __IntegerResampleInterface_h
#define __IntegerResampleInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ViewList.h>

#include "IntegerResampleInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class IntegerResampleInterface : public ProcessInterface
{
public:

   IntegerResampleInterface();
   virtual ~IntegerResampleInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;

private:

   IntegerResampleInstance m_instance;

   // Sample source dimensions in pixels.
   int sourceWidth = 1000;
   int sourceHeight = 1000;

   struct GUIData
   {
      GUIData( IntegerResampleInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        IntegerResample_SectionBar;
      Control           IntegerResample_Control;
      VerticalSizer     IntegerResample_Sizer;
         HorizontalSizer   ResampleFactor_Sizer;
            Label             ResampleFactor_Label;
            SpinBox           ResampleFactor_SpinBox;
         HorizontalSizer   Downsample_Sizer;
            RadioButton       Downsample_RadioButton;
            Label             DownsampleMode_Label;
            ComboBox          DownsampleMode_ComboBox;
         HorizontalSizer   Upsample_Sizer;
            RadioButton       Upsample_RadioButton;
         HorizontalSizer   GammaCorrection_Sizer;
            CheckBox          GammaCorrection_CheckBox;

      SectionBar        Dimensions_SectionBar;
      Control           Dimensions_Control;
      VerticalSizer     Dimensions_Sizer;
         HorizontalSizer   DimensionsRow1_Sizer;
            Label             SourcePixels_Label;
            Label             TargetPixels_Label;
            Label             TargetCentimeters_Label;
            Label             TargetInches_Label;
         HorizontalSizer   DimensionsRow2_Sizer;
            Label             Width_Label;
            NumericEdit       SourceWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPixels_NumericEdit;
            NumericEdit       TargetWidthCentimeters_NumericEdit;
            NumericEdit       TargetWidthInches_NumericEdit;
         HorizontalSizer   DimensionsRow3_Sizer;
            Label             Height_Label;
            NumericEdit       SourceHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPixels_NumericEdit;
            NumericEdit       TargetHeightCentimeters_NumericEdit;
            NumericEdit       TargetHeightInches_NumericEdit;
         HorizontalSizer   DimensionsRow4_Sizer;
            Label             SizeInfo_Label;

      SectionBar        Resolution_SectionBar;
      Control           Resolution_Control;
      VerticalSizer     Resolution_Sizer;
         HorizontalSizer   ResolutionRow1_Sizer;
            NumericEdit       HorizontalResolution_NumericEdit;
            NumericEdit       VerticalResolution_NumericEdit;
         HorizontalSizer   ResolutionRow2_Sizer;
            RadioButton       CentimeterUnits_RadioButton;
            RadioButton       InchUnits_RadioButton;
            CheckBox          ForceResolution_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ViewSelected( ViewList& sender, View& view );
   void e_ButtonClick( Button& sender, bool checked );
   void e_SpinBoxValueUpdated( SpinBox& sender, int value );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern IntegerResampleInterface* TheIntegerResampleInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegerResampleInterface_h

// ----------------------------------------------------------------------------
// EOF IntegerResampleInterface.h - Released 2025-04-07T08:53:55Z
