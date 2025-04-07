//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// LRGBCombinationInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LRGBCombinationInterface_h
#define __LRGBCombinationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "LRGBCombinationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class LRGBCombinationInterface : public ProcessInterface
{
public:

   LRGBCombinationInterface();
   virtual ~LRGBCombinationInterface();

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

   LRGBCombinationInstance m_instance;

   struct GUIData
   {
      GUIData( LRGBCombinationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Channels_SectionBar;
      Control           Channels_Control;
      VerticalSizer     Channels_Sizer;
         HorizontalSizer   L_Sizer;
            CheckBox          L_CheckBox;
            Label             L_Label;
            Edit              L_Edit;
            ToolButton        L_ToolButton;
         HorizontalSizer   R_Sizer;
            CheckBox          R_CheckBox;
            Label             R_Label;
            Edit              R_Edit;
            ToolButton        R_ToolButton;
         HorizontalSizer   G_Sizer;
            CheckBox          G_CheckBox;
            Label             G_Label;
            Edit              G_Edit;
            ToolButton        G_ToolButton;
         HorizontalSizer   B_Sizer;
            CheckBox          B_CheckBox;
            Label             B_Label;
            Edit              B_Edit;
            ToolButton        B_ToolButton;
         HorizontalSizer   InheritAstrometricSolution_Sizer;
            CheckBox          InheritAstrometricSolution_CheckBox;

      SectionBar        Weights_SectionBar;
      Control           Weights_Control;
      VerticalSizer     Weights_Sizer;
         NumericControl    L_Weight_NumericControl;
         NumericControl    R_Weight_NumericControl;
         NumericControl    G_Weight_NumericControl;
         NumericControl    B_Weight_NumericControl;
         HorizontalSizer   ClipHighlights_Sizer;
            CheckBox          ClipHighlights_CheckBox;

      SectionBar        MTF_SectionBar;
      Control           MTF_Control;
      VerticalSizer     MTF_Sizer;
         NumericControl    LuminanceMTF_NumericControl;
         NumericControl    SaturationMTF_NumericControl;

      SectionBar        NoiseReduction_SectionBar;
      Control           NoiseReduction_Control;
      VerticalSizer     NoiseReduction_Sizer;
         HorizontalSizer   NoiseReductionRow1_Sizer;
            Label             LayersRemoved_Label;
            SpinBox           LayersRemoved_SpinBox;
         HorizontalSizer   NoiseReductionRow2_Sizer;
            Label             LayersProtected_Label;
            SpinBox           LayersProtected_SpinBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __Channel_Click( Button& sender, bool checked );
   void __ChannelId_GetFocus( Control& sender );
   void __ChannelId_EditCompleted( Edit& sender );
   void __Channel_SelectSource_Click( Button& sender, bool checked );
   void __Channel_Weight_ValueUpdated( NumericEdit& sender, double value );
   void __ClipHighlights_Click( Button& sender, bool checked );
   void __MTF_ValueUpdated( NumericEdit& sender, double value );
   void __NoiseReduction_Check( SectionBar& sender, bool checked );
   void __NoiseReduction_ValueUpdated( SpinBox& sender, int value );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
   friend class  ChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern LRGBCombinationInterface* TheLRGBCombinationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LRGBCombinationInterface_h

// ----------------------------------------------------------------------------
// EOF LRGBCombinationInterface.h - Released 2025-04-07T08:53:55Z
