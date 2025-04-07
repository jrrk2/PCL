//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ChannelMatchProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelMatchProcess.h"
#include "ChannelMatchParameters.h"
#include "ChannelMatchInstance.h"
#include "ChannelMatchInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelMatchProcess* TheChannelMatchProcess = nullptr;

// ----------------------------------------------------------------------------

ChannelMatchProcess::ChannelMatchProcess()
{
   TheChannelMatchProcess = this;

   new CMChannels( this );
   new CMEnabled( TheCMChannelsParameter );
   new CMXOffset( TheCMChannelsParameter );
   new CMYOffset( TheCMChannelsParameter );
   new CMFactor( TheCMChannelsParameter );
}

// ----------------------------------------------------------------------------

IsoString ChannelMatchProcess::Id() const
{
   return "ChannelMatch";
}

// ----------------------------------------------------------------------------

IsoString ChannelMatchProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 ChannelMatchProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ChannelMatchProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ChannelMatchProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ChannelMatch.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ChannelMatchProcess::DefaultInterface() const
{
   return TheChannelMatchInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ChannelMatchProcess::Create() const
{
   return new ChannelMatchInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ChannelMatchProcess::Clone( const ProcessImplementation& p ) const
{
   const ChannelMatchInstance* instPtr = dynamic_cast<const ChannelMatchInstance*>( &p );
   return (instPtr != nullptr) ? new ChannelMatchInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ChannelMatchProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelMatchProcess.cpp - Released 2025-04-07T08:53:55Z
