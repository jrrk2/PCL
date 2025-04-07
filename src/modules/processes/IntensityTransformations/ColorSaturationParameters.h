//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ColorSaturationParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ColorSaturationParameters_h
#define __ColorSaturationParameters_h

#include "CurvesTransformationParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

DECLARE_CURVE_TABLE_PARAMETER( HueSaturationCurve, "HS", 0 );

// ----------------------------------------------------------------------------

class HSCurvePointValue : public CurvePointValue
{
public:

   HSCurvePointValue( MetaTable* );

   virtual IsoString Id() const = 0;
   double MinimumValue() const override;
   double MaximumValue() const override;
   virtual bool IsX() const = 0;
};

#define DECLARE_HSCURVE_POINT_VALUE_PARAMETER( classId, parId, isX )       \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public HSCurvePointValue                                   \
{                                                                          \
public:                                                                    \
   classId( MetaTable* t ) : HSCurvePointValue( t )                        \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   IsoString Id() const override { return parId; }                         \
   bool IsX() const override { return isX; }                               \
}

DECLARE_HSCURVE_POINT_VALUE_PARAMETER( XHS, "x", true  );
DECLARE_HSCURVE_POINT_VALUE_PARAMETER( YHS, "y", false );

// ----------------------------------------------------------------------------

DECLARE_CURVE_TYPE_PARAMETER( HueSaturationCurveType, "HSt", 0 );

// ----------------------------------------------------------------------------

class HueShift : public MetaFloat
{
public:

   HueShift( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern HueShift* TheHueShiftParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorSaturationParameters_h

// ----------------------------------------------------------------------------
// EOF ColorSaturationParameters.h - Released 2025-04-07T08:53:56Z
