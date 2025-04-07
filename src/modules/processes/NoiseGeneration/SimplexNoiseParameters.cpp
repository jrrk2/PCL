//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SimplexNoiseParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SimplexNoiseParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SNAmount*   TheSNAmountParameter = nullptr;
SNScale*    TheSNScaleParameter = nullptr;
SNOffsetX*  TheSNOffsetXParameter = nullptr;
SNOffsetY*  TheSNOffsetYParameter = nullptr;
SNOperator* TheSNOperatorParameter = nullptr;

// ----------------------------------------------------------------------------

SNAmount::SNAmount( MetaProcess* p ) : MetaFloat( p )
{
   TheSNAmountParameter = this;
}

IsoString SNAmount::Id() const
{
   return "amount";
}

int SNAmount::Precision() const
{
   return 2;
}

double SNAmount::MinimumValue() const
{
   return 0;
}

double SNAmount::MaximumValue() const
{
   return 1;
}

double SNAmount::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SNScale::SNScale( MetaProcess* p ) : MetaInt32( p )
{
   TheSNScaleParameter = this;
}

IsoString SNScale::Id() const
{
   return "scale";
}

double SNScale::MinimumValue() const
{
   return 1;
}

double SNScale::MaximumValue() const
{
   return 65536;
}

double SNScale::DefaultValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SNOffsetX::SNOffsetX( MetaProcess* p ) : MetaInt32( p )
{
   TheSNOffsetXParameter = this;
}

IsoString SNOffsetX::Id() const
{
   return "offsetX";
}

double SNOffsetX::MinimumValue() const
{
   return -32768;
}

double SNOffsetX::MaximumValue() const
{
   return +32767;
}

double SNOffsetX::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

SNOffsetY::SNOffsetY( MetaProcess* p ) : MetaInt32( p )
{
   TheSNOffsetYParameter = this;
}

IsoString SNOffsetY::Id() const
{
   return "offsetY";
}

double SNOffsetY::MinimumValue() const
{
   return -32768;
}

double SNOffsetY::MaximumValue() const
{
   return +32767;
}

double SNOffsetY::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

SNOperator::SNOperator( MetaProcess* p ) : MetaEnumeration( p )
{
   TheSNOperatorParameter = this;
}

IsoString SNOperator::Id() const
{
   return "operator";
}

size_type SNOperator::NumberOfElements() const
{
   return NumberOfOperators;
}

IsoString SNOperator::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Copy:   return "Copy";
   case Add:    return "Add";
   case Sub:    return "Sub";
   case Mul:    return "Mul";
   case Div:    return "Div";
   case Pow:    return "Pow";
   case Dif:    return "Dif";
   case Screen: return "Screen";
   case Or:     return "Or";
   case And:    return "And";
   case Xor:    return "Xor";
   case Nor:    return "Nor";
   case Nand:   return "Nand";
   case Xnor:   return "Xnor";
   }
}

int SNOperator::ElementValue( size_type i ) const
{
   return int( i );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SimplexNoiseParameters.cpp - Released 2025-04-07T08:53:56Z
