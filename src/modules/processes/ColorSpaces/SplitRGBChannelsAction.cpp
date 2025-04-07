//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// SplitRGBChannelsAction.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SplitRGBChannelsAction.h"
#include "ChannelExtractionInstance.h"
#include "ChannelExtractionProcess.h"
#include "ChannelParameters.h"

#include <pcl/ButtonCodes.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SplitRGBChannelsAction::SplitRGBChannelsAction()
   : Action( "Image > Extract > Split RGB Channels",
             "@module_icons_dir/SplitRGBChannelsAction.svg",
             "Image" )
{
   SetToolTip( "Split RGB Channels" );
}

// ----------------------------------------------------------------------------

void SplitRGBChannelsAction::Execute()
{
   ChannelExtractionInstance instance( TheChannelExtractionProcess );

   instance.p_colorSpace = ColorSpaceId::RGB;
   instance.p_channelEnabled[0] = true;
   instance.p_channelEnabled[1] = true;
   instance.p_channelEnabled[2] = true;

   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool SplitRGBChannelsAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && info.isColor;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ExtractRedChannelAction::ExtractRedChannelAction()
   : Action( "Image > Extract > Red Channel",
             "@module_icons_dir/ExtractRedChannelAction.svg" )
{
   SetToolTip( "Extract Red Channel" );
}

// ----------------------------------------------------------------------------

void ExtractRedChannelAction::Execute()
{
   ChannelExtractionInstance instance( TheChannelExtractionProcess );

   instance.p_colorSpace = ColorSpaceId::RGB;
   instance.p_channelEnabled[0] = true;
   instance.p_channelEnabled[1] = false;
   instance.p_channelEnabled[2] = false;

   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ExtractRedChannelAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && info.isColor;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ExtractGreenChannelAction::ExtractGreenChannelAction()
   : Action( "Image > Extract > Green Channel",
             "@module_icons_dir/ExtractGreenChannelAction.svg" )
{
   SetToolTip( "Extract Green Channel" );
}

// ----------------------------------------------------------------------------

void ExtractGreenChannelAction::Execute()
{
   ChannelExtractionInstance instance( TheChannelExtractionProcess );

   instance.p_colorSpace = ColorSpaceId::RGB;
   instance.p_channelEnabled[0] = false;
   instance.p_channelEnabled[1] = true;
   instance.p_channelEnabled[2] = false;

   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ExtractGreenChannelAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && info.isColor;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ExtractBlueChannelAction::ExtractBlueChannelAction()
   : Action( "Image > Extract > Blue Channel",
             "@module_icons_dir/ExtractBlueChannelAction.svg" )
{
   SetToolTip( "Extract Blue Channel" );
}

// ----------------------------------------------------------------------------

void ExtractBlueChannelAction::Execute()
{
   ChannelExtractionInstance instance( TheChannelExtractionProcess );

   instance.p_colorSpace = ColorSpaceId::RGB;
   instance.p_channelEnabled[0] = false;
   instance.p_channelEnabled[1] = false;
   instance.p_channelEnabled[2] = true;

   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ExtractBlueChannelAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && info.isColor;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SplitRGBChannelsAction.cpp - Released 2025-04-07T08:53:55Z
