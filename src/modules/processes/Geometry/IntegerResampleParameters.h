//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __IntegerResampleParameters_h
#define __IntegerResampleParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class IRZoomFactor : public MetaInt32
{
public:

   IRZoomFactor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IRZoomFactor* TheIRZoomFactorParameter;

// ----------------------------------------------------------------------------

class IRDownsamplingMode : public MetaEnumeration
{
public:

   enum { Average,
          Median,
          Maximum,
          Minimum,
          NumberOfModes,
          Default = Average };

   IRDownsamplingMode( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IRDownsamplingMode* TheIRDownsamplingModeParameter;

// ----------------------------------------------------------------------------

extern XResolution* TheIRXResolutionParameter;
extern YResolution* TheIRYResolutionParameter;
extern MetricResolution* TheIRMetricResolutionParameter;
extern ForceResolution* TheIRForceResolutionParameter;
extern GammaCorrection* TheIRGammaCorrectionParameter;

extern NoGUIMessages* TheIRNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __IntegerResampleParameters_h

// ----------------------------------------------------------------------------
// EOF IntegerResampleParameters.h - Released 2025-04-07T08:53:56Z
