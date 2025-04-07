//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CloneStampParameters_h
#define __CloneStampParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CSClonerTable : public MetaTable
{
public:

   CSClonerTable( MetaProcess* );

   IsoString Id() const override;
};

extern CSClonerTable* TheCSClonerTableParameter;

// ----------------------------------------------------------------------------

class CSActionIndex : public MetaUInt32
{
public:

   CSActionIndex( MetaTable* );

   IsoString Id() const override;
};

extern CSActionIndex* TheCSActionIndexParameter;

// ----------------------------------------------------------------------------

class CSTargetX : public MetaInt32
{
public:

   CSTargetX( MetaTable* );

   IsoString Id() const override;
};

extern CSTargetX* TheCSTargetXParameter;

// ----------------------------------------------------------------------------

class CSTargetY : public MetaInt32
{
public:

   CSTargetY( MetaTable* );

   IsoString Id() const override;
};

extern CSTargetY* TheCSTargetYParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CSActionTable : public MetaTable
{
public:

   CSActionTable( MetaProcess* );

   IsoString Id() const override;
};

extern CSActionTable* TheCSActionTableParameter;

// ----------------------------------------------------------------------------

class CSSourceIdentifier : public MetaString
{
public:

   CSSourceIdentifier( MetaTable* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern CSSourceIdentifier* TheCSSourceIdentifierParameter;

// ----------------------------------------------------------------------------

class CSSourceWidth : public MetaInt32
{
public:

   CSSourceWidth( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSSourceWidth* TheCSSourceWidthParameter;

// ----------------------------------------------------------------------------

class CSSourceHeight : public MetaInt32
{
public:

   CSSourceHeight( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSSourceHeight* TheCSSourceHeightParameter;

// ----------------------------------------------------------------------------

class CSSourceX : public MetaInt32
{
public:

   CSSourceX( MetaTable* );

   IsoString Id() const override;
};

extern CSSourceX* TheCSSourceXParameter;

// ----------------------------------------------------------------------------

class CSSourceY : public MetaInt32
{
public:

   CSSourceY( MetaTable* );

   IsoString Id() const override;
};

extern CSSourceY* TheCSSourceYParameter;

// ----------------------------------------------------------------------------

class CSRadius : public MetaInt32
{
public:

   CSRadius( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CSRadius* TheCSRadiusParameter;

// ----------------------------------------------------------------------------

class CSSoftness : public MetaFloat
{
public:

   CSSoftness( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CSSoftness* TheCSSoftnessParameter;

// ----------------------------------------------------------------------------

class CSOpacity : public MetaFloat
{
public:

   CSOpacity( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CSOpacity* TheCSOpacityParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class CSWidth : public MetaInt32
{
public:

   CSWidth( MetaProcess* );

   IsoString Id() const override;
};

extern CSWidth* TheCSWidthParameter;

// ----------------------------------------------------------------------------

class CSHeight : public MetaInt32
{
public:

   CSHeight( MetaProcess* );

   IsoString Id() const override;
};

extern CSHeight* TheCSHeightParameter;

// ----------------------------------------------------------------------------

class CSColor : public MetaUInt32
{
public:

   CSColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSColor* TheCSColorParameter;

// ----------------------------------------------------------------------------

class CSBoundsColor : public MetaUInt32
{
public:

   CSBoundsColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CSBoundsColor* TheCSBoundsColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CloneStampParameters_h

// ----------------------------------------------------------------------------
// EOF CloneStampParameters.h - Released 2025-04-07T08:53:55Z
