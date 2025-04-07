//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SimplexNoiseProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SimplexNoiseInstance.h"
#include "SimplexNoiseInterface.h"
#include "SimplexNoiseParameters.h"
#include "SimplexNoiseProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SimplexNoiseProcess* TheSimplexNoiseProcess = nullptr;

// ----------------------------------------------------------------------------

SimplexNoiseProcess::SimplexNoiseProcess()
{
   TheSimplexNoiseProcess = this;

   new SNAmount( this );
   new SNScale( this );
   new SNOffsetX( this );
   new SNOffsetY( this );
   new SNOperator( this );
}

// ----------------------------------------------------------------------------

IsoString SimplexNoiseProcess::Id() const
{
   return "SimplexNoise";
}

// ----------------------------------------------------------------------------

IsoString SimplexNoiseProcess::Category() const
{
   return "NoiseGeneration";
}

// ----------------------------------------------------------------------------

uint32 SimplexNoiseProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SimplexNoiseProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String SimplexNoiseProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SimplexNoise.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SimplexNoiseProcess::DefaultInterface() const
{
   return TheSimplexNoiseInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* SimplexNoiseProcess::Create() const
{
   return new SimplexNoiseInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SimplexNoiseProcess::Clone( const ProcessImplementation& p ) const
{
   const SimplexNoiseInstance* instPtr = dynamic_cast<const SimplexNoiseInstance*>( &p );
   return (instPtr != nullptr) ? new SimplexNoiseInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SimplexNoiseProcess.cpp - Released 2025-04-07T08:53:56Z
