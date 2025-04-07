//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FindingChartInterface_h
#define __FindingChartInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ColorComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "FindingChartInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FindingChartInterface : public ProcessInterface
{
public:

   FindingChartInterface();
   virtual ~FindingChartInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   FindingChartInstance m_instance;

   struct GUIData
   {
      GUIData( FindingChartInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   ChartSize_Sizer;
         Label             ChartSize_Label;
         SpinBox           ChartSize_SpinBox;
      HorizontalSizer   AutoChartResolution_Sizer;
         CheckBox          AutoChartResolution_CheckBox;
      NumericControl    ChartResolution_NumericControl;
      HorizontalSizer   AutoChartMaxMagnitude_Sizer;
         CheckBox          AutoChartMaxMagnitude_CheckBox;
      NumericControl    ChartMaxMagnitude_NumericControl;
      HorizontalSizer   GenerateBitmapFile_Sizer;
         CheckBox          GenerateBitmapFile_CheckBox;
      HorizontalSizer   OutputDirectory_Sizer;
         Label             OutputDirectory_Label;
         Edit              OutputDirectory_Edit;
         ToolButton        OutputDirectory_ToolButton;
      HorizontalSizer   OutputSuffix_Sizer;
         Label             OutputSuffix_Label;
         Edit              OutputSuffix_Edit;
      HorizontalSizer   CloseOutputWindow_Sizer;
         CheckBox          CloseOutputWindow_CheckBox;

      SectionBar        Contents_SectionBar;
      Control           Contents_Control;
      VerticalSizer     Contents_Sizer;
         HorizontalSizer   DrawGrid_Sizer;
            CheckBox          DrawGrid_CheckBox;
         HorizontalSizer   DrawEcliptic_Sizer;
            CheckBox          DrawEcliptic_CheckBox;
         HorizontalSizer   DrawGalacticEquator_Sizer;
            CheckBox          DrawGalacticEquator_CheckBox;
         HorizontalSizer   DrawConstellationBorders_Sizer;
            CheckBox          DrawConstellationBorders_CheckBox;
         HorizontalSizer   DrawConstellationLines_Sizer;
            CheckBox          DrawConstellationLines_CheckBox;
         HorizontalSizer   DrawConstellationNames_Sizer;
            CheckBox          DrawConstellationNames_CheckBox;
         HorizontalSizer   DrawStars_Sizer;
            CheckBox          DrawStars_CheckBox;
         HorizontalSizer   DrawStarNames_Sizer;
            CheckBox          DrawStarNames_CheckBox;
         HorizontalSizer   DrawMessierObjects_Sizer;
            CheckBox          DrawMessierObjects_CheckBox;
         HorizontalSizer   DrawNGCObjects_Sizer;
            CheckBox          DrawNGCObjects_CheckBox;

      SectionBar        Colors_SectionBar;
      Control           Colors_Control;
      VerticalSizer     Colors_Sizer;
         HorizontalSizer   ImageRegionColor_Sizer;
            Label             ImageRegionColor_Label;
            ColorComboBox     ImageRegionColor_ComboBox;
            Control           ImageRegionColor_Control;
         HorizontalSizer   ImageRegionBorderColor_Sizer;
            Label             ImageRegionBorderColor_Label;
            ColorComboBox     ImageRegionBorderColor_ComboBox;
            Control           ImageRegionBorderColor_Control;
         HorizontalSizer   GridColor_Sizer;
            Label             GridColor_Label;
            ColorComboBox     GridColor_ComboBox;
            Control           GridColor_Control;
         HorizontalSizer   GridTextColor_Sizer;
            Label             GridTextColor_Label;
            ColorComboBox     GridTextColor_ComboBox;
            Control           GridTextColor_Control;
         HorizontalSizer   EclipticColor_Sizer;
            Label             EclipticColor_Label;
            ColorComboBox     EclipticColor_ComboBox;
            Control           EclipticColor_Control;
         HorizontalSizer   EclipticTextColor_Sizer;
            Label             EclipticTextColor_Label;
            ColorComboBox     EclipticTextColor_ComboBox;
            Control           EclipticTextColor_Control;
         HorizontalSizer   GalacticEquatorColor_Sizer;
            Label             GalacticEquatorColor_Label;
            ColorComboBox     GalacticEquatorColor_ComboBox;
            Control           GalacticEquatorColor_Control;
         HorizontalSizer   GalacticEquatorTextColor_Sizer;
            Label             GalacticEquatorTextColor_Label;
            ColorComboBox     GalacticEquatorTextColor_ComboBox;
            Control           GalacticEquatorTextColor_Control;
         HorizontalSizer   ConstellationBorderColor_Sizer;
            Label             ConstellationBorderColor_Label;
            ColorComboBox     ConstellationBorderColor_ComboBox;
            Control           ConstellationBorderColor_Control;
         HorizontalSizer   ConstellationLineColor_Sizer;
            Label             ConstellationLineColor_Label;
            ColorComboBox     ConstellationLineColor_ComboBox;
            Control           ConstellationLineColor_Control;
         HorizontalSizer   ConstellationTextColor_Sizer;
            Label             ConstellationTextColor_Label;
            ColorComboBox     ConstellationTextColor_ComboBox;
            Control           ConstellationTextColor_Control;
         HorizontalSizer   StarTextColor_Sizer;
            Label             StarTextColor_Label;
            ColorComboBox     StarTextColor_ComboBox;
            Control           StarTextColor_Control;
         HorizontalSizer   MessierColor_Sizer;
            Label             MessierColor_Label;
            ColorComboBox     MessierColor_ComboBox;
            Control           MessierColor_Control;
         HorizontalSizer   NGCColor_Sizer;
            Label             NGCColor_Label;
            ColorComboBox     NGCColor_ComboBox;
            Control           NGCColor_Control;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_RealValueUpdated( NumericEdit& sender, double value );
   void e_IntegerValueUpdated( SpinBox& sender, int value );
   void e_ItemClicked( Button& sender, bool checked );
   void e_EditGetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );
   void e_ColorSelected( ColorComboBox& sender, RGBA color );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern FindingChartInterface* TheFindingChartInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FindingChartInterface_h

// ----------------------------------------------------------------------------
// EOF FindingChartInterface.h - Released 2025-04-07T08:53:55Z
