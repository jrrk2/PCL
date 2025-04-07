//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// FITSHeaderAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FITSHeaderAction.h"
#include "FITSHeaderInstance.h"
#include "FITSHeaderInterface.h"
#include "FITSHeaderProcess.h"

#include <pcl/ButtonCodes.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FITSHeaderAction::FITSHeaderAction()
   : Action( "File > FITS Header",
             "@module_icons_dir/FITSHeader.svg",
             "File" )
{
   SetToolTip( "FITS Header" );
}

// ----------------------------------------------------------------------------

void FITSHeaderAction::Execute()
{
   FITSHeaderInstance instance( TheFITSHeaderProcess );
   instance.LaunchInterface();

   TheFITSHeaderInterface->SelectImage( ImageWindow::ActiveWindow() );
}

// ----------------------------------------------------------------------------

bool FITSHeaderAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderAction.cpp - Released 2025-04-07T08:53:56Z
