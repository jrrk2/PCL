//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// ICCProfileTransformationInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ICCProfileTransformationInterface_h
#define __ICCProfileTransformationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/ICCProfile.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/ViewList.h>

#include "ICCProfileTransformationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ICCProfileTransformationInterface : public ProcessInterface
{
public:

   ICCProfileTransformationInterface();
   virtual ~ICCProfileTransformationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;
   void ImageCMUpdated( const View& ) override;
   bool WantsGlobalNotifications() const override;
   void GlobalCMUpdated() override;

private:

   ICCProfileTransformationInstance m_instance;
   ICCProfile::profile_list         m_profiles;
   String                           m_defaultRGBProfile;
   String                           m_defaultGrayscaleProfile;

   struct GUIData
   {
      GUIData( ICCProfileTransformationInterface& );

      VerticalSizer     Global_Sizer;

      GroupBox          SourceProfile_GroupBox;
      VerticalSizer     SourceProfile_Sizer;
         ViewList          AllImages_ViewList;
         Edit              SourceProfile_Edit;

      GroupBox          TargetProfile_GroupBox;
      VerticalSizer     TargetProfile_Sizer;
         RadioButton       ToProfile_RadioButton;
         Edit              TargetProfile_Edit;
         HorizontalSizer   AllProfiles_Sizer;
            ComboBox          AllProfiles_ComboBox;
            ToolButton        RefreshProfiles_ToolButton;
         RadioButton       ToDefaultProfile_RadioButton;
         HorizontalSizer   DefaultRGBProfile_Sizer;
            Label             DefaultRGBProfile_Label;
            Edit              DefaultRGBProfile_Edit;
         HorizontalSizer   DefaultGrayscaleProfile_Sizer;
            Label             DefaultGrayscaleProfile_Label;
            Edit              DefaultGrayscaleProfile_Edit;

      GroupBox          RenderingIntent_GroupBox;
      VerticalSizer     RenderingIntent_Sizer;
         ComboBox          RenderingIntent_ComboBox;

      GroupBox          Options_GroupBox;
      HorizontalSizer   Options_Sizer;
         CheckBox          UseBlackPointCompensation_CheckBox;
         CheckBox          UseFloatingPointTransformation_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void RefreshProfiles();
   void RefreshDefaultProfiles();

   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __TargetProfile_EditCompleted( Edit& );
   void __Profile_ItemSelected( ComboBox& sender, int itemIndex );
   void __RefreshProfiles_ButtonClick( Button& sender, bool checked );
   void __ToProfile_ButtonClick( Button& sender, bool checked );
   void __RenderingIntent_ItemSelected( ComboBox& sender, int itemIndex );
   void __Option_ButtonClick( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ICCProfileTransformationInterface* TheICCProfileTransformationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ICCProfileTransformationInterface_h

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationInterface.h - Released 2025-04-07T08:53:55Z
