//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// AssignICCProfileInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AssignICCProfileInterface_h
#define __AssignICCProfileInterface_h

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

#include "AssignICCProfileInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class AssignICCProfileInterface : public ProcessInterface
{
public:

   AssignICCProfileInterface();
   virtual ~AssignICCProfileInterface();

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

   AssignICCProfileInstance m_instance;
   ICCProfile::profile_list m_profiles;

   struct GUIData
   {
      GUIData( AssignICCProfileInterface& );

      VerticalSizer     Global_Sizer;

      GroupBox          CurrentProfile_GroupBox;
      VerticalSizer     CurrentProfile_Sizer;
         ViewList          AllImages_ViewList;
         Edit              CurrentProfile_Edit;

      GroupBox          NewProfile_GroupBox;
      VerticalSizer     NewProfile_Sizer;
         RadioButton       AssignDefault_RadioButton;
         RadioButton       LeaveUntagged_RadioButton;
         RadioButton       AssignProfile_RadioButton;
         Edit              TargetProfile_Edit;
         HorizontalSizer   AllProfiles_Sizer;
            ComboBox          AllProfiles_ComboBox;
            ToolButton        RefreshProfiles_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void RefreshProfiles();

   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __AssignMode_ButtonClick( Button& sender, bool checked );
   void __TargetProfile_EditCompleted( Edit& );
   void __Profile_ItemSelected( ComboBox& sender, int itemIndex );
   void __RefreshProfiles_ButtonClick( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AssignICCProfileInterface* TheAssignICCProfileInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AssignICCProfileInterface_h

// ----------------------------------------------------------------------------
// EOF AssignICCProfileInterface.h - Released 2025-04-07T08:53:55Z
