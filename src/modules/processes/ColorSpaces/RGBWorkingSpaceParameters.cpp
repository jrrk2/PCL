//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// RGBWorkingSpaceParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RGBWorkingSpaceParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RGBWSChannelTable*         TheRGBWSChannelTableParameter;
RGBWSLuminanceCoefficient* TheRGBWSLuminanceCoefficientParameter = 0;
RGBWSChromaticityX*        TheRGBWSChromaticityXParameter = 0;
RGBWSChromaticityY*        TheRGBWSChromaticityYParameter = 0;
RGBWSGamma*                TheRGBWSGammaParameter = 0;
RGBWSsRGBG*                TheRGBWSsRGBGParameter = 0;
RGBWSApplyGlobal*          TheRGBWSApplyGlobalParameter = 0;

// ----------------------------------------------------------------------------

RGBWSChannelTable::RGBWSChannelTable( MetaProcess* P ) : MetaTable( P )
{
   TheRGBWSChannelTableParameter = this;
}

IsoString RGBWSChannelTable::Id() const
{
   return "channels";
}

size_type RGBWSChannelTable::MinLength() const
{
   return 3;
}

size_type RGBWSChannelTable::MaxLength() const
{
   return 3;
}

// ----------------------------------------------------------------------------

RGBWSLuminanceCoefficient::RGBWSLuminanceCoefficient( MetaTable* T ) : MetaFloat( T )
{
   TheRGBWSLuminanceCoefficientParameter = this;
}

IsoString RGBWSLuminanceCoefficient::Id() const
{
   return "Y";
}

int RGBWSLuminanceCoefficient::Precision() const
{
   return 6;
}

double RGBWSLuminanceCoefficient::MinimumValue() const
{
   return 1.0e-06;
}

double RGBWSLuminanceCoefficient::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

RGBWSChromaticityX::RGBWSChromaticityX( MetaTable* T ) : MetaFloat( T )
{
   TheRGBWSChromaticityXParameter = this;
}

IsoString RGBWSChromaticityX::Id() const
{
   return "x";
}

int RGBWSChromaticityX::Precision() const
{
   return 6;
}

double RGBWSChromaticityX::MinimumValue() const
{
   return 1.0e-06;
}

double RGBWSChromaticityX::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

RGBWSChromaticityY::RGBWSChromaticityY( MetaTable* T ) : MetaFloat( T )
{
   TheRGBWSChromaticityYParameter = this;
}

IsoString RGBWSChromaticityY::Id() const
{
   return "y";
}

int RGBWSChromaticityY::Precision() const
{
   return 6;
}

double RGBWSChromaticityY::MinimumValue() const
{
   return 1.0e-06;
}

double RGBWSChromaticityY::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

RGBWSGamma::RGBWSGamma( MetaProcess* P ) : MetaFloat( P )
{
   TheRGBWSGammaParameter = this;
}

IsoString RGBWSGamma::Id() const
{
   return "gamma";
}

int RGBWSGamma::Precision() const
{
   return 2;
}

double RGBWSGamma::MinimumValue() const
{
   return 0.1;
}

double RGBWSGamma::MaximumValue() const
{
   return 10.0;
}

double RGBWSGamma::DefaultValue() const
{
   return 2.2;
}

// ----------------------------------------------------------------------------

RGBWSsRGBG::RGBWSsRGBG( MetaProcess* P ) : MetaBoolean( P )
{
   TheRGBWSsRGBGParameter = this;
}

IsoString RGBWSsRGBG::Id() const
{
   return "sRGBGamma";
}

bool RGBWSsRGBG::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

RGBWSApplyGlobal::RGBWSApplyGlobal( MetaProcess* P ) : MetaBoolean( P )
{
   TheRGBWSApplyGlobalParameter = this;
}

IsoString RGBWSApplyGlobal::Id() const
{
   return "applyGlobalRGBWS";
}

bool RGBWSApplyGlobal::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceParameters.cpp - Released 2025-04-07T08:53:55Z
