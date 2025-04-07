//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ExtractCIELAction.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelExtractionProcess.h"
#include "ChannelExtractionInstance.h"
#include "ChannelParameters.h"
#include "ExtractCIELAction.h"

#include <pcl/ButtonCodes.h>
// #include <pcl/GlobalSettings.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExtractCIELAction::ExtractCIELAction()
   : Action( "Image > Extract > Lightness (CIE L*)",
             "@module_icons_dir/ExtractCIELAction.svg",
         /*
          * Don't hide this toolbar button because a lot of people loves it:
          * https://pixinsight.com/forum/index.php?topic=11298.0
          */
//            (PixInsightSettings::GlobalInteger( "MainWindow/LogicalScreenWidth" ) > 1920) ? "Image" : "" )
             "Image" )
{
   SetToolTip( "Extract CIE L* component" );
}

// ----------------------------------------------------------------------------

void ExtractCIELAction::Execute()
{
   ChannelExtractionInstance instance( TheChannelExtractionProcess );

   instance.p_colorSpace = ColorSpaceId::CIELab;
   instance.p_channelEnabled[0] = true;
   instance.p_channelEnabled[1] = false;
   instance.p_channelEnabled[2] = false;

   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ExtractCIELAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && info.isColor;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExtractCIELAction.cpp - Released 2025-04-07T08:53:55Z
