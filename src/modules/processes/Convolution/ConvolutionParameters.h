//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ConvolutionParameters_h
#define __ConvolutionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ConMode : public MetaEnumeration
{
public:

   enum { Parametric,
          Library,
          Image,
          NumberOfModes,
          Default = Parametric };

   ConMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
   IsoString ElementAliases() const override;
};

extern ConMode* TheConModeParameter;

// ----------------------------------------------------------------------------

class ConSigma : public MetaFloat
{
public:

   ConSigma( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern ConSigma* TheConSigmaParameter;

// ----------------------------------------------------------------------------

class ConShape : public MetaFloat
{
public:

   ConShape( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern ConShape* TheConShapeParameter;

// ----------------------------------------------------------------------------

class ConAspectRatio : public MetaFloat
{
public:

   ConAspectRatio( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern ConAspectRatio* TheConAspectRatioParameter;

// ----------------------------------------------------------------------------

class ConRotationAngle : public MetaFloat
{
public:

   ConRotationAngle( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern ConRotationAngle* TheConRotationAngleParameter;

// ----------------------------------------------------------------------------

class ConSourceCode : public MetaString
{
public:

   ConSourceCode( MetaProcess* );

   IsoString Id() const override;
};

extern ConSourceCode* TheConSourceCodeParameter;

// ----------------------------------------------------------------------------

class ConRescaleHighPass : public MetaBoolean
{
public:

   ConRescaleHighPass( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ConRescaleHighPass* TheConRescaleHighPassParameter;

// ----------------------------------------------------------------------------

class ConViewId : public MetaString
{
public:

   ConViewId( MetaProcess* );

   IsoString Id() const override;
};

extern ConViewId* TheConViewIdParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ConvolutionParameters_h

// ----------------------------------------------------------------------------
// EOF ConvolutionParameters.h - Released 2025-04-07T08:53:55Z
