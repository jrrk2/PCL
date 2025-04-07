//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ColorManagementSetupAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ColorManagementSetupAction.h"
#include "ColorManagementSetupInstance.h"
#include "ColorManagementSetupInterface.h"
#include "ColorManagementSetupProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementSetupAction::ColorManagementSetupAction()
   : Action( "Edit > Color Management Setup",
             "@module_icons_dir/ColorManagementSetup.svg",
             "Edit" )
{
   SetToolTip( "Color Management Setup" );
}

// ----------------------------------------------------------------------------

void ColorManagementSetupAction::Execute()
{
   ColorManagementSetupInstance instance( TheColorManagementSetupProcess );
   instance.LoadCurrentSettings();
   instance.LaunchInterface();
   if ( TheColorManagementSetupInterface->IsVisible() )
      TheColorManagementSetupInterface->ActivateWindow();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupAction.cpp - Released 2025-04-07T08:53:56Z
