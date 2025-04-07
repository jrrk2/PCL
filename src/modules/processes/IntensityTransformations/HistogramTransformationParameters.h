//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// HistogramTransformationParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __HistogramTransformationParameters_h
#define __HistogramTransformationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class HistogramTransformationTable : public MetaTable
{
public:

   HistogramTransformationTable( MetaProcess* P );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

extern HistogramTransformationTable* TheHistogramTransformationTableParameter;

// ----------------------------------------------------------------------------

class ShadowsClipping : public MetaDouble
{
public:

   ShadowsClipping( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ShadowsClipping* TheShadowsClippingParameter;

// ----------------------------------------------------------------------------

class HighlightsClipping : public MetaDouble
{
public:

   HighlightsClipping( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern HighlightsClipping* TheHighlightsClippingParameter;

// ----------------------------------------------------------------------------

class MidtonesBalance : public MetaDouble
{
public:

   MidtonesBalance( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MidtonesBalance* TheMidtonesBalanceParameter;

// ----------------------------------------------------------------------------

class LowRange : public MetaDouble
{
public:

   LowRange( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LowRange* TheLowRangeParameter;

// ----------------------------------------------------------------------------

class HighRange : public MetaDouble
{
public:

   HighRange( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern HighRange* TheHighRangeParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __HistogramTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF HistogramTransformationParameters.h - Released 2025-04-07T08:53:56Z
