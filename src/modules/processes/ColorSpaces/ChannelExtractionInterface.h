//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelExtractionInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelExtractionInterface_h
#define __ChannelExtractionInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ComboBox.h>

#include "ChannelExtractionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelExtractionInterface : public ProcessInterface
{
public:

   ChannelExtractionInterface();
   virtual ~ChannelExtractionInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   ChannelExtractionInstance m_instance;

   struct GUIData
   {
      GUIData( ChannelExtractionInterface& );

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
               Edit              C0_Edit;
            HorizontalSizer   C1_Sizer;
               CheckBox          C1_CheckBox;
               Edit              C1_Edit;
            HorizontalSizer   C2_Sizer;
               CheckBox          C2_CheckBox;
               Edit              C2_Edit;
            HorizontalSizer   SampleFormat_Sizer;
               Label             SampleFormat_Label;
               ComboBox          SampleFormat_ComboBox;
            HorizontalSizer   InheritAstrometricSolution_Sizer;
               CheckBox          InheritAstrometricSolution_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ColorSpace_Click( Button& sender, bool checked );
   void e_Channel_Click( Button& sender, bool checked );
   void e_ChannelId_GetFocus( Control& sender );
   void e_ChannelId_EditCompleted( Edit& sender );
   void e_SampleFormat_ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ChannelExtractionInterface* TheChannelExtractionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelExtractionInterface_h

// ----------------------------------------------------------------------------
// EOF ChannelExtractionInterface.h - Released 2025-04-07T08:53:55Z
