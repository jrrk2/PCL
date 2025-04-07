//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// RescaleActions.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RescaleActions.h"
#include "RescaleInstance.h"
#include "RescaleProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RescaleRGBAction::RescaleRGBAction()
   : Action( "Image > Rescale > Rescale RGB/K",
             "@module_icons_dir/Rescale.svg",
             "Intensity Transformations" )
{
   SetToolTip( "Rescale" );
}

// ----------------------------------------------------------------------------

void RescaleRGBAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::RGBK ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool RescaleRGBAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RescaleRGBIndividualAction::RescaleRGBIndividualAction()
   : Action( "Image > Rescale > Rescale Individual RGB/K Channels",
             "@module_icons_dir/RescaleRGBAction.svg" )
{
   SetToolTip( "Rescale Individual RGB/K Channels" );
}

// ----------------------------------------------------------------------------

void RescaleRGBIndividualAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::RGBK ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool RescaleRGBIndividualAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RescaleCIELAction::RescaleCIELAction()
   : Action( "Image > Rescale > Rescale CIE L* Component",
             "@module_icons_dir/RescaleLumAction.svg" )
{
   SetToolTip( "Rescale CIE L* Component" );
}

// ----------------------------------------------------------------------------

void RescaleCIELAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::CIEL ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool RescaleCIELAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RescaleCIEYAction::RescaleCIEYAction()
   : Action( "Image > Rescale > Rescale CIE Y Component",
             "@module_icons_dir/RescaleLumAction.svg" )
{
   SetToolTip( "Rescale CIE Y Component" );
}

// ----------------------------------------------------------------------------

void RescaleCIEYAction::Execute()
{
   RescaleInstance( TheRescaleProcess, RescalingMode::CIEY ).LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

bool RescaleCIEYAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleActions.cpp - Released 2025-04-07T08:53:56Z
