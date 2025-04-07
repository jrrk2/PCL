//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelCombinationProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelCombinationProcess.h"
#include "ChannelCombinationParameters.h"
#include "ChannelCombinationInstance.h"
#include "ChannelCombinationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelCombinationProcess* TheChannelCombinationProcess = nullptr;

// ----------------------------------------------------------------------------

ChannelCombinationProcess::ChannelCombinationProcess()
{
   TheChannelCombinationProcess = this;

   TheColorSpaceIdCombinationParameter = new ColorSpaceId( this );
   TheChannelTableCombinationParameter = new ChannelTable( this );
   TheChannelEnabledCombinationParameter = new ChannelEnabled( TheChannelTableCombinationParameter );
   TheChannelIdCombinationParameter = new ChannelId( TheChannelTableCombinationParameter );
   TheCCInheritAstrometricSolutionParameter = new CSInheritAstrometricSolution( this );
}

// ----------------------------------------------------------------------------

IsoString ChannelCombinationProcess::Id() const
{
   return "ChannelCombination";
}

// ----------------------------------------------------------------------------

IsoString ChannelCombinationProcess::Categories() const
{
   return "ColorSpaces,ChannelManagement";
}

// ----------------------------------------------------------------------------

String ChannelCombinationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ChannelCombinationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ChannelCombination.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ChannelCombinationProcess::DefaultInterface() const
{
   return TheChannelCombinationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ChannelCombinationProcess::Create() const
{
   return new ChannelCombinationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ChannelCombinationProcess::Clone( const ProcessImplementation& p ) const
{
   const ChannelCombinationInstance* instPtr = dynamic_cast<const ChannelCombinationInstance*>( &p );
   return (instPtr != nullptr) ? new ChannelCombinationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ChannelCombinationProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelCombinationProcess.cpp - Released 2025-04-07T08:53:55Z
