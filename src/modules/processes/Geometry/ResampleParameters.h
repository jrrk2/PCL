//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ResampleParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ResampleParameters_h
#define __ResampleParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class RSXSize : public MetaDouble
{
public:

   RSXSize( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern RSXSize* TheRSXSizeParameter;

// ----------------------------------------------------------------------------

class RSYSize : public MetaDouble
{
public:

   RSYSize( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern RSYSize* TheRSYSizeParameter;

// ----------------------------------------------------------------------------

class RSMode : public MetaEnumeration
{
public:

   enum { RelativeDimensions,    // relative to current image dimensions
          AbsolutePixels,        // to absolute dimensions in pixels
          AbsoluteCentimeters,   // to absolute dimensions in centimeters
          AbsoluteInches,        // to absolute dimensions in inches
          ForceArea,             // force number of pixels, keep aspect ratio
          NumberOfModes,
          Default = RelativeDimensions };

   RSMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern RSMode* TheRSModeParameter;

// ----------------------------------------------------------------------------

class RSAbsoluteMode : public MetaEnumeration
{
public:

   enum { ForceWidthAndHeight,   // force both dimensions
          ForceWidth,            // force width, preserve aspect ratio
          ForceHeight,           // force height, preserve aspect ratio
          NumberOfModes,
          Default = ForceWidthAndHeight };

   RSAbsoluteMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern RSAbsoluteMode* TheRSAbsoluteModeParameter;

// ----------------------------------------------------------------------------

extern XResolution* TheRSXResolutionParameter;
extern YResolution* TheRSYResolutionParameter;
extern MetricResolution* TheRSMetricResolutionParameter;
extern ForceResolution* TheRSForceResolutionParameter;

extern InterpolationAlgorithm* TheRSInterpolationAlgorithmParameter;
extern ClampingThreshold* TheRSClampingThresholdParameter;
extern Smoothness* TheRSSmoothnessParameter;
extern GammaCorrection* TheRSGammaCorrectionParameter;

extern NoGUIMessages* TheRSNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ResampleParameters_h

// ----------------------------------------------------------------------------
// EOF ResampleParameters.h - Released 2025-04-07T08:53:55Z
