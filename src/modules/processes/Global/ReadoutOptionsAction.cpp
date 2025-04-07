//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ReadoutOptionsAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ReadoutOptionsAction.h"
#include "ReadoutOptionsInstance.h"
#include "ReadoutOptionsInterface.h"
#include "ReadoutOptionsProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptionsAction::ReadoutOptionsAction()
   : Action( "Edit > Readout Options",
             "@module_icons_dir/ReadoutOptions.svg",
             "Edit" )
{
   SetToolTip( "Readout Options" );
}

// ----------------------------------------------------------------------------

void ReadoutOptionsAction::Execute()
{
   ReadoutOptionsInstance instance( TheReadoutOptionsProcess );
   instance.LoadCurrentOptions();
   instance.LaunchInterface();
   if ( TheReadoutOptionsInterface->IsVisible() )
      TheReadoutOptionsInterface->ActivateWindow();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsAction.cpp - Released 2025-04-07T08:53:56Z
