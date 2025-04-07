//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelCombinationInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelCombinationInterface_h
#define __ChannelCombinationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "ChannelCombinationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelCombinationInterface : public ProcessInterface
{
public:

   ChannelCombinationInterface();
   virtual ~ChannelCombinationInterface();

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

   ChannelCombinationInstance m_instance;

   struct GUIData
   {
      GUIData( ChannelCombinationInterface& );

      HorizontalSizer      Global_Sizer;
         GroupBox          ColorSpace_GroupBox;
         HorizontalSizer   ColorSpace_Sizer;
            VerticalSizer     ColorSpaceLeft_Sizer;
               RadioButton       RGB_RadioButton;
               RadioButton       HSV_RadioButton;
               RadioButton       HSI_RadioButton;
            VerticalSizer     ColorSpaceRight_Sizer;
               RadioButton       CIEXYZ_RadioButton;
               RadioButton       CIELab_RadioButton;
               RadioButton       CIELch_RadioButton;
         GroupBox          Channels_GroupBox;
         VerticalSizer     Channels_Sizer;
            HorizontalSizer   C0_Sizer;
               CheckBox          C0_CheckBox;
               Label             C0_Label;
               Edit              C0_Edit;
               ToolButton        C0_ToolButton;
            HorizontalSizer   C1_Sizer;
               CheckBox          C1_CheckBox;
               Label             C1_Label;
               Edit              C1_Edit;
               ToolButton        C1_ToolButton;
            HorizontalSizer   C2_Sizer;
               CheckBox          C2_CheckBox;
               Label             C2_Label;
               Edit              C2_Edit;
               ToolButton        C2_ToolButton;
            HorizontalSizer   InheritAstrometricSolution_Sizer;
               CheckBox          InheritAstrometricSolution_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ColorSpace_Click( Button& sender, bool checked );
   void e_Channel_Click( Button& sender, bool checked );
   void e_ChannelId_GetFocus( Control& sender );
   void e_ChannelId_EditCompleted( Edit& sender );
   void e_Channel_SelectSource_Click( Button& sender, bool checked );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
   friend class  ChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ChannelCombinationInterface* TheChannelCombinationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelCombinationInterface_h

// ----------------------------------------------------------------------------
// EOF ChannelCombinationInterface.h - Released 2025-04-07T08:53:55Z
