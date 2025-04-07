//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CloneStampParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CSClonerTable*      TheCSClonerTableParameter = nullptr;
CSActionIndex*      TheCSActionIndexParameter = nullptr;
CSTargetX*          TheCSTargetXParameter = nullptr;
CSTargetY*          TheCSTargetYParameter = nullptr;
CSActionTable*      TheCSActionTableParameter = nullptr;
CSSourceIdentifier* TheCSSourceIdentifierParameter = nullptr;
CSSourceWidth*      TheCSSourceWidthParameter = nullptr;
CSSourceHeight*     TheCSSourceHeightParameter = nullptr;
CSSourceX*          TheCSSourceXParameter = nullptr;
CSSourceY*          TheCSSourceYParameter = nullptr;
CSRadius*           TheCSRadiusParameter = nullptr;
CSSoftness*         TheCSSoftnessParameter = nullptr;
CSOpacity*          TheCSOpacityParameter = nullptr;
CSWidth*            TheCSWidthParameter = nullptr;
CSHeight*           TheCSHeightParameter = nullptr;
CSColor*            TheCSColorParameter = nullptr;
CSBoundsColor*      TheCSBoundsColorParameter = nullptr;

// ----------------------------------------------------------------------------

CSClonerTable::CSClonerTable( MetaProcess* P ) : MetaTable( P )
{
   TheCSClonerTableParameter = this;
}

IsoString CSClonerTable::Id() const
{
   return "cloner";
}

// ----------------------------------------------------------------------------

CSActionIndex::CSActionIndex( MetaTable* T ) : MetaUInt32( T )
{
   TheCSActionIndexParameter = this;
}

IsoString CSActionIndex::Id() const
{
   return "actionIndex";
}

// ----------------------------------------------------------------------------

CSTargetX::CSTargetX( MetaTable* T ) : MetaInt32( T )
{
   TheCSTargetXParameter = this;
}

IsoString CSTargetX::Id() const
{
   return "targetX";
}

// ----------------------------------------------------------------------------

CSTargetY::CSTargetY( MetaTable* T ) : MetaInt32( T )
{
   TheCSTargetYParameter = this;
}

IsoString CSTargetY::Id() const
{
   return "targetY";
}

// ----------------------------------------------------------------------------

CSActionTable::CSActionTable( MetaProcess* P ) : MetaTable( P )
{
   TheCSActionTableParameter = this;
}

IsoString CSActionTable::Id() const
{
   return "actions";
}

// ----------------------------------------------------------------------------

CSSourceIdentifier::CSSourceIdentifier( MetaTable* T ) : MetaString( T )
{
   TheCSSourceIdentifierParameter = this;
}

IsoString CSSourceIdentifier::Id() const
{
   return "sourceId";
}

size_type CSSourceIdentifier::MinLength() const
{
   return 0; // can be void
}

String CSSourceIdentifier::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

CSSourceWidth::CSSourceWidth( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceWidthParameter = this;
}

IsoString CSSourceWidth::Id() const
{
   return "sourceWidth";
}

double CSSourceWidth::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CSSourceHeight::CSSourceHeight( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceHeightParameter = this;
}

IsoString CSSourceHeight::Id() const
{
   return "sourceHeight";
}

double CSSourceHeight::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

CSSourceX::CSSourceX( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceXParameter = this;
}

IsoString CSSourceX::Id() const
{
   return "sourceX";
}

// ----------------------------------------------------------------------------

CSSourceY::CSSourceY( MetaTable* T ) : MetaInt32( T )
{
   TheCSSourceYParameter = this;
}

IsoString CSSourceY::Id() const
{
   return "sourceY";
}

// ----------------------------------------------------------------------------

CSRadius::CSRadius( MetaTable* T ) : MetaInt32( T )
{
   TheCSRadiusParameter = this;
}

IsoString CSRadius::Id() const
{
   return "radius";
}

double CSRadius::DefaultValue() const
{
   return 5;
}

double CSRadius::MinimumValue() const
{
   return 0;
}

double CSRadius::MaximumValue() const
{
   return 255;
}

// ----------------------------------------------------------------------------

CSSoftness::CSSoftness( MetaTable* T ) : MetaFloat( T )
{
   TheCSSoftnessParameter = this;
}

IsoString CSSoftness::Id() const
{
   return "softness";
}

double CSSoftness::DefaultValue() const
{
   return 0.5;
}

double CSSoftness::MinimumValue() const
{
   return 0;
}

double CSSoftness::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CSOpacity::CSOpacity( MetaTable* T ) : MetaFloat( T )
{
   TheCSOpacityParameter = this;
}

IsoString CSOpacity::Id() const
{
   return "opacity";
}

double CSOpacity::DefaultValue() const
{
   return 1;
}

double CSOpacity::MinimumValue() const
{
   return 0;
}

double CSOpacity::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CSWidth::CSWidth( MetaProcess* P ) : MetaInt32( P )
{
   TheCSWidthParameter = this;
}

IsoString CSWidth::Id() const
{
   return "width";
}

// ----------------------------------------------------------------------------

CSHeight::CSHeight( MetaProcess* P ) : MetaInt32( P )
{
   TheCSHeightParameter = this;
}

IsoString CSHeight::Id() const
{
   return "height";
}

// ----------------------------------------------------------------------------

CSColor::CSColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheCSColorParameter = this;
}

IsoString CSColor::Id() const
{
   return "clonerColor";
}

double CSColor::DefaultValue() const
{
   return 0xffe0e0e0;
}

// ----------------------------------------------------------------------------

CSBoundsColor::CSBoundsColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheCSBoundsColorParameter = this;
}

IsoString CSBoundsColor::Id() const
{
   return "boundsColor";
}

double CSBoundsColor::DefaultValue() const
{
   return 0xffe0e0e0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CloneStampParameters.cpp - Released 2025-04-07T08:53:55Z
