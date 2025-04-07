//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// LRGBCombinationProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "LRGBCombinationProcess.h"
#include "LRGBCombinationParameters.h"
#include "LRGBCombinationInstance.h"
#include "LRGBCombinationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LRGBCombinationProcess* TheLRGBCombinationProcess = nullptr;

// ----------------------------------------------------------------------------

LRGBCombinationProcess::LRGBCombinationProcess()
{
   TheLRGBCombinationProcess = this;

   new LRGBChannelTable( this );
   new LRGBChannelEnabled( TheLRGBChannelTableParameter );
   new LRGBChannelId( TheLRGBChannelTableParameter );
   new LRGBChannelWeight( TheLRGBChannelTableParameter );
   new LRGBLuminanceMTF( this );
   new LRGBSaturationMTF( this );
   new LRGBClipHighlights( this );
   new LRGBNoiseReduction( this );
   new LRGBLayersRemoved( this );
   new LRGBLayersProtected( this );
   TheLRGBCInheritAstrometricSolutionParameter = new CSInheritAstrometricSolution( this );
}

// ----------------------------------------------------------------------------

IsoString LRGBCombinationProcess::Id() const
{
   return "LRGBCombination";
}

// ----------------------------------------------------------------------------

IsoString LRGBCombinationProcess::Categories() const
{
   return "ColorSpaces";
}

// ----------------------------------------------------------------------------

String LRGBCombinationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String LRGBCombinationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/LRGBCombination.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* LRGBCombinationProcess::DefaultInterface() const
{
   return TheLRGBCombinationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* LRGBCombinationProcess::Create() const
{
   return new LRGBCombinationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* LRGBCombinationProcess::Clone( const ProcessImplementation& p ) const
{
   const LRGBCombinationInstance* instPtr = dynamic_cast<const LRGBCombinationInstance*>( &p );
   return (instPtr != nullptr) ? new LRGBCombinationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool LRGBCombinationProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LRGBCombinationProcess.cpp - Released 2025-04-07T08:53:55Z
