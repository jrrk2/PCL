//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// CurvesTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CurvesTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

#define DEFINE_PARAMETER( classId )    classId* The##classId##Parameter = nullptr

DEFINE_PARAMETER( RedCurve );
DEFINE_PARAMETER( XR );
DEFINE_PARAMETER( YR );
DEFINE_PARAMETER( RedCurveType );

DEFINE_PARAMETER( GreenCurve );
DEFINE_PARAMETER( XG );
DEFINE_PARAMETER( YG );
DEFINE_PARAMETER( GreenCurveType );

DEFINE_PARAMETER( BlueCurve );
DEFINE_PARAMETER( XB );
DEFINE_PARAMETER( YB );
DEFINE_PARAMETER( BlueCurveType );

DEFINE_PARAMETER( RGBKCurve );
DEFINE_PARAMETER( XRGBK );
DEFINE_PARAMETER( YRGBK );
DEFINE_PARAMETER( RGBKCurveType );

DEFINE_PARAMETER( AlphaCurve );
DEFINE_PARAMETER( XA );
DEFINE_PARAMETER( YA );
DEFINE_PARAMETER( AlphaCurveType );

DEFINE_PARAMETER( CIELCurve );
DEFINE_PARAMETER( XL );
DEFINE_PARAMETER( YL );
DEFINE_PARAMETER( CIELCurveType );

DEFINE_PARAMETER( CIEaCurve );
DEFINE_PARAMETER( Xa );
DEFINE_PARAMETER( Ya );
DEFINE_PARAMETER( CIEaCurveType );

DEFINE_PARAMETER( CIEbCurve );
DEFINE_PARAMETER( Xb );
DEFINE_PARAMETER( Yb );
DEFINE_PARAMETER( CIEbCurveType );

DEFINE_PARAMETER( CIEcCurve );
DEFINE_PARAMETER( Xc );
DEFINE_PARAMETER( Yc );
DEFINE_PARAMETER( CIEcCurveType );

DEFINE_PARAMETER( HueCurve );
DEFINE_PARAMETER( XH );
DEFINE_PARAMETER( YH );
DEFINE_PARAMETER( HueCurveType );

DEFINE_PARAMETER( SaturationCurve );
DEFINE_PARAMETER( XS );
DEFINE_PARAMETER( YS );
DEFINE_PARAMETER( SaturationCurveType );

// ----------------------------------------------------------------------------

CurvePointValue::CurvePointValue( MetaTable* t ) : MetaDouble( t )
{
}

int CurvePointValue::Precision() const
{
   return 5;
}

double CurvePointValue::DefaultValue() const
{
   return 0;
}

double CurvePointValue::MinimumValue() const
{
   return 0;
}

double CurvePointValue::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CurveType::CurveType( MetaProcess* P ) : MetaEnumeration( P )
{
}

size_type CurveType::NumberOfElements() const
{
   return NumberOfCurveTypes;
}

IsoString CurveType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case CubicSpline:     return "CubicSpline";
   case Linear:          return "Linear";
   default:
   case AkimaSubsplines: return "AkimaSubsplines";
   }
}

int CurveType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CurveType::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CurvesTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
