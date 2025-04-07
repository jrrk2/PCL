//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ColorSaturationProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ColorSaturationProcess.h"
#include "ColorSaturationParameters.h"
#include "ColorSaturationInstance.h"
#include "ColorSaturationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSaturationProcess* TheColorSaturationProcess = nullptr;

// ----------------------------------------------------------------------------

ColorSaturationProcess::ColorSaturationProcess()
{
   TheColorSaturationProcess = this;

   // Instantiate process parameters

   new HueSaturationCurve( this );
   new XHS( TheHueSaturationCurveParameter );
   new YHS( TheHueSaturationCurveParameter );
   new HueSaturationCurveType( this );

   new HueShift( this );
}

// ----------------------------------------------------------------------------

IsoString ColorSaturationProcess::Id() const
{
   return "ColorSaturation";
}

// ----------------------------------------------------------------------------

IsoString ColorSaturationProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 ColorSaturationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ColorSaturationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ColorSaturationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ColorSaturation.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ColorSaturationProcess::DefaultInterface() const
{
   return TheColorSaturationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ColorSaturationProcess::Create() const
{
   return new ColorSaturationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ColorSaturationProcess::Clone( const ProcessImplementation& p ) const
{
   const ColorSaturationInstance* instPtr = dynamic_cast<const ColorSaturationInstance*>( &p );
   return (instPtr != nullptr) ? new ColorSaturationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorSaturationProcess.cpp - Released 2025-04-07T08:53:56Z
