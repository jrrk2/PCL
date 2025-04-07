//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef e_SubframeSelectorInterface_h
#define e_SubframeSelectorInterface_h

#include <pcl/BitmapBox.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorInterface : public ProcessInterface
{
public:

   SubframeSelectorInterface();
   virtual ~SubframeSelectorInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void EditPreferences() override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

   enum SubframeSortingCriterion
   {
      SortByTimeAscending,
      SortByTimeDescending,
      SortByPathAscending,
      SortByPathDescending
   };

private:

   SubframeSelectorInstance m_instance;
   bool                     m_expressionsWasVisible = true;
   bool                     m_measurementsWasVisible = true;

   struct GUIData
   {
      GUIData( SubframeSelectorInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   Routine_Sizer;
         Label             Routine_Label;
         ComboBox          Routine_ComboBox;
         ToolButton        GoToExpressionsWindow_ToolButton;
         ToolButton        GoToMeasurementsWindow_ToolButton;

      SectionBar        SubframeImages_SectionBar;
      Control           SubframeImages_Control;
      HorizontalSizer   SubframeImages_Sizer;
         TreeBox           SubframeImages_TreeBox;
         VerticalSizer     SubframeButtons_Sizer;
            PushButton        SubframeImages_AddFiles_PushButton;
            PushButton        SubframeImages_AddDrizzleFiles_PushButton;
            PushButton        SubframeImages_ClearDrizzleFiles_PushButton;
            PushButton        SubframeImages_AddLocalNormalizationFiles_PushButton;
            PushButton        SubframeImages_ClearLocalNormalizationFiles_PushButton;
            PushButton        SubframeImages_Sort_PushButton;
            PushButton        SubframeImages_SelectAll_PushButton;
            PushButton        SubframeImages_Invert_PushButton;
            PushButton        SubframeImages_Toggle_PushButton;
            PushButton        SubframeImages_Remove_PushButton;
            PushButton        SubframeImages_Clear_PushButton;
            CheckBox          SubframeImages_StaticDataTargets_CheckBox;
            CheckBox          SubframeImages_FullPaths_CheckBox;
            CheckBox          SubframeImages_FileCache_CheckBox;

      SectionBar        SystemParameters_SectionBar;
      Control           SystemParameters_Control;
      VerticalSizer     SystemParameters_Sizer;
         HorizontalSizer   SystemParameters_SubframeScale_Sizer;
            NumericControl    SystemParameters_SubframeScale_Control;
            Label             SystemParameters_SubframeScale_Unit_Label;
         HorizontalSizer   SystemParameters_CameraGain_Sizer;
            NumericControl    SystemParameters_CameraGain_Control;
            Label             SystemParameters_CameraGain_Unit_Label;
         HorizontalSizer   SystemParameters_CameraResolution_Sizer;
            Label             SystemParameters_CameraResolution_Label;
            ComboBox          SystemParameters_CameraResolution_ComboBox;
         HorizontalSizer   SystemParameters_SiteLocalMidnight_Sizer;
            Label             SystemParameters_SiteLocalMidnight_Label;
            SpinBox           SystemParameters_SiteLocalMidnight_SpinBox;
            Label             SystemParameters_SiteLocalMidnight_Unit_Label;
         HorizontalSizer   SystemParameters_ScaleUnit_Sizer;
            Label             SystemParameters_ScaleUnit_Label;
            ComboBox          SystemParameters_ScaleUnit_ComboBox;
         HorizontalSizer   SystemParameters_DataUnit_Sizer;
            Label             SystemParameters_DataUnit_Label;
            ComboBox          SystemParameters_DataUnit_ComboBox;

      SectionBar        StarDetectorParameters_SectionBar;
      Control           StarDetectorParameters_Control;
      VerticalSizer     StarDetectorParameters_Sizer;
         HorizontalSizer   StarDetectorParameters_StructureLayers_Sizer;
            Label             StarDetectorParameters_StructureLayers_Label;
            SpinBox           StarDetectorParameters_StructureLayers_SpinBox;
         HorizontalSizer   StarDetectorParameters_NoiseLayers_Sizer;
            Label             StarDetectorParameters_NoiseLayers_Label;
            SpinBox           StarDetectorParameters_NoiseLayers_SpinBox;
         HorizontalSizer   StarDetectorParameters_MinStructureSize_Sizer;
            Label             StarDetectorParameters_MinStructureSize_Label;
            SpinBox           StarDetectorParameters_MinStructureSize_SpinBox;
         HorizontalSizer   StarDetectorParameters_HotPixelFilterRadius_Sizer;
            Label             StarDetectorParameters_HotPixelFilterRadius_Label;
            SpinBox           StarDetectorParameters_HotPixelFilterRadius_SpinBox;
         HorizontalSizer   StarDetectorParameters_NoiseReductionFilterRadius_Sizer;
            Label             StarDetectorParameters_NoiseReductionFilterRadius_Label;
            SpinBox           StarDetectorParameters_NoiseReductionFilterRadius_SpinBox;
         NumericControl    StarDetectorParameters_Sensitivity_Control;
         NumericControl    StarDetectorParameters_PeakResponse_Control;
         NumericControl    StarDetectorParameters_BrightThreshold_Control;
         NumericControl    StarDetectorParameters_MaxDistortion_Control;
         NumericControl    StarDetectorParameters_UpperLimit_Control;
         HorizontalSizer   StarDetectorParameters_AllowClusteredSources_Sizer;
            CheckBox          StarDetectorParameters_AllowClusteredSources_CheckBox;
         HorizontalSizer   StarDetectorParameters_PSFFit_Sizer;
            Label             StarDetectorParameters_PSFFit_Label;
            ComboBox          StarDetectorParameters_PSFFit_ComboBox;
         HorizontalSizer   StarDetectorParameters_PSFFitCircular_Sizer;
            CheckBox          StarDetectorParameters_PSFFitCircular_CheckBox;
         HorizontalSizer   StarDetectorParameters_MaxPSFFits_Sizer;
            Label             StarDetectorParameters_MaxPSFFits_Label;
            SpinBox           StarDetectorParameters_MaxPSFFits_SpinBox;

      SectionBar        RegionOfInterest_SectionBar;
      Control           RegionOfInterest_Control;
      VerticalSizer     RegionOfInterest_Sizer;
         HorizontalSizer   RegionOfInterestRow1_Sizer;
            Label             RegionOfInterestX0_Label;
            SpinBox           RegionOfInterestX0_SpinBox;
            Label             RegionOfInterestY0_Label;
            SpinBox           RegionOfInterestY0_SpinBox;
         HorizontalSizer   RegionOfInterestRow2_Sizer;
            Label             RegionOfInterestWidth_Label;
            SpinBox           RegionOfInterestWidth_SpinBox;
            Label             RegionOfInterestHeight_Label;
            SpinBox           RegionOfInterestHeight_SpinBox;
            PushButton        RegionOfInterestSelectPreview_Button;

      SectionBar        Pedestal_SectionBar;
      Control           Pedestal_Control;
      VerticalSizer     Pedestal_Sizer;
         HorizontalSizer   PedestalMode_Sizer;
            Label             PedestalMode_Label;
            ComboBox          PedestalMode_ComboBox;
         HorizontalSizer   PedestalValue_Sizer;
            Label             PedestalValue_Label;
            SpinBox           PedestalValue_SpinBox;
         HorizontalSizer   PedestalKeyword_Sizer;
            Label             PedestalKeyword_Label;
            Edit              PedestalKeyword_Edit;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;
         HorizontalSizer   OutputHints_Sizer;
            Label             OutputHints_Label;
            Edit              OutputHints_Edit;

      SectionBar        OutputFiles_SectionBar;
      Control           OutputFiles_Control;
      VerticalSizer     OutputFiles_Sizer;
         HorizontalSizer   OutputDirectory_Sizer;
            Label             OutputDirectory_Label;
            Edit              OutputDirectory_Edit;
            ToolButton        OutputDirectory_ToolButton;
         HorizontalSizer   OutputChunks_Sizer;
            Label             OutputPrefix_Label;
            Edit              OutputPrefix_Edit;
            Label             OutputPostfix_Label;
            Edit              OutputPostfix_Edit;
         HorizontalSizer   OutputKeyword_Sizer;
            Label             OutputKeyword_Label;
            Edit              OutputKeyword_Edit;
         HorizontalSizer   OverwriteExistingFiles_Sizer;
            CheckBox          OverwriteExistingFiles_CheckBox;
         HorizontalSizer   OnError_Sizer;
            Label             OnError_Label;
            ComboBox          OnError_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateSubframeImageItem( size_type );
   void UpdateSubframeImagesList();
   void UpdateSubframeImageSelectionButtons();
   void UpdateSystemParametersControls();
   void UpdateStarDetectorControls();
   void UpdatePedestalControls();
   void UpdateFormatHintsControls();
   void UpdateOutputFilesControls();

   void ShowExpressionsInterface() const;
   void HideExpressionsInterface() const;
   void ShowMeasurementsInterface() const;
   void HideMeasurementsInterface() const;

   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_SubframeImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_SubframeImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_SubframeImages_NodeSelectionUpdated( TreeBox& sender );
   void e_SubframeImages_Click( Button& sender, bool checked );
   void e_RealValueUpdated( NumericEdit& sender, double value );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_CheckboxUpdated( Button& sender, Button::check_state state );
   void e_ButtonClick( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
   void e_StarDetector_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_StarDetector_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void e_Hide( Control& );

   String LocalNormalizationTargetName( const String& filePath );
   String DrizzleTargetName( const String& filePath );

   void SortSubframes( SubframeSortingCriterion );

   friend struct GUIData;
   friend class SubframeSelectorInstance;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorInterface* TheSubframeSelectorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.h - Released 2025-04-07T08:53:56Z
