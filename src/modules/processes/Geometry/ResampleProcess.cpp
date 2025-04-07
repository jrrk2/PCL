//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ResampleProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ResampleProcess.h"
#include "ResampleParameters.h"
#include "ResampleInstance.h"
#include "ResampleInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ResampleProcess* TheResampleProcess = nullptr;

// ----------------------------------------------------------------------------

ResampleProcess::ResampleProcess()
{
   TheResampleProcess = this;

   new RSXSize( this );
   new RSYSize( this );
   new RSMode( this );
   new RSAbsoluteMode( this );
   TheRSXResolutionParameter = new XResolution( this );
   TheRSYResolutionParameter = new YResolution( this );
   TheRSMetricResolutionParameter = new MetricResolution( this );
   TheRSForceResolutionParameter = new ForceResolution( this );
   TheRSInterpolationAlgorithmParameter = new InterpolationAlgorithm( this );
   TheRSClampingThresholdParameter = new ClampingThreshold( this );
   TheRSSmoothnessParameter = new Smoothness( this );
   TheRSGammaCorrectionParameter = new GammaCorrection( this );
   TheRSNoGUIMessagesParameter = new NoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString ResampleProcess::Id() const
{
   return "Resample";
}

// ----------------------------------------------------------------------------

IsoString ResampleProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 ResampleProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ResampleProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ResampleProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Resample.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ResampleProcess::DefaultInterface() const
{
   return TheResampleInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ResampleProcess::Create() const
{
   return new ResampleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ResampleProcess::Clone( const ProcessImplementation& p ) const
{
   const ResampleInstance* instPtr = dynamic_cast<const ResampleInstance*>( &p );
   return (instPtr != nullptr) ? new ResampleInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ResampleProcess.cpp - Released 2025-04-07T08:53:55Z
