//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "MorphologicalTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

MTOperator*            TheMTOperatorParameter = nullptr;
MTInterlacingDistance* TheMTInterlacingDistanceParameter = nullptr;
MTLowThreshold*        TheMTLowThresholdParameter = nullptr;
MTHighThreshold*       TheMTHighThresholdParameter = nullptr;
MTNumberOfIterations*  TheMTNumberOfIterationsParameter = nullptr;
MTAmount*              TheMTAmountParameter = nullptr;
MTSelectionPoint*      TheMTSelectionPointParameter = nullptr;
MTStructureName*       TheMTStructureNameParameter = nullptr;
MTStructureSize*       TheMTStructureSizeParameter = nullptr;
MTStructureWayTable*   TheMTStructureWayTableParameter = nullptr;
MTStructureWayMask*    TheMTStructureWayMaskParameter = nullptr;

// ----------------------------------------------------------------------------

MTOperator::MTOperator( MetaProcess* P ) : MetaEnumeration( P )
{
   TheMTOperatorParameter = this;
}

IsoString MTOperator::Id() const
{
   return "operator";
}

size_type MTOperator::NumberOfElements() const
{
   return NumberOfMorphologicalOps;
}

IsoString MTOperator::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Erosion:   return "Erosion";
   case Dilation:  return "Dilation";
   case Opening:   return "Opening";
   case Closing:   return "Closing";
   case Median:    return "Median";
   case Selection: return "Selection";
   case Midpoint:  return "Midpoint";
   }
}

int MTOperator::ElementValue( size_type i ) const
{
   return int( i );
}

size_type MTOperator::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

MTInterlacingDistance::MTInterlacingDistance( MetaProcess* P ) : MetaUInt32( P )
{
   TheMTInterlacingDistanceParameter = this;
}

IsoString MTInterlacingDistance::Id() const
{
   return "interlacingDistance";
}

double MTInterlacingDistance::DefaultValue() const
{
   return 1;
}

double MTInterlacingDistance::MinimumValue() const
{
   return 1;
}

double MTInterlacingDistance::MaximumValue() const
{
   return 4096;
}

// ----------------------------------------------------------------------------

MTLowThreshold::MTLowThreshold( MetaProcess* P ) : MetaDouble( P )
{
   TheMTLowThresholdParameter = this;
}

IsoString MTLowThreshold::Id() const
{
   return "lowThreshold";
}

int MTLowThreshold::Precision() const
{
   return 6;
}

double MTLowThreshold::DefaultValue() const
{
   return 0;
}

double MTLowThreshold::MinimumValue() const
{
   return 0;
}

double MTLowThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MTHighThreshold::MTHighThreshold( MetaProcess* P ) : MetaDouble( P )
{
   TheMTHighThresholdParameter = this;
}

IsoString MTHighThreshold::Id() const
{
   return "highThreshold";
}

int MTHighThreshold::Precision() const
{
   return 6;
}

double MTHighThreshold::DefaultValue() const
{
   return 0;
}

double MTHighThreshold::MinimumValue() const
{
   return 0;
}

double MTHighThreshold::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MTNumberOfIterations::MTNumberOfIterations( MetaProcess* P ) : MetaUInt32( P )
{
   TheMTNumberOfIterationsParameter = this;
}

IsoString MTNumberOfIterations::Id() const
{
   return "numberOfIterations";
}

double MTNumberOfIterations::DefaultValue() const
{
   return 1;
}

double MTNumberOfIterations::MinimumValue() const
{
   return 1;
}

double MTNumberOfIterations::MaximumValue() const
{
   return 10;
}

// ----------------------------------------------------------------------------

MTAmount::MTAmount( MetaProcess* P ) : MetaFloat( P )
{
   TheMTAmountParameter = this;
}

IsoString MTAmount::Id() const
{
   return "amount";
}

int MTAmount::Precision() const
{
   return 2;
}

double MTAmount::DefaultValue() const
{
   return 1;
}

double MTAmount::MinimumValue() const
{
   return 0;
}

double MTAmount::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MTSelectionPoint::MTSelectionPoint( MetaProcess* P ) : MetaFloat( P )
{
   TheMTSelectionPointParameter = this;
}

IsoString MTSelectionPoint::Id() const
{
   return "selectionPoint";
}

int MTSelectionPoint::Precision() const
{
   return 2;
}

double MTSelectionPoint::DefaultValue() const
{
   return 0.5;
}

double MTSelectionPoint::MinimumValue() const
{
   return 0;
}

double MTSelectionPoint::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MTStructureName::MTStructureName( MetaProcess* P ) : MetaString( P )
{
   TheMTStructureNameParameter = this;
}

IsoString MTStructureName::Id() const
{
   return "structureName";
}

// ----------------------------------------------------------------------------

MTStructureSize::MTStructureSize( MetaProcess* P ) : MetaUInt32( P )
{
   TheMTStructureSizeParameter = this;
}

IsoString MTStructureSize::Id() const
{
   return "structureSize";
}

double MTStructureSize::DefaultValue() const
{
   return 3;
}

double MTStructureSize::MinimumValue() const
{
   return 3;
}

double MTStructureSize::MaximumValue() const
{
   return 25;
}

// ----------------------------------------------------------------------------

MTStructureWayTable::MTStructureWayTable( MetaProcess* P ) : MetaTable( P )
{
   TheMTStructureWayTableParameter = this;
}

IsoString MTStructureWayTable::Id() const
{
   return "structureWayTable";
}

size_type MTStructureWayTable::MinLength() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MTStructureWayMask::MTStructureWayMask( MetaTable* T ) : MetaBlock( T )
{
   TheMTStructureWayMaskParameter = this;
}

IsoString MTStructureWayMask::Id() const
{
   return "mask";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
