//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "IntegerResampleParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IRZoomFactor*       TheIRZoomFactorParameter = nullptr;
IRDownsamplingMode* TheIRDownsamplingModeParameter = nullptr;
XResolution*        TheIRXResolutionParameter = nullptr;
YResolution*        TheIRYResolutionParameter = nullptr;
MetricResolution*   TheIRMetricResolutionParameter = nullptr;
ForceResolution*    TheIRForceResolutionParameter = nullptr;
GammaCorrection*    TheIRGammaCorrectionParameter = nullptr;
NoGUIMessages*      TheIRNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

IRZoomFactor::IRZoomFactor( MetaProcess* P ) : MetaInt32( P )
{
   TheIRZoomFactorParameter = this;
}

IsoString IRZoomFactor::Id() const
{
   return "zoomFactor";
}

double IRZoomFactor::DefaultValue() const
{
   return -2;
}

double IRZoomFactor::MinimumValue() const
{
   return -256;
}

double IRZoomFactor::MaximumValue() const
{
   return +256;
}

// ----------------------------------------------------------------------------

IRDownsamplingMode::IRDownsamplingMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIRDownsamplingModeParameter = this;
}

IsoString IRDownsamplingMode::Id() const
{
   return "downsamplingMode";
}

IsoString IRDownsamplingMode::Aliases() const
{
   return "subsampleMode"; // for compatibility with old versions
}

size_type IRDownsamplingMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString IRDownsamplingMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Average:  return "Average";
   case Median:   return "Median";
   case Maximum:  return "Maximum";
   case Minimum:  return "Minimum";
   }
}

int IRDownsamplingMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IRDownsamplingMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleParameters.cpp - Released 2025-04-07T08:53:55Z
