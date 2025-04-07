//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AutoHistogramParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AutoHistogramParameters_h
#define __AutoHistogramParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class AHClip : public MetaBoolean
{
public:

   AHClip( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHClip* TheAHClipParameter;

// ----------------------------------------------------------------------------

class AHClipTogether : public MetaBoolean
{
public:

   AHClipTogether( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHClipTogether* TheAHClipTogetherParameter;

// ----------------------------------------------------------------------------

class AHClipLowR : public MetaFloat
{
public:

   AHClipLowR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipLowR* TheAHClipLowRParameter;

// ----------------------------------------------------------------------------

class AHClipLowG : public MetaFloat
{
public:

   AHClipLowG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipLowG* TheAHClipLowGParameter;

// ----------------------------------------------------------------------------

class AHClipLowB : public MetaFloat
{
public:

   AHClipLowB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipLowB* TheAHClipLowBParameter;

// ----------------------------------------------------------------------------

class AHClipHighR : public MetaFloat
{
public:

   AHClipHighR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

// ----------------------------------------------------------------------------

extern AHClipHighR* TheAHClipHighRParameter;

class AHClipHighG : public MetaFloat
{
public:

   AHClipHighG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipHighG* TheAHClipHighGParameter;

// ----------------------------------------------------------------------------

class AHClipHighB : public MetaFloat
{
public:

   AHClipHighB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHClipHighB* TheAHClipHighBParameter;

// ----------------------------------------------------------------------------

class AHStretch : public MetaBoolean
{
public:

   AHStretch( MetaProcess* );
   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHStretch* TheAHStretchParameter;

// ----------------------------------------------------------------------------

class AHStretchTogether : public MetaBoolean
{
public:

   AHStretchTogether( MetaProcess* );
   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern AHStretchTogether* TheAHStretchTogetherParameter;

// ----------------------------------------------------------------------------

class AHStretchMethod : public MetaEnumeration
{
public:

   enum { Gamma,
          Logarithm,
          MTF,
          NumberOfItems,
          Default = Gamma };

   AHStretchMethod( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern AHStretchMethod* TheAHStretchMethodParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianR : public MetaDouble
{
public:

   AHTargetMedianR( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHTargetMedianR* TheAHTargetMedianRParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianG : public MetaDouble
{
public:

   AHTargetMedianG( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHTargetMedianG* TheAHTargetMedianGParameter;

// ----------------------------------------------------------------------------

class AHTargetMedianB : public MetaDouble
{
public:

   AHTargetMedianB( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern AHTargetMedianB* TheAHTargetMedianBParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __AutoHistogramParameters_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramParameters.h - Released 2025-04-07T08:53:56Z
