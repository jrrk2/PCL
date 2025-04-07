//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// HistogramTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "HistogramTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramTransformationTable* TheHistogramTransformationTableParameter = 0;
ShadowsClipping*              TheShadowsClippingParameter = 0;
HighlightsClipping*           TheHighlightsClippingParameter = 0;
MidtonesBalance*              TheMidtonesBalanceParameter = 0;
LowRange*                     TheLowRangeParameter = 0;
HighRange*                    TheHighRangeParameter = 0;

// ----------------------------------------------------------------------------

HistogramTransformationTable::HistogramTransformationTable( MetaProcess* P ) : MetaTable( P )
{
   TheHistogramTransformationTableParameter = this;
}

IsoString HistogramTransformationTable::Id() const
{
   return "H";
}

size_type HistogramTransformationTable::MinLength() const
{
   return 4; // For compatibility with old (vintage?) versions: allow no alpha channel
}

size_type HistogramTransformationTable::MaxLength() const
{
   return 5;
}

// ----------------------------------------------------------------------------

ShadowsClipping::ShadowsClipping( MetaTable* T ) : MetaDouble( T )
{
   TheShadowsClippingParameter = this;
}

IsoString ShadowsClipping::Id() const
{
   return "c0";
}

int ShadowsClipping::Precision() const
{
   return 8;
}

double ShadowsClipping::DefaultValue() const
{
   return 0;
}

double ShadowsClipping::MinimumValue() const
{
   return 0;
}

double ShadowsClipping::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

HighlightsClipping::HighlightsClipping( MetaTable* T ) : MetaDouble( T )
{
   TheHighlightsClippingParameter = this;
}

IsoString HighlightsClipping::Id() const
{
   return "c1";
}

int HighlightsClipping::Precision() const
{
   return 8;
}

double HighlightsClipping::DefaultValue() const
{
   return 1;
}

double HighlightsClipping::MinimumValue() const
{
   return 0;
}

double HighlightsClipping::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

MidtonesBalance::MidtonesBalance( MetaTable* T ) : MetaDouble( T )
{
   TheMidtonesBalanceParameter = this;
}

IsoString MidtonesBalance::Id() const
{
   return "m";
}

int MidtonesBalance::Precision() const
{
   return 8;
}

double MidtonesBalance::DefaultValue() const
{
   return 0.5;
}

double MidtonesBalance::MinimumValue() const
{
   return 0;
}

double MidtonesBalance::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LowRange::LowRange( MetaTable* T ) : MetaDouble( T )
{
   TheLowRangeParameter = this;
}

IsoString LowRange::Id() const
{
   return "r0";
}

int LowRange::Precision() const
{
   return 8;
}

double LowRange::DefaultValue() const
{
   return 0;
}

double LowRange::MinimumValue() const
{
   return -10;
}

double LowRange::MaximumValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

HighRange::HighRange( MetaTable* T ) : MetaDouble( T )
{
   TheHighRangeParameter = this;
}

IsoString HighRange::Id() const
{
   return "r1";
}

int HighRange::Precision() const
{
   return 8;
}

double HighRange::DefaultValue() const
{
   return 1;
}

double HighRange::MinimumValue() const
{
   return 1;
}

double HighRange::MaximumValue() const
{
   return +11;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
