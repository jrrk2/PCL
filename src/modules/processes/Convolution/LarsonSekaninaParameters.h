//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// LarsonSekaninaParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LarsonSekaninaParameters_h
#define __LarsonSekaninaParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LSInterpolation : public MetaEnumeration
{
public:

   enum { Bilinear,
          Bicubic,
          BicubicSpline,
          BicubicBSpline,
          NumberOfInterpolationModes,
          Default = Bicubic };

   LSInterpolation( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LSInterpolation* TheLSInterpolationParameter;

// ----------------------------------------------------------------------------

class LSRadiusDiff : public MetaFloat
{
public:

   LSRadiusDiff( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSRadiusDiff* TheLSRadiusDiffParameter;

// ----------------------------------------------------------------------------

class LSAngleDiff : public MetaFloat
{
public:

   LSAngleDiff( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSAngleDiff* TheLSAngleDiffParameter;

// ----------------------------------------------------------------------------

class LSCenterX : public MetaFloat
{
public:

   LSCenterX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSCenterX* TheLSCenterXParameter;

// ----------------------------------------------------------------------------

class LSCenterY : public MetaFloat
{
public:

   LSCenterY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSCenterY* TheLSCenterYParameter;

// ----------------------------------------------------------------------------

class LSAmount : public MetaFloat
{
public:

   LSAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSAmount* TheLSAmountParameter;

// ----------------------------------------------------------------------------

class LSThreshold : public MetaFloat
{
public:

   LSThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSThreshold* TheLSThresholdParameter;

// ----------------------------------------------------------------------------

class LSDeringing : public MetaFloat
{
public:

   LSDeringing( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSDeringing* TheLSDeringingParameter;

// ----------------------------------------------------------------------------

class LSRangeLow : public MetaFloat
{
public:

   LSRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSRangeLow* TheLSRangeLowParameter;

// ----------------------------------------------------------------------------

class LSRangeHigh : public MetaFloat
{
public:

   LSRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern LSRangeHigh* TheLSRangeHighParameter;

// ----------------------------------------------------------------------------

class LSUseLuminance : public MetaBoolean
{
public:

   LSUseLuminance( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LSUseLuminance* TheLSUseLuminanceParameter;

// ----------------------------------------------------------------------------

class LSHighPass : public MetaBoolean
{
public:

   LSHighPass( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LSHighPass* TheLSHighPassParameter;

// ----------------------------------------------------------------------------

class LSDisableExtension : public MetaBoolean
{
public:

   LSDisableExtension( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LSDisableExtension* TheLSDisableExtensionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ShapernParameters_h

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaParameters.h - Released 2025-04-07T08:53:55Z
