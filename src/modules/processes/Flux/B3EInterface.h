//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// B3EInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __B3EInterface_h
#define __B3EInterface_h

#include <pcl/Button.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "B3EInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class B3EInterface : public ProcessInterface
{
public:

   B3EInterface();
   virtual ~B3EInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   B3EInstance m_instance;

   struct GUIData
   {
      GUIData( B3EInterface& );

      VerticalSizer     Global_Sizer;
         VerticalSizer        Input_Sizer;
            HorizontalSizer      InputImage1_Sizer;
               Label                InputImage1_Label;
               Edit                 InputImage1_Edit;
               ToolButton           InputImage1_ToolButton;
            HorizontalSizer      InputImage2_Sizer;
               Label                InputImage2_Label;
               Edit                 InputImage2_Edit;
               ToolButton           InputImage2_ToolButton;
         VerticalSizer        ProcessParameters_Sizer;
            NumericEdit          CenterInput1_NumericEdit;
            NumericEdit          CenterInput2_NumericEdit;
            NumericEdit          CenterOutput_NumericEdit;
            HorizontalSizer      IntensityUnits_Sizer;
               Label                IntensityUnits_Label;
               ComboBox             IntensityUnits_ComboBox;
         VerticalSizer        OutputData_Sizer;
            HorizontalSizer      OutputImages_Sizer;
               Label                OutputImages_Label;
               ComboBox             OutputImages_ComboBox;
            HorizontalSizer      OutOfRangeMask_Sizer;
               CheckBox             OutOfRangeMask_CheckBox;
      // Background Calibration 1
      SectionBar        BackgroundReference1_SectionBar;
      Control           BackgroundReference1_Control;
      VerticalSizer     BackgroundReference1_Sizer;
         HorizontalSizer   BackgroundReferenceView1_Sizer;
            Label             BackgroundReferenceView1_Label;
            Edit              BackgroundReferenceView1_Edit;
            ToolButton        BackgroundReferenceView1_ToolButton;
         NumericControl    BackgroundLow1_NumericControl;
         NumericControl    BackgroundHigh1_NumericControl;
         HorizontalSizer   OutputBackgroundReferenceMask1_Sizer;
            CheckBox          OutputBackgroundReferenceMask1_CheckBox;
         GroupBox          BackgroundROI1_GroupBox;
         VerticalSizer     BackgroundROI1_Sizer;
            HorizontalSizer   BackgroundROIRow11_Sizer;
               Label             BackgroundROIX01_Label;
               SpinBox           BackgroundROIX01_SpinBox;
               Label             BackgroundROIY01_Label;
               SpinBox           BackgroundROIY01_SpinBox;
            HorizontalSizer   BackgroundROIRow21_Sizer;
               Label             BackgroundROIWidth1_Label;
               SpinBox           BackgroundROIWidth1_SpinBox;
               Label             BackgroundROIHeight1_Label;
               SpinBox           BackgroundROIHeight1_SpinBox;
               PushButton        BackgroundROISelectPreview1_Button;
      // Background Calibration 2
      SectionBar        BackgroundReference2_SectionBar;
      Control           BackgroundReference2_Control;
      VerticalSizer     BackgroundReference2_Sizer;
         HorizontalSizer   BackgroundReferenceView2_Sizer;
            Label             BackgroundReferenceView2_Label;
            Edit              BackgroundReferenceView2_Edit;
            ToolButton        BackgroundReferenceView2_ToolButton;
         NumericControl    BackgroundLow2_NumericControl;
         NumericControl    BackgroundHigh2_NumericControl;
         HorizontalSizer   OutputBackgroundReferenceMask2_Sizer;
            CheckBox          OutputBackgroundReferenceMask2_CheckBox;
         GroupBox          BackgroundROI2_GroupBox;
         VerticalSizer     BackgroundROI2_Sizer;
            HorizontalSizer   BackgroundROIRow12_Sizer;
               Label             BackgroundROIX02_Label;
               SpinBox           BackgroundROIX02_SpinBox;
               Label             BackgroundROIY02_Label;
               SpinBox           BackgroundROIY02_SpinBox;
            HorizontalSizer   BackgroundROIRow22_Sizer;
               Label             BackgroundROIWidth2_Label;
               SpinBox           BackgroundROIWidth2_SpinBox;
               Label             BackgroundROIHeight2_Label;
               SpinBox           BackgroundROIHeight2_SpinBox;
               PushButton        BackgroundROISelectPreview2_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __EditCompleted( Edit& sender );
   void __EditCompleted_bkg( Edit& sender );
   void __Clicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ValueUpdated( NumericEdit& sender, double value );
   void __GetFocus( Control& sender );
   void __SpinValueUpdated( SpinBox& sender, int value );
   void __GroupBoxCheck( GroupBox& sender, bool checked );
   void __BackgroundReference_Check( SectionBar& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern B3EInterface* TheB3EInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __B3EInterface_h

// ----------------------------------------------------------------------------
// EOF B3EInterface.h - Released 2025-04-07T08:53:55Z
