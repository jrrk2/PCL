//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// GrayscaleRGBActions.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "GrayscaleRGBActions.h"
#include "GrayscaleRGBInstances.h"
#include "GrayscaleRGBProcesses.h"

#include <pcl/ButtonCodes.h>
#include <pcl/KeyCodes.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ConvertToGrayscaleAction::ConvertToGrayscaleAction()
   : Action( "Image > Color Spaces > Convert to Grayscale",
             "@module_icons_dir/ConvertToGrayscale.svg",
             "Color Spaces" )
{
   SetToolTip( "Convert to Grayscale" );
}

// ----------------------------------------------------------------------------

void ConvertToGrayscaleAction::Execute()
{
   ConvertToGrayscaleInstance( TheConvertToGrayscaleProcess ).LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && info.isColor; // only RGB color view images can be converted to grayscale
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvertToRGBColorAction::ConvertToRGBColorAction()
   : Action( "Image > Color Spaces > Convert to RGB Color",
             "@module_icons_dir/ConvertToRGBColor.svg",
             "Color Spaces" )
{
   SetToolTip( "Convert to RGB Color" );
}

// ----------------------------------------------------------------------------

void ConvertToRGBColorAction::Execute()
{
   ConvertToRGBColorInstance( TheConvertToRGBColorProcess ).LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorAction::IsEnabled( ActionInfo info ) const
{
   return info.isImage && !info.isColor; // only grayscale view images can be converted to RGB color
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBActions.cpp - Released 2025-04-07T08:53:55Z
