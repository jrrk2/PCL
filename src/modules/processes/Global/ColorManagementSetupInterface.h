//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ColorManagementSetupInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ColorManagementSetupInterface_h
#define __ColorManagementSetupInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ColorComboBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/ICCProfile.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/StringList.h>

#include "ColorManagementSetupInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ColorManagementSetupInterface : public ProcessInterface
{
public:

   ColorManagementSetupInterface();
   virtual ~ColorManagementSetupInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void Initialize() override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   ColorManagementSetupInstance m_instance;

   ICCProfile::profile_list rgbProfiles;      // RGB only
   ICCProfile::profile_list grayProfiles;     // RGB and grayscale only
   ICCProfile::profile_list p_proofingProfiles; // any color space

   struct GUIData
   {
      GUIData( ColorManagementSetupInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        MonitorProfile_SectionBar;
      Control           MonitorProfile_Control;
      VerticalSizer     MonitorProfile_Sizer;
         Edit              MonitorProfile_Edit;
         Edit              MonitorProfileFullPath_Edit;
         HorizontalSizer   RenderingIntent_Sizer;
            Label             RenderingIntent_Label;
            ComboBox          RenderingIntent_ComboBox;

      SectionBar        SystemSettings_SectionBar;
      Control           SystemSettings_Control;
      VerticalSizer     SystemSettings_Sizer;
         HorizontalSizer   DetectMonitorProfile_Sizer;
            CheckBox          DetectMonitorProfile_CheckBox;
         GroupBox          NewMonitorProfile_GroupBox;
         VerticalSizer     NewMonitorProfile_Sizer;
            Edit              NewMonitorProfile_Edit;
            ComboBox          NewMonitorProfile_ComboBox;

      SectionBar        DefaultProfiles_SectionBar;
      Control           DefaultProfiles_Control;
      VerticalSizer     DefaultProfiles_Sizer;
         GroupBox          RGBProfiles_GroupBox;
         VerticalSizer     RGBProfiles_Sizer;
            Edit              RGBProfileId_Edit;
            ComboBox          RGBProfile_ComboBox;
         GroupBox          GrayscaleProfiles_GroupBox;
         VerticalSizer     GrayscaleProfiles_Sizer;
            Edit              GrayscaleProfileId_Edit;
            ComboBox          GrayscaleProfile_ComboBox;

      SectionBar        DefaultPolicies_SectionBar;
      Control           DefaultPolicies_Control;
      HorizontalSizer   DefaultPolicies_Sizer;
         GroupBox          OnProfileMismatch_GroupBox;
         VerticalSizer     OnProfileMismatch_Sizer;
            RadioButton       OnProfileMismatch_Ask_RadioButton;
            RadioButton       OnProfileMismatch_Keep_RadioButton;
            RadioButton       OnProfileMismatch_Convert_RadioButton;
            RadioButton       OnProfileMismatch_Discard_RadioButton;
            RadioButton       OnProfileMismatch_Disable_RadioButton;
         GroupBox          OnMissingProfile_GroupBox;
         VerticalSizer     OnMissingProfile_Sizer;
            RadioButton       OnMissingProfile_Ask_RadioButton;
            RadioButton       OnMissingProfile_Assign_RadioButton;
            RadioButton       OnMissingProfile_Leave_RadioButton;
            RadioButton       OnMissingProfile_Disable_RadioButton;

      SectionBar        Proofing_SectionBar;
      Control           Proofing_Control;
      VerticalSizer     Proofing_Sizer;
         GroupBox          ProofingProfile_GroupBox;
         VerticalSizer     ProofingProfile_Sizer;
            Edit              ProofingProfileId_Edit;
            ComboBox          ProofingProfile_ComboBox;
         HorizontalSizer   ProofingIntent_Sizer;
            Label             ProofingIntent_Label;
            ComboBox          ProofingIntent_ComboBox;
         HorizontalSizer   UseProofingBPC_Sizer;
            CheckBox          UseProofingBPC_CheckBox;
         HorizontalSizer   DefaultProofingEnabled_Sizer;
            CheckBox          DefaultProofingEnabled_CheckBox;
         HorizontalSizer   DefaultGamutCheckEnabled_Sizer;
            CheckBox          DefaultGamutCheckEnabled_CheckBox;
         HorizontalSizer   GamutWarningColor_Sizer;
            Label             GamutWarningColor_Label;
            ColorComboBox     GamutWarningColor_ComboBox;
            Control           GamutWarningColor_Control;

      SectionBar        GlobalOptions_SectionBar;
      Control           GlobalOptions_Control;
      HorizontalSizer   GlobalOptions_Sizer;
         VerticalSizer     MiscOptions_Sizer;
            CheckBox          EnableColorManagement_CheckBox;
            CheckBox          UseLowResolutionCLUTs_CheckBox;
         VerticalSizer     EmbedProfiles_Sizer;
            CheckBox          EmbedProfilesInRGBImages_CheckBox;
            CheckBox          EmbedProfilesInGrayscaleImages_CheckBox;

      HorizontalSizer      GlobalActions_Sizer;
         PushButton           RefreshProfiles_PushButton;
         PushButton           LoadCurrentSettings_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void RefreshProfiles();

   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_EditCompleted( Edit& );
   void e_ButtonClick( Button& sender, bool checked );
   void e_ColorSelected( ColorComboBox& sender, RGBA color );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ColorManagementSetupInterface* TheColorManagementSetupInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorManagementSetupInterface_h

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupInterface.h - Released 2025-04-07T08:53:56Z
