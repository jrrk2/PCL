//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// LRGBCombinationParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LRGBCombinationParameters_h
#define __LRGBCombinationParameters_h

#include "ChannelParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LRGBChannelTable : public MetaTable
{
public:

   LRGBChannelTable( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

extern LRGBChannelTable*   TheLRGBChannelTableParameter;

// ----------------------------------------------------------------------------

class LRGBChannelEnabled : public ChannelEnabled
{
public:

   LRGBChannelEnabled( MetaTable* );
};

extern LRGBChannelEnabled* TheLRGBChannelEnabledParameter;

// ----------------------------------------------------------------------------

class LRGBChannelId : public ChannelId
{
public:

   LRGBChannelId( MetaTable* );
};

extern LRGBChannelId*      TheLRGBChannelIdParameter;

// ----------------------------------------------------------------------------

class LRGBChannelWeight : public MetaDouble
{
public:

   LRGBChannelWeight( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBChannelWeight*  TheLRGBChannelWeightParameter;

// ----------------------------------------------------------------------------

class LRGBLuminanceMTF : public MetaFloat
{
public:

   LRGBLuminanceMTF( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBLuminanceMTF*   TheLRGBLuminanceMTFParameter;

// ----------------------------------------------------------------------------

class LRGBSaturationMTF : public MetaFloat
{
public:

   LRGBSaturationMTF( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBSaturationMTF*  TheLRGBSaturationMTFParameter;

// ----------------------------------------------------------------------------

class LRGBClipHighlights : public MetaBoolean
{
public:

   LRGBClipHighlights( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LRGBClipHighlights* TheLRGBClipHighlightsParameter;

// ----------------------------------------------------------------------------

class LRGBNoiseReduction : public MetaBoolean
{
public:

   LRGBNoiseReduction( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LRGBNoiseReduction* TheLRGBNoiseReductionParameter;

// ----------------------------------------------------------------------------

class LRGBLayersRemoved : public MetaUInt32
{
public:

   LRGBLayersRemoved( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBLayersRemoved* TheLRGBLayersRemovedParameter;

// ----------------------------------------------------------------------------

class LRGBLayersProtected : public MetaUInt32
{
public:

   LRGBLayersProtected( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LRGBLayersProtected* TheLRGBLayersProtectedParameter;

// ----------------------------------------------------------------------------

extern CSInheritAstrometricSolution* TheLRGBCInheritAstrometricSolutionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __LRGBCombinationParameters_h

// ----------------------------------------------------------------------------
// EOF LRGBCombinationParameters.h - Released 2025-04-07T08:53:55Z
