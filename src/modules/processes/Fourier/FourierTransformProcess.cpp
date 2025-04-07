//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierTransformProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FourierTransformProcess.h"
#include "FourierTransformParameters.h"
#include "FourierTransformInstance.h"
#include "FourierTransformInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FourierTransformProcess* TheFourierTransformProcess = nullptr;

// ----------------------------------------------------------------------------

FourierTransformProcess::FourierTransformProcess()
{
   TheFourierTransformProcess = this;

   new FTRadialCoordinates( this );
   new FTCentered( this );
}

// ----------------------------------------------------------------------------

IsoString FourierTransformProcess::Id() const
{
   return "FourierTransform";
}

// ----------------------------------------------------------------------------

IsoString FourierTransformProcess::Category() const
{
   return "Fourier";
}

// ----------------------------------------------------------------------------

uint32 FourierTransformProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String FourierTransformProcess::Description() const
{
   return

   "<html>"
   "<p>FourierTransform is a simple tool to compute the Fourier transform of an image. "
   "It can provide the components of the Fourier transform directly as complex "
   "numbers, or as the magnitude and phase components of the power spectrum.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String FourierTransformProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/FourierTransform.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* FourierTransformProcess::DefaultInterface() const
{
   return TheFourierTransformInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FourierTransformProcess::Create() const
{
   return new FourierTransformInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* FourierTransformProcess::Clone( const ProcessImplementation& p ) const
{
   const FourierTransformInstance* instPtr = dynamic_cast<const FourierTransformInstance*>( &p );
   return (instPtr != nullptr) ? new FourierTransformInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FourierTransformProcess.cpp - Released 2025-04-07T08:53:55Z
