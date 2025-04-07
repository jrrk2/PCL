//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationActions.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FastRotationActions.h"
#include "FastRotationInstance.h"
#include "FastRotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

Rotate180Action::Rotate180Action()
   : Action( "Image > Geometry > Rotate 180\xb0",
             "@module_icons_dir/Rotate180Action.svg",
             "Geometry" )
{
   SetToolTip( "Rotate 180\xb0" );
}

// ----------------------------------------------------------------------------

void Rotate180Action::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::Rotate180 ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool Rotate180Action::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Rotate90CWAction::Rotate90CWAction()
   : Action( "Image > Geometry > Rotate 90\xb0 Clockwise",
             "@module_icons_dir/Rotate90CWAction.svg",
             "Geometry" )
{
   SetToolTip( "Rotate 90\xb0 Clockwise" );
}

// ----------------------------------------------------------------------------

void Rotate90CWAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::Rotate90CW ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool Rotate90CWAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Rotate90CCWAction::Rotate90CCWAction()
   : Action( "Image > Geometry > Rotate 90\xb0 Counter-clockwise",
             "@module_icons_dir/Rotate90CCWAction.svg",
             "Geometry" )
{
   SetToolTip( "Rotate 90\xb0 Counter-clockwise" );
}

// ----------------------------------------------------------------------------

void Rotate90CCWAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::Rotate90CCW ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool Rotate90CCWAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

HorizontalMirrorAction::HorizontalMirrorAction()
   : Action( "Image > Geometry >> Horizontal Mirror",
             "@module_icons_dir/HorizontalMirrorAction.svg",
             "Geometry" )
{
   SetToolTip( "Horizontal Mirror" );
}

// ----------------------------------------------------------------------------

void HorizontalMirrorAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::HorizontalMirror ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool HorizontalMirrorAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

VerticalMirrorAction::VerticalMirrorAction()
   : Action( "Image > Geometry > Vertical Mirror",
             "@module_icons_dir/VerticalMirrorAction.svg",
             "Geometry" )
{
   SetToolTip( "Vertical Mirror" );
}

// ----------------------------------------------------------------------------

void VerticalMirrorAction::Execute()
{
   FastRotationInstance( TheFastRotationProcess, FRMode::VerticalMirror ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool VerticalMirrorAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationActions.cpp - Released 2025-04-07T08:53:56Z
