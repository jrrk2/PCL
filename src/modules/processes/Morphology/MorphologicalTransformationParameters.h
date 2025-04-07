//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MorphologicalTransformationParameters_h
#define __MorphologicalTransformationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MTOperator : public MetaEnumeration
{
public:

   enum { Erosion, Dilation, Opening, Closing, Median, Selection, Midpoint,
          NumberOfMorphologicalOps, Default = Erosion };

   MTOperator( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern MTOperator* TheMTOperatorParameter;

// ----------------------------------------------------------------------------

class MTInterlacingDistance : public MetaUInt32
{
public:

   MTInterlacingDistance( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTInterlacingDistance* TheMTInterlacingDistanceParameter;

// ----------------------------------------------------------------------------

class MTLowThreshold : public MetaDouble
{
public:

   MTLowThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTLowThreshold* TheMTLowThresholdParameter;

// ----------------------------------------------------------------------------

class MTHighThreshold : public MetaDouble
{
public:

   MTHighThreshold( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTHighThreshold* TheMTHighThresholdParameter;

// ----------------------------------------------------------------------------

class MTNumberOfIterations : public MetaUInt32
{
public:

   MTNumberOfIterations( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTNumberOfIterations* TheMTNumberOfIterationsParameter;

// ----------------------------------------------------------------------------

class MTAmount : public MetaFloat
{
public:

   MTAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTAmount* TheMTAmountParameter;

// ----------------------------------------------------------------------------

class MTSelectionPoint : public MetaFloat
{
public:

   MTSelectionPoint( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTSelectionPoint* TheMTSelectionPointParameter;

// ----------------------------------------------------------------------------

class MTStructureName : public MetaString
{
public:

   MTStructureName( MetaProcess* );

   IsoString Id() const override;
};

extern MTStructureName* TheMTStructureNameParameter;

// ----------------------------------------------------------------------------

class MTStructureSize : public MetaUInt32
{
public:

   MTStructureSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MTStructureSize* TheMTStructureSizeParameter;

// ----------------------------------------------------------------------------

class MTStructureWayTable : public MetaTable
{
public:

   MTStructureWayTable( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
};

extern MTStructureWayTable* TheMTStructureWayTableParameter;

// ----------------------------------------------------------------------------

class MTStructureWayMask : public MetaBlock
{
public:

   MTStructureWayMask( MetaTable* );

   IsoString Id() const override;
};

extern MTStructureWayMask* TheMTStructureWayMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __MorphologicalTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationParameters.h - Released 2025-04-07T08:53:56Z
