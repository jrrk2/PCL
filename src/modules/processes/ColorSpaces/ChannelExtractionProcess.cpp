//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelExtractionProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelExtractionProcess.h"
#include "ChannelExtractionParameters.h"
#include "ChannelExtractionInstance.h"
#include "ChannelExtractionInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelExtractionProcess*  TheChannelExtractionProcess = nullptr;

// ----------------------------------------------------------------------------

ChannelExtractionProcess::ChannelExtractionProcess()
{
   TheChannelExtractionProcess = this;

   TheColorSpaceIdExtractionParameter = new ColorSpaceId( this );
   TheChannelTableExtractionParameter = new ChannelTable( this );
   TheChannelEnabledExtractionParameter = new ChannelEnabled( TheChannelTableExtractionParameter );
   TheChannelIdExtractionParameter = new ChannelId( TheChannelTableExtractionParameter );
   TheChannelSampleFormatExtractionParameter = new ChannelSampleFormat( this );
   TheCEInheritAstrometricSolutionParameter = new CSInheritAstrometricSolution( this );
}

// ----------------------------------------------------------------------------

IsoString ChannelExtractionProcess::Id() const
{
   return "ChannelExtraction";
}

// ----------------------------------------------------------------------------

IsoString ChannelExtractionProcess::Categories() const
{
   return "ColorSpaces,ChannelManagement";
}

// ----------------------------------------------------------------------------

String ChannelExtractionProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ChannelExtractionProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ChannelExtraction.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ChannelExtractionProcess::DefaultInterface() const
{
   return TheChannelExtractionInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ChannelExtractionProcess::Create() const
{
   return new ChannelExtractionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ChannelExtractionProcess::Clone( const ProcessImplementation& p ) const
{
   const ChannelExtractionInstance* instPtr = dynamic_cast<const ChannelExtractionInstance*>( &p );
   return (instPtr != nullptr) ? new ChannelExtractionInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ChannelExtractionProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelExtractionProcess.cpp - Released 2025-04-07T08:53:55Z
