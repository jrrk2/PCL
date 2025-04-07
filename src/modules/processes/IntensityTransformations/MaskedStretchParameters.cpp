//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// MaskedStretchParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "MaskedStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MSTargetBackground*          TheMSTargetBackgroundParameter = nullptr;
MSNumberOfIterations*        TheMSNumberOfIterationsParameter = nullptr;
MSClippingFraction*          TheMSClippingFractionParameter = nullptr;
MSBackgroundReferenceViewId* TheMSBackgroundReferenceViewIdParameter = nullptr;
MSBackgroundLow*             TheMSBackgroundLowParameter = nullptr;
MSBackgroundHigh*            TheMSBackgroundHighParameter = nullptr;
MSUseROI*                    TheMSUseROIParameter = nullptr;
MSROIX0*                     TheMSROIX0Parameter = nullptr;
MSROIY0*                     TheMSROIY0Parameter = nullptr;
MSROIX1*                     TheMSROIX1Parameter = nullptr;
MSROIY1*                     TheMSROIY1Parameter = nullptr;
MSMaskType*                  TheMSMaskTypeParameter = nullptr;

// ----------------------------------------------------------------------------

MSTargetBackground::MSTargetBackground( MetaProcess* P ) : MetaDouble( P )
{
   TheMSTargetBackgroundParameter = this;
}

IsoString MSTargetBackground::Id() const
{
   return "targetBackground";
}

int MSTargetBackground::Precision() const
{
   return 8;
}

double MSTargetBackground::MinimumValue() const
{
   return 0;
}

double MSTargetBackground::MaximumValue() const
{
   return 1;
}

double MSTargetBackground::DefaultValue() const
{
   return 0.125;
}

// ----------------------------------------------------------------------------

MSNumberOfIterations::MSNumberOfIterations( MetaProcess* P ) : MetaInt32( P )
{
   TheMSNumberOfIterationsParameter = this;
}

IsoString MSNumberOfIterations::Id() const
{
   return "numberOfIterations";
}

double MSNumberOfIterations::DefaultValue() const
{
   return 100;
}

double MSNumberOfIterations::MinimumValue() const
{
   return 2;
}

double MSNumberOfIterations::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

MSClippingFraction::MSClippingFraction( MetaProcess* P ) : MetaDouble( P )
{
   TheMSClippingFractionParameter = this;
}

IsoString MSClippingFraction::Id() const
{
   return "clippingFraction";
}

int MSClippingFraction::Precision() const
{
   return 8;
}

double MSClippingFraction::MinimumValue() const
{
   return 0;
}

double MSClippingFraction::MaximumValue() const
{
   return 1;
}

double MSClippingFraction::DefaultValue() const
{
   return 0.0005;
}

// ----------------------------------------------------------------------------

MSBackgroundReferenceViewId::MSBackgroundReferenceViewId( MetaProcess* P ) : MetaString( P )
{
   TheMSBackgroundReferenceViewIdParameter = this;
}

IsoString MSBackgroundReferenceViewId::Id() const
{
   return "backgroundReferenceViewId";
}

// ----------------------------------------------------------------------------

MSBackgroundLow::MSBackgroundLow( MetaProcess* P ) : MetaDouble( P )
{
   TheMSBackgroundLowParameter = this;
}

IsoString MSBackgroundLow::Id() const
{
   return "backgroundLow";
}

int MSBackgroundLow::Precision() const
{
   return 8;
}

double MSBackgroundLow::MinimumValue() const
{
   return 0;
}

double MSBackgroundLow::MaximumValue() const
{
   return 1;
}

double MSBackgroundLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

MSBackgroundHigh::MSBackgroundHigh( MetaProcess* P ) : MetaDouble( P )
{
   TheMSBackgroundHighParameter = this;
}

IsoString MSBackgroundHigh::Id() const
{
   return "backgroundHigh";
}

int MSBackgroundHigh::Precision() const
{
   return 8;
}

double MSBackgroundHigh::MinimumValue() const
{
   return 0;
}

double MSBackgroundHigh::MaximumValue() const
{
   return 1;
}

double MSBackgroundHigh::DefaultValue() const
{
   return 0.05;
}

// ----------------------------------------------------------------------------

MSUseROI::MSUseROI( MetaProcess* P ) : MetaBoolean( P )
{
   TheMSUseROIParameter = this;
}

IsoString MSUseROI::Id() const
{
   return "useROI";
}

bool MSUseROI::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

MSROIX0::MSROIX0( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIX0Parameter = this;
}

IsoString MSROIX0::Id() const
{
   return "roiX0";
}

double MSROIX0::DefaultValue() const
{
   return 0;
}

double MSROIX0::MinimumValue() const
{
   return 0;
}

double MSROIX0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSROIY0::MSROIY0( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIY0Parameter = this;
}

IsoString MSROIY0::Id() const
{
   return "roiY0";
}

double MSROIY0::DefaultValue() const
{
   return 0;
}

double MSROIY0::MinimumValue() const
{
   return 0;
}

double MSROIY0::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSROIX1::MSROIX1( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIX1Parameter = this;
}

IsoString MSROIX1::Id() const
{
   return "roiX1";
}

double MSROIX1::DefaultValue() const
{
   return 0;
}

double MSROIX1::MinimumValue() const
{
   return 0;
}

double MSROIX1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSROIY1::MSROIY1( MetaProcess* P ) : MetaInt32( P )
{
   TheMSROIY1Parameter = this;
}

IsoString MSROIY1::Id() const
{
   return "roiY1";
}

double MSROIY1::DefaultValue() const
{
   return 0;
}

double MSROIY1::MinimumValue() const
{
   return 0;
}

double MSROIY1::MaximumValue() const
{
   return int32_max;
}

// ----------------------------------------------------------------------------

MSMaskType::MSMaskType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheMSMaskTypeParameter = this;
}

IsoString MSMaskType::Id() const
{
   return "maskType";
}

size_type MSMaskType::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString MSMaskType::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Intensity: return "MaskType_Intensity";
   case Value:     return "MaskType_Value";
   }
}

int MSMaskType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type MSMaskType::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MaskedStretchParameters.cpp - Released 2025-04-07T08:53:56Z
