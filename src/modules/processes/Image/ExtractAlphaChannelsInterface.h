//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExtractAlphaChannelsInterface_h
#define __ExtractAlphaChannelsInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>

#include "ExtractAlphaChannelsInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ExtractAlphaChannelsInterface : public ProcessInterface
{
public:

   ExtractAlphaChannelsInterface();
   virtual ~ExtractAlphaChannelsInterface();

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

   ExtractAlphaChannelsInstance instance;

   struct GUIData
   {
      GUIData( ExtractAlphaChannelsInterface& );

      VerticalSizer      Global_Sizer;
         GroupBox          Channels_GroupBox;
         VerticalSizer     Channels_Sizer;
            RadioButton       AllAlphaChannels_RadioButton;
            RadioButton       ActiveAlphaChannel_RadioButton;
            RadioButton       ChannelList_RadioButton;
            Edit              ChannelList_Edit;
         GroupBox          Mode_GroupBox;
         VerticalSizer     Mode_Sizer;
            CheckBox          ExtractChannels_CheckBox;
            CheckBox          DeleteChannels_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __Channels_Click( Button& sender, bool checked );
   void __ChannelList_EditCompleted( Edit& sender );
   void __Mode_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ExtractAlphaChannelsInterface* TheExtractAlphaChannelsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExtractAlphaChannelsInterface_h

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsInterface.h - Released 2025-04-07T08:53:56Z
