//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// CurvesTransformationParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CurvesTransformationParameters_h
#define __CurvesTransformationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CurveIndex
{
public:

   enum { R, G, B, RGBK, A, L, a, b, c, H, S, NumberOfCurves };
};

// ----------------------------------------------------------------------------

class TableIndexer
{
public:

   virtual int CurveTableIndex() const = 0;
};

// ----------------------------------------------------------------------------

class CurveTable : public MetaTable, public TableIndexer
{
public:

   CurveTable( MetaProcess* P ) : MetaTable( P )
   {
   }

   virtual IsoString Id() const = 0;
   virtual int CurveTableIndex() const = 0;
};

#define DECLARE_CURVE_TABLE_PARAMETER( classId, parId, idx )               \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public CurveTable                                          \
{                                                                          \
public:                                                                    \
   classId( MetaProcess* P ) : CurveTable( P )                             \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   IsoString Id() const override { return parId; }                         \
   int CurveTableIndex() const override { return idx; }                    \
}

DECLARE_CURVE_TABLE_PARAMETER( RedCurve,        "R", CurveIndex::R );
DECLARE_CURVE_TABLE_PARAMETER( GreenCurve,      "G", CurveIndex::G );
DECLARE_CURVE_TABLE_PARAMETER( BlueCurve,       "B", CurveIndex::B );
DECLARE_CURVE_TABLE_PARAMETER( RGBKCurve,       "K", CurveIndex::RGBK );
DECLARE_CURVE_TABLE_PARAMETER( AlphaCurve,      "A", CurveIndex::A );
DECLARE_CURVE_TABLE_PARAMETER( CIELCurve,       "L", CurveIndex::L );
DECLARE_CURVE_TABLE_PARAMETER( CIEaCurve,       "a", CurveIndex::a );
DECLARE_CURVE_TABLE_PARAMETER( CIEbCurve,       "b", CurveIndex::b );
DECLARE_CURVE_TABLE_PARAMETER( CIEcCurve,       "c", CurveIndex::c );
DECLARE_CURVE_TABLE_PARAMETER( HueCurve,        "H", CurveIndex::H );
DECLARE_CURVE_TABLE_PARAMETER( SaturationCurve, "S", CurveIndex::S );

// ----------------------------------------------------------------------------

class CurvePointValue : public MetaDouble, public TableIndexer
{
public:

   CurvePointValue( MetaTable* );

   virtual IsoString Id() const = 0;

   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;

   int CurveTableIndex() const override
   {
      return dynamic_cast<const TableIndexer*>( Parent() )->CurveTableIndex();
   }

   virtual bool IsX() const = 0;
};

#define DECLARE_CURVE_POINT_VALUE_PARAMETER( classId, parId, isX )         \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public CurvePointValue                                     \
{                                                                          \
public:                                                                    \
   classId( MetaTable* t ) : CurvePointValue( t )                          \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   IsoString Id() const override { return parId; }                         \
   bool IsX() const override { return isX; }                               \
}

DECLARE_CURVE_POINT_VALUE_PARAMETER( XR, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YR, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XG, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YG, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XB, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YB, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XRGBK, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YRGBK, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XA, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YA, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XL, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YL, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( Xa, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( Ya, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( Xb, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( Yb, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( Xc, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( Yc, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XH, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YH, "y", false );

DECLARE_CURVE_POINT_VALUE_PARAMETER( XS, "x", true  );
DECLARE_CURVE_POINT_VALUE_PARAMETER( YS, "y", false );

// ----------------------------------------------------------------------------

class CurveType : public MetaEnumeration, public TableIndexer
{
public:

   enum { CubicSpline,
          Linear,
          AkimaSubsplines,
          NumberOfCurveTypes,
          Default = AkimaSubsplines };

   CurveType( MetaProcess* );

   virtual IsoString Id() const = 0;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
   virtual int CurveTableIndex() const = 0;
};

#define DECLARE_CURVE_TYPE_PARAMETER( classId, parId, idx )                \
class classId;                                                             \
extern classId* The##classId##Parameter;                                   \
class classId : public CurveType                                           \
{                                                                          \
public:                                                                    \
   classId( MetaProcess* P ) : CurveType( P )                              \
   { if ( The##classId##Parameter == 0 ) The##classId##Parameter = this; } \
   IsoString Id() const override { return parId; }                         \
   int CurveTableIndex() const override { return idx; }                    \
}

DECLARE_CURVE_TYPE_PARAMETER( RedCurveType,        "Rt", CurveIndex::R );
DECLARE_CURVE_TYPE_PARAMETER( GreenCurveType,      "Gt", CurveIndex::G );
DECLARE_CURVE_TYPE_PARAMETER( BlueCurveType,       "Bt", CurveIndex::B );
DECLARE_CURVE_TYPE_PARAMETER( RGBKCurveType,       "Kt", CurveIndex::RGBK );
DECLARE_CURVE_TYPE_PARAMETER( AlphaCurveType,      "At", CurveIndex::A );
DECLARE_CURVE_TYPE_PARAMETER( CIELCurveType,       "Lt", CurveIndex::L );
DECLARE_CURVE_TYPE_PARAMETER( CIEaCurveType,       "at", CurveIndex::a );
DECLARE_CURVE_TYPE_PARAMETER( CIEbCurveType,       "bt", CurveIndex::b );
DECLARE_CURVE_TYPE_PARAMETER( CIEcCurveType,       "ct", CurveIndex::c );
DECLARE_CURVE_TYPE_PARAMETER( HueCurveType,        "Ht", CurveIndex::H );
DECLARE_CURVE_TYPE_PARAMETER( SaturationCurveType, "St", CurveIndex::S );

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CurvesTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF CurvesTransformationParameters.h - Released 2025-04-07T08:53:56Z
