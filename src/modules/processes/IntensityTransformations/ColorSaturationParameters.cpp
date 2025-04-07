//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ColorSaturationParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ColorSaturationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#define DEFINE_PARAMETER( classId )    classId* The##classId##Parameter = nullptr

DEFINE_PARAMETER( HueSaturationCurve );
DEFINE_PARAMETER( XHS );
DEFINE_PARAMETER( YHS );
DEFINE_PARAMETER( HueSaturationCurveType );

DEFINE_PARAMETER( HueShift );

// ----------------------------------------------------------------------------

HSCurvePointValue::HSCurvePointValue( MetaTable* t ) : CurvePointValue( t )
{
}

double HSCurvePointValue::MinimumValue() const
{
   return -10.0;
}

double HSCurvePointValue::MaximumValue() const
{
   return 10.0;
}

// ----------------------------------------------------------------------------

HueShift::HueShift( MetaProcess* P ) : MetaFloat( P )
{
   TheHueShiftParameter = this;
}

IsoString HueShift::Id() const
{
   return "hueShift";
}

int HueShift::Precision() const
{
   return 3;
}

double HueShift::MinimumValue() const
{
   return 0;
}

double HueShift::MaximumValue() const
{
   return 1;
}

double HueShift::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorSaturationParameters.cpp - Released 2025-04-07T08:53:56Z
