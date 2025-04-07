//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ResampleParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ResampleParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RSXSize*                TheRSXSizeParameter = nullptr;
RSYSize*                TheRSYSizeParameter = nullptr;
RSMode*                 TheRSModeParameter = nullptr;
RSAbsoluteMode*         TheRSAbsoluteModeParameter = nullptr;
XResolution*            TheRSXResolutionParameter = nullptr;
YResolution*            TheRSYResolutionParameter = nullptr;
MetricResolution*       TheRSMetricResolutionParameter = nullptr;
ForceResolution*        TheRSForceResolutionParameter = nullptr;
InterpolationAlgorithm* TheRSInterpolationAlgorithmParameter = nullptr;
ClampingThreshold*      TheRSClampingThresholdParameter = nullptr;
Smoothness*             TheRSSmoothnessParameter = nullptr;
GammaCorrection*        TheRSGammaCorrectionParameter = nullptr;
NoGUIMessages*          TheRSNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

RSXSize::RSXSize( MetaProcess* P ) : MetaDouble( P )
{
   TheRSXSizeParameter = this;
}

IsoString RSXSize::Id() const
{
   return "xSize";
}

int RSXSize::Precision() const
{
   return 4;
}

double RSXSize::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RSYSize::RSYSize( MetaProcess* P ) : MetaDouble( P )
{
   TheRSYSizeParameter = this;
}

IsoString RSYSize::Id() const
{
   return "ySize";
}

int RSYSize::Precision() const
{
   return 4;
}

double RSYSize::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

RSMode::RSMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheRSModeParameter = this;
}

IsoString RSMode::Id() const
{
   return "mode";
}

size_type RSMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString RSMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RelativeDimensions:   return "RelativeDimensions";
   case AbsolutePixels:       return "AbsolutePixels";
   case AbsoluteCentimeters:  return "AbsoluteCentimeters";
   case AbsoluteInches:       return "AbsoluteInches";
   case ForceArea:            return "ForceArea";
   }
}

int RSMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type RSMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

RSAbsoluteMode::RSAbsoluteMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheRSAbsoluteModeParameter = this;
}

IsoString RSAbsoluteMode::Id() const
{
   return "absoluteMode";
}

size_type RSAbsoluteMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString RSAbsoluteMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case ForceWidthAndHeight:  return "ForceWidthAndHeight";
   case ForceWidth:           return "ForceWidth";
   case ForceHeight:          return "ForceHeight";
   }
}

int RSAbsoluteMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type RSAbsoluteMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ResampleParameters.cpp - Released 2025-04-07T08:53:55Z
