//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AdaptiveStretchProcess.h"
#include "AdaptiveStretchParameters.h"
#include "AdaptiveStretchInstance.h"
#include "AdaptiveStretchInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AdaptiveStretchProcess* TheAdaptiveStretchProcess = nullptr;

// ----------------------------------------------------------------------------

AdaptiveStretchProcess::AdaptiveStretchProcess()
{
   TheAdaptiveStretchProcess = this;

   new ASNoiseThreshold( this );
   new ASProtection( this );
   new ASUseProtection( this );
   new ASMaxCurvePoints( this );
   new ASUseROI( this );
   new ASROIX0( this );
   new ASROIY0( this );
   new ASROIX1( this );
   new ASROIY1( this );
}

// ----------------------------------------------------------------------------

IsoString AdaptiveStretchProcess::Id() const
{
   return "AdaptiveStretch";
}

// ----------------------------------------------------------------------------

IsoString AdaptiveStretchProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 AdaptiveStretchProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String AdaptiveStretchProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String AdaptiveStretchProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/AdaptiveStretch.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* AdaptiveStretchProcess::DefaultInterface() const
{
   return TheAdaptiveStretchInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* AdaptiveStretchProcess::Create() const
{
   return new AdaptiveStretchInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AdaptiveStretchProcess::Clone( const ProcessImplementation& p ) const
{
   const AdaptiveStretchInstance* instPtr = dynamic_cast<const AdaptiveStretchInstance*>( &p );
   return (instPtr != nullptr) ? new AdaptiveStretchInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchProcess.cpp - Released 2025-04-07T08:53:56Z
