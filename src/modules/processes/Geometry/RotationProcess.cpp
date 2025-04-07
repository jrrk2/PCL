//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// RotationProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RotationProcess.h"
#include "RotationParameters.h"
#include "RotationInstance.h"
#include "RotationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RotationProcess* TheRotationProcess = nullptr;

// ----------------------------------------------------------------------------

RotationProcess::RotationProcess()
{
   TheRotationProcess = this;

   TheRTRotationAngleParameter = new RotationAngle( this );
   TheRTOptimizeFastRotationsParameter = new OptimizeFastRotations( this );
   TheRTInterpolationAlgorithmParameter = new InterpolationAlgorithm( this );
   TheRTClampingThresholdParameter = new ClampingThreshold( this );
   TheRTSmoothnessParameter = new Smoothness( this );
   TheRTGammaCorrectionParameter = new GammaCorrection( this );
   TheRTFillRedParameter = new FillRed( this );
   TheRTFillGreenParameter = new FillGreen( this );
   TheRTFillBlueParameter = new FillBlue( this );
   TheRTFillAlphaParameter = new FillAlpha( this );
   TheRTNoGUIMessagesParameter = new NoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString RotationProcess::Id() const
{
   return "Rotation";
}

// ----------------------------------------------------------------------------

IsoString RotationProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 RotationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String RotationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String RotationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Rotation.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* RotationProcess::DefaultInterface() const
{
   return TheRotationInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* RotationProcess::Create() const
{
   return new RotationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* RotationProcess::Clone( const ProcessImplementation& p ) const
{
   const RotationInstance* instPtr = dynamic_cast<const RotationInstance*>( &p );
   return (instPtr != nullptr) ? new RotationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RotationProcess.cpp - Released 2025-04-07T08:53:55Z
