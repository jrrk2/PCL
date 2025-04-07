//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ScreenTransferFunctionParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ScreenTransferFunctionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

STFSet*                 TheSTFSetParameter = 0;
STFShadowsClipping*     TheSTFShadowsClippingParameter = 0;
STFHighlightsClipping*  TheSTFHighlightsClippingParameter = 0;
STFMidtonesBalance*     TheSTFMidtonesBalanceParameter = 0;
STFLowRange*            TheSTFLowRangeParameter = 0;
STFHighRange*           TheSTFHighRangeParameter = 0;
STFInteraction*         TheSTFInteractionParameter = 0;

// ----------------------------------------------------------------------------

STFSet::STFSet( MetaProcess* p ) : MetaTable( p )
{
   TheSTFSetParameter = this;
}

IsoString STFSet::Id() const
{
   return "STF";
}

size_type STFSet::MinLength() const
{
   return 4;
}

size_type STFSet::MaxLength() const
{
   return 4;
}

// ----------------------------------------------------------------------------

STFShadowsClipping::STFShadowsClipping( MetaTable* T ) : MetaFloat( T )
{
   TheSTFShadowsClippingParameter = this;
}

IsoString STFShadowsClipping::Id() const
{
   return "c0";
}

int STFShadowsClipping::Precision() const
{
   return 5;
}

double STFShadowsClipping::DefaultValue() const
{
   return 0.0;
}

double STFShadowsClipping::MinimumValue() const
{
   return 0.0;
}

double STFShadowsClipping::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

STFHighlightsClipping::STFHighlightsClipping( MetaTable* T ) : MetaFloat( T )
{
   TheSTFHighlightsClippingParameter = this;
}

IsoString STFHighlightsClipping::Id() const
{
   return "c1";
}

int STFHighlightsClipping::Precision() const
{
   return 5;
}

double STFHighlightsClipping::DefaultValue() const
{
   return 0.0;
}

double STFHighlightsClipping::MinimumValue() const
{
   return 0.0;
}

double STFHighlightsClipping::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

STFMidtonesBalance::STFMidtonesBalance( MetaTable* T ) : MetaFloat( T )
{
   TheSTFMidtonesBalanceParameter = this;
}

IsoString STFMidtonesBalance::Id() const
{
   return "m";
}

int STFMidtonesBalance::Precision() const
{
   return 5;
}

double STFMidtonesBalance::DefaultValue() const
{
   return 0.5;
}

double STFMidtonesBalance::MinimumValue() const
{
   return 0.0;
}

double STFMidtonesBalance::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

STFLowRange::STFLowRange( MetaTable* T ) : MetaFloat( T )
{
   TheSTFLowRangeParameter = this;
}

IsoString STFLowRange::Id() const
{
   return "r0";
}

int STFLowRange::Precision() const
{
   return 5;
}

double STFLowRange::DefaultValue() const
{
   return 0.0;
}

double STFLowRange::MinimumValue() const
{
   return -10.0;
}

double STFLowRange::MaximumValue() const
{
   return 0.0;
}

// ----------------------------------------------------------------------------

STFHighRange::STFHighRange( MetaTable* T ) : MetaFloat( T )
{
   TheSTFHighRangeParameter = this;
}

IsoString STFHighRange::Id() const
{
   return "r1";
}

int STFHighRange::Precision() const
{
   return 5;
}

double STFHighRange::DefaultValue() const
{
   return 1.0;
}

double STFHighRange::MinimumValue() const
{
   return 1.0;
}

double STFHighRange::MaximumValue() const
{
   return +11.0;
}

// ----------------------------------------------------------------------------

STFInteraction::STFInteraction( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSTFInteractionParameter = this;
}

IsoString STFInteraction::Id() const
{
   return "interaction";
}

size_type STFInteraction::NumberOfElements() const
{
   return NumberOfInteractionModes;
}

IsoString STFInteraction::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case SeparateChannels: return "SeparateChannels";
   case Grayscale:        return "Grayscale";
   }
}

int STFInteraction::ElementValue( size_type i ) const
{
   return int( i );
}

size_type STFInteraction::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionParameters.cpp - Released 2025-04-07T08:53:56Z
