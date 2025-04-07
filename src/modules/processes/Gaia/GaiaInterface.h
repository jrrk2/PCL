//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.2.6
// ----------------------------------------------------------------------------
// GaiaInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Gaia PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __GaiaInterface_h
#define __GaiaInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "GaiaInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class OnlineObjectSearchDialog;

class GaiaInterface : public ProcessInterface
{
public:

   GaiaInterface();
   virtual ~GaiaInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void EditPreferences() override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   GaiaInstance              m_instance;
   OnlineObjectSearchDialog* m_searchDialog = nullptr;
   bool                      m_showCompoundAngles = true;
   bool                      m_raInTimeUnits = true;

   struct GUIData
   {
      GUIData( GaiaInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        SearchParameters_SectionBar;
      Control           SearchParameters_Control;
      VerticalSizer     SearchParameters_Sizer;
         HorizontalSizer   DataRelease_Sizer;
            Label             DataRelease_Label;
            ComboBox          DataRelease_ComboBox;
         HorizontalSizer   SearchParametersBottom_Sizer;
            VerticalSizer     SearchParametersLeft_Sizer;
               HorizontalSizer   CenterRA_Sizer;
                  Label             CenterRA_Label;
                  Edit              CenterRA_Edit;
               HorizontalSizer   CenterDec_Sizer;
                  Label             CenterDec_Label;
                  Edit              CenterDec_Edit;
               HorizontalSizer   Radius_Sizer;
                  Label             Radius_Label;
                  Edit              Radius_Edit;
               HorizontalSizer   ShowCompoundAngles_Sizer;
                  CheckBox          ShowCompoundAngles_CheckBox;
               HorizontalSizer   RAInTimeUnits_Sizer;
                  CheckBox          RAInTimeUnits_CheckBox;
               NumericEdit       MagnitudeLow_NumericEdit;
               NumericEdit       MagnitudeHigh_NumericEdit;
               HorizontalSizer   RequiredFlags_Sizer;
                  Label             RequiredFlags_Label;
                  Edit              RequiredFlags_Edit;
                  ToolButton        RequiredFlagsClear_ToolButton;
                  ToolButton        RequiredFlags_ToolButton;
               HorizontalSizer   InclusionFlags_Sizer;
                  Label             InclusionFlags_Label;
                  Edit              InclusionFlags_Edit;
                  ToolButton        InclusionFlagsClear_ToolButton;
                  ToolButton        InclusionFlags_ToolButton;
               HorizontalSizer   ExclusionFlags_Sizer;
                  Label             ExclusionFlags_Label;
                  Edit              ExclusionFlags_Edit;
                  ToolButton        ExclusionFlagsClear_ToolButton;
                  ToolButton        ExclusionFlags_ToolButton;
               NumericEdit       SourceLimit_NumericEdit;
               ToolButton        SourceLimitReset_ToolButton;
            VerticalSizer     SearchParametersRight_Sizer;
                  PushButton        SearchCoordinates_Button;

      SectionBar        OutputParameters_SectionBar;
      Control           OutputParameters_Control;
      VerticalSizer     OutputParameters_Sizer;
         HorizontalSizer   SortBy_Sizer;
            Label             SortBy_Label;
            ComboBox          SortBy_ComboBox;
         HorizontalSizer   GenerateTextOutput_Sizer;
            CheckBox          GenerateTextOutput_CheckBox;
         HorizontalSizer   GenerateBinaryOutput_Sizer;
            CheckBox          GenerateBinaryOutput_CheckBox;
         HorizontalSizer   TextFormat_Sizer;
            Label             TextFormat_Label;
            ComboBox          TextFormat_ComboBox;
         HorizontalSizer   TextHeaders_Sizer;
            Label             TextHeaders_Label;
            ComboBox          TextHeaders_ComboBox;
         HorizontalSizer   OutputFilePath_Sizer;
            Label             OutputFilePath_Label;
            Edit              OutputFilePath_Edit;
            ToolButton        OutputFilePath_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   String RAToString( double ) const;
   String DecToString( double ) const;
   String DistToString( double ) const;

   void e_EditCompleted( Edit& sender );
   void e_EditValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Click( Button& sender, bool checked );

   friend struct GUIData;
   friend class GaiaPreferencesDialog;
   friend class GaiaInstance;
   friend class GaiaProcess;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GaiaInterface* TheGaiaInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GaiaInterface_h

// ----------------------------------------------------------------------------
// EOF GaiaInterface.h - Released 2025-04-07T08:53:55Z
