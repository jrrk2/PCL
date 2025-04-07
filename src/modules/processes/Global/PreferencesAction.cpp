//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// PreferencesAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "PreferencesAction.h"
#include "PreferencesInstance.h"
#include "PreferencesInterface.h"
#include "PreferencesProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

PreferencesAction::PreferencesAction()
   : Action( "Edit > Global Preferences",
             "@module_icons_dir/Preferences.svg",
             "Edit" )
{
   SetToolTip( "Global Preferences" );
}

// ----------------------------------------------------------------------------

void PreferencesAction::Execute()
{
   PreferencesInstance instance( ThePreferencesProcess );
   instance.LoadCurrentSettings();
   instance.LaunchInterface();
   if ( ThePreferencesInterface->IsVisible() )
      ThePreferencesInterface->ActivateWindow();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PreferencesAction.cpp - Released 2025-04-07T08:53:56Z
