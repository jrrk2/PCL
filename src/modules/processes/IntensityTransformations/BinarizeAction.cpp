//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeAction.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "BinarizeAction.h"
#include "BinarizeInstance.h"
#include "BinarizeProcess.h"

#include <pcl/ButtonCodes.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeAction::BinarizeAction()
   : Action( "Image > Binarize",
             "@module_icons_dir/Binarize.svg",
             "Intensity Transformations" )
{
   SetToolTip( "Binarize" );
}

// ----------------------------------------------------------------------------

void BinarizeAction::Execute()
{
   BinarizeInstance( TheBinarizeProcess ).LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool BinarizeAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage; // every view can be Binarized
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeAction.cpp - Released 2025-04-07T08:53:56Z
