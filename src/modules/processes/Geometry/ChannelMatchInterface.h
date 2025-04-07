//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ChannelMatchInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelMatchInterface_h
#define __ChannelMatchInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "ChannelMatchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelMatchInterface : public ProcessInterface
{
public:

   ChannelMatchInterface();
   virtual ~ChannelMatchInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   ChannelMatchInstance m_instance;

   struct GUIData
   {
      GUIData( ChannelMatchInterface& );

      HorizontalSizer   Global_Sizer;

         GroupBox          Channels_GroupBox;
         VerticalSizer     Channels_Sizer;
            CheckBox          R_CheckBox;
            CheckBox          G_CheckBox;
            CheckBox          B_CheckBox;

         GroupBox          XOffset_GroupBox;
         VerticalSizer     XOffset_Sizer;
            HorizontalSizer   R_XOffset_Sizer;
               NumericEdit       R_XOffset_NumericEdit;
               ToolButton        R_Left_ToolButton;
               ToolButton        R_Right_ToolButton;
            HorizontalSizer   G_XOffset_Sizer;
               NumericEdit       G_XOffset_NumericEdit;
               ToolButton        G_Left_ToolButton;
               ToolButton        G_Right_ToolButton;
            HorizontalSizer   B_XOffset_Sizer;
               NumericEdit       B_XOffset_NumericEdit;
               ToolButton        B_Left_ToolButton;
               ToolButton        B_Right_ToolButton;

         GroupBox          YOffset_GroupBox;
         VerticalSizer     YOffset_Sizer;
            HorizontalSizer   R_YOffset_Sizer;
               NumericEdit       R_YOffset_NumericEdit;
               ToolButton        R_Up_ToolButton;
               ToolButton        R_Down_ToolButton;
            HorizontalSizer   G_YOffset_Sizer;
               NumericEdit       G_YOffset_NumericEdit;
               ToolButton        G_Up_ToolButton;
               ToolButton        G_Down_ToolButton;
            HorizontalSizer   B_YOffset_Sizer;
               NumericEdit       B_YOffset_NumericEdit;
               ToolButton        B_Up_ToolButton;
               ToolButton        B_Down_ToolButton;

         GroupBox          Factors_GroupBox;
         VerticalSizer     Factors_Sizer;
            NumericControl    R_Factor_NumericControl;
            NumericControl    G_Factor_NumericControl;
            NumericControl    B_Factor_NumericControl;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_Channel_ButtonClick( Button& sender, bool checked );
   void e_ChannelOffset_ButtonClick( Button& sender, bool checked );
   void e_ValueUpdated( NumericEdit& sender, double value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ChannelMatchInterface* TheChannelMatchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelMatchInterface_h

// ----------------------------------------------------------------------------
// EOF ChannelMatchInterface.h - Released 2025-04-07T08:53:55Z
