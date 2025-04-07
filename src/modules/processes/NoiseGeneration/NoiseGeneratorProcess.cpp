//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// NoiseGeneratorProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "NoiseGeneratorInstance.h"
#include "NoiseGeneratorInterface.h"
#include "NoiseGeneratorParameters.h"
#include "NoiseGeneratorProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NoiseGeneratorProcess* TheNoiseGeneratorProcess = nullptr;

// ----------------------------------------------------------------------------

NoiseGeneratorProcess::NoiseGeneratorProcess()
{
   TheNoiseGeneratorProcess = this;

   new NGNoiseAmount( this );
   new NGNoiseDistribution( this );
   new NGImpulsionalNoiseProbability( this );
   new NGPreserveBrightness( this ); // ### DEPRECATED
}

// ----------------------------------------------------------------------------

IsoString NoiseGeneratorProcess::Id() const
{
   return "NoiseGenerator";
}

// ----------------------------------------------------------------------------

IsoString NoiseGeneratorProcess::Category() const
{
   return "NoiseGeneration";
}

// ----------------------------------------------------------------------------

uint32 NoiseGeneratorProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String NoiseGeneratorProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String NoiseGeneratorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/NoiseGenerator.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* NoiseGeneratorProcess::DefaultInterface() const
{
   return TheNoiseGeneratorInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* NoiseGeneratorProcess::Create() const
{
   return new NoiseGeneratorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* NoiseGeneratorProcess::Clone( const ProcessImplementation& p ) const
{
   const NoiseGeneratorInstance* instPtr = dynamic_cast<const NoiseGeneratorInstance*>( &p );
   return (instPtr != nullptr) ? new NoiseGeneratorInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoiseGeneratorProcess.cpp - Released 2025-04-07T08:53:56Z
