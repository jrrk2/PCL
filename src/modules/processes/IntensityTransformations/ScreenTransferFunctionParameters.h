//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ScreenTransferFunctionParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ScreenTransferFunctionParameters_h
#define __ScreenTransferFunctionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------
// STFSet parameter.
//
// This is a table parameter including four rows. Each table row provides a STF
// corresponding to a display channel, namely:
//
//    row   channel
//    ===   =========
//     0    Red/Gray
//     1    Green
//     2    Blue
//     3    CIE L* and CIE Y
// ----------------------------------------------------------------------------

class STFSet : public MetaTable
{
public:

   STFSet( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

extern STFSet* TheSTFSetParameter;

// ----------------------------------------------------------------------------

class STFShadowsClipping : public MetaFloat
{
public:

   STFShadowsClipping( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern STFShadowsClipping* TheSTFShadowsClippingParameter;

// ----------------------------------------------------------------------------

class STFHighlightsClipping : public MetaFloat
{
public:

   STFHighlightsClipping( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern STFHighlightsClipping* TheSTFHighlightsClippingParameter;

// ----------------------------------------------------------------------------

class STFMidtonesBalance : public MetaFloat
{
public:

   STFMidtonesBalance( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern STFMidtonesBalance* TheSTFMidtonesBalanceParameter;

// ----------------------------------------------------------------------------

class STFLowRange : public MetaFloat
{
public:

   STFLowRange( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern STFLowRange* TheSTFLowRangeParameter;

// ----------------------------------------------------------------------------

class STFHighRange : public MetaFloat
{
public:

   STFHighRange( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern STFHighRange* TheSTFHighRangeParameter;

// ----------------------------------------------------------------------------

class STFInteraction : public MetaEnumeration
{
public:

   enum { SeparateChannels,
          Grayscale,
          NumberOfInteractionModes,
          Default = SeparateChannels };

   STFInteraction( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern STFInteraction* TheSTFInteractionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ScreenTransferFunctionParameters_h

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionParameters.h - Released 2025-04-07T08:53:56Z
