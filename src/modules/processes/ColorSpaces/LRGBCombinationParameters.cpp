//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// LRGBCombinationParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "LRGBCombinationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LRGBChannelTable*             TheLRGBChannelTableParameter = nullptr;
LRGBChannelEnabled*           TheLRGBChannelEnabledParameter = nullptr;
LRGBChannelId*                TheLRGBChannelIdParameter = nullptr;
LRGBChannelWeight*            TheLRGBChannelWeightParameter = nullptr;
LRGBLuminanceMTF*             TheLRGBLuminanceMTFParameter = nullptr;
LRGBSaturationMTF*            TheLRGBSaturationMTFParameter = nullptr;
LRGBClipHighlights*           TheLRGBClipHighlightsParameter = nullptr;
LRGBNoiseReduction*           TheLRGBNoiseReductionParameter = nullptr;
LRGBLayersRemoved*            TheLRGBLayersRemovedParameter = nullptr;
LRGBLayersProtected*          TheLRGBLayersProtectedParameter = nullptr;
CSInheritAstrometricSolution* TheLRGBCInheritAstrometricSolutionParameter = nullptr;

// ----------------------------------------------------------------------------

LRGBChannelTable::LRGBChannelTable( MetaProcess* P ) : MetaTable( P )
{
   TheLRGBChannelTableParameter = this;
}

IsoString LRGBChannelTable::Id() const
{
   return "channels";
}

size_type LRGBChannelTable::MinLength() const
{
   return 4;
}

size_type LRGBChannelTable::MaxLength() const
{
   return 4;
}

// ----------------------------------------------------------------------------

LRGBChannelEnabled::LRGBChannelEnabled( MetaTable* T ) : ChannelEnabled( T )
{
   TheLRGBChannelEnabledParameter = this;
}

// ----------------------------------------------------------------------------

LRGBChannelId::LRGBChannelId( MetaTable* T ) : ChannelId( T )
{
   TheLRGBChannelIdParameter = this;
}

// ----------------------------------------------------------------------------

LRGBChannelWeight::LRGBChannelWeight( MetaTable* T ) : MetaDouble( T )
{
   TheLRGBChannelWeightParameter = this;
}

IsoString LRGBChannelWeight::Id() const
{
   return "k";
}

int LRGBChannelWeight::Precision() const
{
   return 5;
}

double LRGBChannelWeight::MinimumValue() const
{
   return 0;
}

double LRGBChannelWeight::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LRGBLuminanceMTF::LRGBLuminanceMTF( MetaProcess* P ) : MetaFloat( P )
{
   TheLRGBLuminanceMTFParameter = this;
}

IsoString LRGBLuminanceMTF::Id() const
{
   return "mL";
}

int LRGBLuminanceMTF::Precision() const
{
   return 3;
}

double LRGBLuminanceMTF::DefaultValue() const
{
   return 0.5;
}

double LRGBLuminanceMTF::MinimumValue() const
{
   return 0.001;
}

double LRGBLuminanceMTF::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

LRGBSaturationMTF::LRGBSaturationMTF( MetaProcess* P ) : MetaFloat( P )
{
   TheLRGBSaturationMTFParameter = this;
}

IsoString LRGBSaturationMTF::Id() const
{
   return "mc";
}

int LRGBSaturationMTF::Precision() const
{
   return 3;
}

double LRGBSaturationMTF::DefaultValue() const
{
   return 0.5;
}

double LRGBSaturationMTF::MinimumValue() const
{
   return 0.001;
}

double LRGBSaturationMTF::MaximumValue() const
{
   return 1.0;
}

// ----------------------------------------------------------------------------

LRGBClipHighlights::LRGBClipHighlights( MetaProcess* P ) : MetaBoolean( P )
{
   TheLRGBClipHighlightsParameter = this;
}

IsoString LRGBClipHighlights::Id() const
{
   return "clipHighlights";
}

bool LRGBClipHighlights::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

LRGBNoiseReduction::LRGBNoiseReduction( MetaProcess* P ) : MetaBoolean( P )
{
   TheLRGBNoiseReductionParameter = this;
}

IsoString LRGBNoiseReduction::Id() const
{
   return "noiseReduction";
}

bool LRGBNoiseReduction::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

LRGBLayersRemoved::LRGBLayersRemoved( MetaProcess* P ) : MetaUInt32( P )
{
   TheLRGBLayersRemovedParameter = this;
}

IsoString LRGBLayersRemoved::Id() const
{
   return "layersRemoved";
}

double LRGBLayersRemoved::DefaultValue() const
{
   return 4;
}

double LRGBLayersRemoved::MinimumValue() const
{
   return 1;
}

double LRGBLayersRemoved::MaximumValue() const
{
   return 6;
}

// ----------------------------------------------------------------------------

LRGBLayersProtected::LRGBLayersProtected( MetaProcess* P ) : MetaUInt32( P )
{
   TheLRGBLayersProtectedParameter = this;
}

IsoString LRGBLayersProtected::Id() const
{
   return "layersProtected";
}

double LRGBLayersProtected::DefaultValue() const
{
   return 2;
}

double LRGBLayersProtected::MinimumValue() const
{
   return 0;
}

double LRGBLayersProtected::MaximumValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LRGBCombinationParameters.cpp - Released 2025-04-07T08:53:55Z
