//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// FilterManagerProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FilterManagerInterface.h"
#include "FilterManagerParameters.h"
#include "FilterManagerProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FilterManagerProcess* TheFilterManagerProcess = nullptr;

// ----------------------------------------------------------------------------

FilterManagerProcess::FilterManagerProcess()
{
   TheFilterManagerProcess = this;

   new FMTask( this );
   new FMPath( this );
   new FMNoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString FilterManagerProcess::Id() const
{
   return "FilterManager";
}

// ----------------------------------------------------------------------------

IsoString FilterManagerProcess::Category() const
{
   return "Global,Spectrophotometry";
}

// ----------------------------------------------------------------------------

uint32 FilterManagerProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String FilterManagerProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String FilterManagerProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/FilterManager.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* FilterManagerProcess::DefaultInterface() const
{
   return TheFilterManagerInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FilterManagerProcess::Create() const
{
   return new FilterManagerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* FilterManagerProcess::Clone( const ProcessImplementation& p ) const
{
   const FilterManagerInstance* instPtr = dynamic_cast<const FilterManagerInstance*>( &p );
   return (instPtr != nullptr) ? new FilterManagerInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool FilterManagerProcess::PrefersGlobalExecution() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FilterManagerProcess.cpp - Released 2025-04-07T08:53:56Z
