//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AdaptiveStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ASNoiseThreshold* TheASNoiseThresholdParameter = nullptr;
ASProtection*     TheASProtectionParameter = nullptr;
ASUseProtection*  TheASUseProtectionParameter = nullptr;
ASMaxCurvePoints* TheASMaxCurvePointsParameter = nullptr;
ASUseROI*         TheASUseROIParameter = nullptr;
ASROIX0*          TheASROIX0Parameter = nullptr;
ASROIY0*          TheASROIY0Parameter = nullptr;
ASROIX1*          TheASROIX1Parameter = nullptr;
ASROIY1*          TheASROIY1Parameter = nullptr;

// ----------------------------------------------------------------------------

ASNoiseThreshold::ASNoiseThreshold( MetaProcess* P ) : MetaDouble( P )
{
   TheASNoiseThresholdParameter = this;
}

IsoString ASNoiseThreshold::Id() const
{
   return "noiseThreshold";
}

int ASNoiseThreshold::Precision() const
{
   return 8;
}

double ASNoiseThreshold::MinimumValue() const
{
   return 0;
}

double ASNoiseThreshold::MaximumValue() const
{
   return 1;
}

double ASNoiseThreshold::DefaultValue() const
{
   return 0.001;
}

// ------------------------------------------------------------------------

ASProtection::ASProtection( MetaProcess* P ) : MetaDouble( P )
{
   TheASProtectionParameter = this;
}

IsoString ASProtection::Id() const
{
   return "protection";
}

int ASProtection::Precision() const
{
   return 8;
}

double ASProtection::MinimumValue() const
{
   return 0;
}

double ASProtection::MaximumValue() const
{
   return 1;
}

double ASProtection::DefaultValue() const
{
   return 0.01;
}

// ----------------------------------------------------------------------------

ASUseProtection::ASUseProtection( MetaProcess* P ) : MetaBoolean( P )
{
   TheASUseProtectionParameter = this;
}

IsoString ASUseProtection::Id() const
{
   return "useProtection";
}

bool ASUseProtection::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ASMaxCurvePoints::ASMaxCurvePoints( MetaProcess* P ) : MetaInt32( P )
{
   TheASMaxCurvePointsParameter = this;
}

IsoString ASMaxCurvePoints::Id() const
{
   return "maxCurvePoints";
}

double ASMaxCurvePoints::DefaultValue() const
{
   return 1000000;
}

double ASMaxCurvePoints::MinimumValue() const
{
   return 32;
}

double ASMaxCurvePoints::MaximumValue() const
{
   return 100000000;
}

// ----------------------------------------------------------------------------

ASUseROI::ASUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheASUseROIParameter = this;
}

IsoString ASUseROI::Id() const
{
   return "useROI";
}

bool ASUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ASROIX0::ASROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIX0Parameter = this;
}

IsoString ASROIX0::Id() const
{
   return "roiX0";
}

double ASROIX0::DefaultValue() const
{
   return 0;
}

double ASROIX0::MinimumValue() const
{
   return 0;
}

double ASROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

ASROIY0::ASROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIY0Parameter = this;
}

IsoString ASROIY0::Id() const
{
   return "roiY0";
}

double ASROIY0::DefaultValue() const
{
   return 0;
}

double ASROIY0::MinimumValue() const
{
   return 0;
}

double ASROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

ASROIX1::ASROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIX1Parameter = this;
}

IsoString ASROIX1::Id() const
{
   return "roiX1";
}

double ASROIX1::DefaultValue() const
{
   return 0;
}

double ASROIX1::MinimumValue() const
{
   return 0;
}

double ASROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

ASROIY1::ASROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheASROIY1Parameter = this;
}

IsoString ASROIY1::Id() const
{
   return "roiY1";
}

double ASROIY1::DefaultValue() const
{
   return 0;
}

double ASROIY1::MinimumValue() const
{
   return 0;
}

double ASROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchParameters.cpp - Released 2025-04-07T08:53:56Z
