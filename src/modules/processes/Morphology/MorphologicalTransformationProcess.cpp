//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "MorphologicalTransformationProcess.h"
#include "MorphologicalTransformationParameters.h"
#include "MorphologicalTransformationInstance.h"
#include "MorphologicalTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MorphologicalTransformationProcess* TheMorphologicalTransformationProcess = nullptr;

// ----------------------------------------------------------------------------

MorphologicalTransformationProcess::MorphologicalTransformationProcess()
{
   TheMorphologicalTransformationProcess = this;

   new MTOperator( this );
   new MTInterlacingDistance( this );
   new MTLowThreshold( this );
   new MTHighThreshold( this );
   new MTNumberOfIterations( this );
   new MTAmount( this );
   new MTSelectionPoint( this );
   new MTStructureName( this );
   new MTStructureSize( this );
   new MTStructureWayTable( this );
   new MTStructureWayMask( TheMTStructureWayTableParameter );
}

// ----------------------------------------------------------------------------

IsoString MorphologicalTransformationProcess::Id() const
{
   return "MorphologicalTransformation";
}

// ----------------------------------------------------------------------------

IsoString MorphologicalTransformationProcess::Category() const
{
   return "Morphology";
}

// ----------------------------------------------------------------------------

uint32 MorphologicalTransformationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String MorphologicalTransformationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String MorphologicalTransformationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/MorphologicalTransformation.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* MorphologicalTransformationProcess::DefaultInterface() const
{
   return TheMorphologicalTransformationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* MorphologicalTransformationProcess::Create() const
{
   return new MorphologicalTransformationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* MorphologicalTransformationProcess::Clone( const ProcessImplementation& p ) const
{
   const MorphologicalTransformationInstance* instPtr = dynamic_cast<const MorphologicalTransformationInstance*>( &p );
   return (instPtr != nullptr) ? new MorphologicalTransformationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationProcess.cpp - Released 2025-04-07T08:53:56Z
