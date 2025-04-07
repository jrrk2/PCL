//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelSourceSelectionDialog.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelSourceSelectionDialog_h
#define __ChannelSourceSelectionDialog_h

#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelSourceSelectionDialog : public Dialog
{
public:

   ChannelSourceSelectionDialog( const String& channelSuffix, const String& channelDesc );

   String SourceId() const
   {
      return sourceId;
   }

private:

   String   sourceId;

   VerticalSizer  Global_Sizer;
      ComboBox          SourceImages_ComboBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __SourceImages_ItemSelected( ComboBox& sender, int itemIndex );
   void __Button_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelSourceSelectionDialog_h

// ----------------------------------------------------------------------------
// EOF ChannelSourceSelectionDialog.h - Released 2025-04-07T08:53:55Z
