//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AdaptiveStretchParameters_h
#define __AdaptiveStretchParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ASNoiseThreshold : public MetaDouble
{
public:

   ASNoiseThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern ASNoiseThreshold* TheASNoiseThresholdParameter;

// ----------------------------------------------------------------------------

class ASProtection : public MetaDouble
{
public:

   ASProtection( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern ASProtection* TheASProtectionParameter;

// ----------------------------------------------------------------------------

class ASUseProtection : public MetaBoolean
{
public:

   ASUseProtection( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ASUseProtection* TheASUseProtectionParameter;

// ----------------------------------------------------------------------------

class ASMaxCurvePoints : public MetaInt32
{
public:

   ASMaxCurvePoints( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ASMaxCurvePoints* TheASMaxCurvePointsParameter;

// ----------------------------------------------------------------------------

class ASUseROI : public MetaBoolean
{
public:

   ASUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ASUseROI* TheASUseROIParameter;

// ----------------------------------------------------------------------------

class ASROIX0 : public MetaInt32
{
public:

   ASROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ASROIX0* TheASROIX0Parameter;

// ----------------------------------------------------------------------------

class ASROIY0 : public MetaInt32
{
public:

   ASROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ASROIY0* TheASROIY0Parameter;

// ----------------------------------------------------------------------------

class ASROIX1 : public MetaInt32
{
public:

   ASROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ASROIX1* TheASROIX1Parameter;

// ----------------------------------------------------------------------------

class ASROIY1 : public MetaInt32
{
public:

   ASROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ASROIY1* TheASROIY1Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AdaptiveStretchParameters_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchParameters.h - Released 2025-04-07T08:53:56Z
