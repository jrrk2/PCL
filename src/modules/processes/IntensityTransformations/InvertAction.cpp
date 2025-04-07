//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// InvertAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "InvertAction.h"
#include "InvertInstance.h"
#include "InvertProcess.h"

#include <pcl/ButtonCodes.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

InvertAction::InvertAction()
   : Action( "Image > Invert",
             "@module_icons_dir/Invert.svg",
             "Intensity Transformations" )
{
   SetAccelerator( KeyModifier::Control, KeyCode::I );
   SetToolTip( "Invert" );
}

// ----------------------------------------------------------------------------

void InvertAction::Execute()
{
   InvertInstance( TheInvertProcess ).LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool InvertAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage; // every view can be inverted
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InvertAction.cpp - Released 2025-04-07T08:53:56Z
