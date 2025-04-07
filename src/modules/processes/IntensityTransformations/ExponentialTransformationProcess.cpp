//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ExponentialTransformationProcess.h"
#include "ExponentialTransformationParameters.h"
#include "ExponentialTransformationInstance.h"
#include "ExponentialTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialTransformationProcess* TheExponentialTransformationProcess = nullptr;

// ----------------------------------------------------------------------------

ExponentialTransformationProcess::ExponentialTransformationProcess()
{
   TheExponentialTransformationProcess = this;

   new ExponentialFunctionType( this );
   new ExponentialFunctionOrder( this );
   new ExponentialFunctionSmoothing( this );
   new ExponentialFunctionMask( this );
}

// ----------------------------------------------------------------------------

IsoString ExponentialTransformationProcess::Id() const
{
   return "ExponentialTransformation";
}

// ----------------------------------------------------------------------------

IsoString ExponentialTransformationProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 ExponentialTransformationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ExponentialTransformationProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ExponentialTransformationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ExponentialTransformation.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ExponentialTransformationProcess::DefaultInterface() const
{
   return TheExponentialTransformationInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ExponentialTransformationProcess::Create() const
{
   return new ExponentialTransformationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ExponentialTransformationProcess::Clone( const ProcessImplementation& p ) const
{
   const ExponentialTransformationInstance* instPtr = dynamic_cast<const ExponentialTransformationInstance*>( &p );
   return (instPtr != nullptr) ? new ExponentialTransformationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationProcess.cpp - Released 2025-04-07T08:53:56Z
