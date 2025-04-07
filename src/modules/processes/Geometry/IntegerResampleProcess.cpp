//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "IntegerResampleProcess.h"
#include "IntegerResampleParameters.h"
#include "IntegerResampleInstance.h"
#include "IntegerResampleInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleProcess* TheIntegerResampleProcess = nullptr;

// ----------------------------------------------------------------------------

IntegerResampleProcess::IntegerResampleProcess()
{
   TheIntegerResampleProcess = this;

   new IRZoomFactor( this );
   new IRDownsamplingMode( this );
   TheIRXResolutionParameter = new XResolution( this );
   TheIRYResolutionParameter = new YResolution( this );
   TheIRMetricResolutionParameter = new MetricResolution( this );
   TheIRForceResolutionParameter = new ForceResolution( this );
   TheIRGammaCorrectionParameter = new GammaCorrection( this );
   TheIRNoGUIMessagesParameter = new NoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString IntegerResampleProcess::Id() const
{
   return "IntegerResample";
}

// ----------------------------------------------------------------------------

IsoString IntegerResampleProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 IntegerResampleProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String IntegerResampleProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String IntegerResampleProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/IntegerResample.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* IntegerResampleProcess::DefaultInterface() const
{
   return TheIntegerResampleInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* IntegerResampleProcess::Create() const
{
   return new IntegerResampleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* IntegerResampleProcess::Clone( const ProcessImplementation& p ) const
{
   const IntegerResampleInstance* instPtr = dynamic_cast<const IntegerResampleInstance*>( &p );
   return (instPtr != nullptr) ? new IntegerResampleInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleProcess.cpp - Released 2025-04-07T08:53:55Z
