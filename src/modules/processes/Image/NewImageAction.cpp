//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// NewImageAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "NewImageAction.h"
#include "NewImageInstance.h"
#include "NewImageInterface.h"
#include "NewImageProcess.h"

#include <pcl/ButtonCodes.h>
#include <pcl/GlobalSettings.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

NewImageAction::NewImageAction()
   : Action( "Image > New",
             "@module_icons_dir/NewImage.svg",
             (PixInsightSettings::GlobalInteger( "MainWindow/LogicalScreenWidth" ) > 1920) ? "> Image" : "" )
{
   SetToolTip( "New Image" );
   SetAccelerator( pcl::KeyModifier::Control, pcl::KeyCode::N );
}

// ----------------------------------------------------------------------------

void NewImageAction::Execute()
{
   NewImageInstance( TheNewImageProcess ).LaunchInterface();
   if ( TheNewImageInterface->IsVisible() )
      TheNewImageInterface->ActivateWindow();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NewImageAction.cpp - Released 2025-04-07T08:53:56Z
