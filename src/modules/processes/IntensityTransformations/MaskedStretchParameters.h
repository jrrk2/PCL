//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// MaskedStretchParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MaskedStretchParameters_h
#define __MaskedStretchParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class MSTargetBackground : public MetaDouble
{
public:

   MSTargetBackground( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSTargetBackground* TheMSTargetBackgroundParameter;

// ----------------------------------------------------------------------------

class MSNumberOfIterations : public MetaInt32
{
public:

   MSNumberOfIterations( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSNumberOfIterations* TheMSNumberOfIterationsParameter;

// ----------------------------------------------------------------------------

class MSClippingFraction : public MetaDouble
{
public:

   MSClippingFraction( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSClippingFraction* TheMSClippingFractionParameter;

// ----------------------------------------------------------------------------

class MSBackgroundReferenceViewId : public MetaString
{
public:

   MSBackgroundReferenceViewId( MetaProcess* );

   IsoString Id() const override;
};

extern MSBackgroundReferenceViewId* TheMSBackgroundReferenceViewIdParameter;

// ----------------------------------------------------------------------------

class MSBackgroundLow : public MetaDouble
{
public:

   MSBackgroundLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSBackgroundLow* TheMSBackgroundLowParameter;

// ----------------------------------------------------------------------------

class MSBackgroundHigh : public MetaDouble
{
public:

   MSBackgroundHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern MSBackgroundHigh* TheMSBackgroundHighParameter;

// ----------------------------------------------------------------------------

class MSUseROI : public MetaBoolean
{
public:

   MSUseROI( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern MSUseROI* TheMSUseROIParameter;

// ----------------------------------------------------------------------------

class MSROIX0 : public MetaInt32
{
public:

   MSROIX0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIX0* TheMSROIX0Parameter;

// ----------------------------------------------------------------------------

class MSROIY0 : public MetaInt32
{
public:

   MSROIY0( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIY0* TheMSROIY0Parameter;

// ----------------------------------------------------------------------------

class MSROIX1 : public MetaInt32
{
public:

   MSROIX1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIX1* TheMSROIX1Parameter;

// ----------------------------------------------------------------------------

class MSROIY1 : public MetaInt32
{
public:

   MSROIY1( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern MSROIY1* TheMSROIY1Parameter;

// ----------------------------------------------------------------------------

class MSMaskType : public MetaEnumeration
{
public:

   enum { Intensity,
          Value,
          NumberOfItems,
          Default = Intensity };

   MSMaskType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern MSMaskType* TheMSMaskTypeParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __MaskedStretchParameters_h

// ----------------------------------------------------------------------------
// EOF MaskedStretchParameters.h - Released 2025-04-07T08:53:56Z
