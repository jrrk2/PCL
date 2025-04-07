//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExponentialTransformationParameters_h
#define __ExponentialTransformationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ExponentialFunctionType : public MetaEnumeration
{
public:

   enum { PIP, SMI, NumberOfExponentialFunctions, Default = PIP };

   ExponentialFunctionType( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ExponentialFunctionType*   TheExponentialFunctionTypeParameter;

// ----------------------------------------------------------------------------

class ExponentialFunctionOrder : public MetaFloat
{
public:

   ExponentialFunctionOrder( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ExponentialFunctionOrder* TheExponentialFunctionOrderParameter;

// ----------------------------------------------------------------------------

class ExponentialFunctionSmoothing : public MetaFloat // sigma
{
public:

   ExponentialFunctionSmoothing( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ExponentialFunctionSmoothing* TheExponentialFunctionSmoothingParameter;

// ----------------------------------------------------------------------------

class ExponentialFunctionMask : public MetaBoolean
{
public:

   ExponentialFunctionMask( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern ExponentialFunctionMask* TheExponentialFunctionMaskParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ExponentialTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationParameters.h - Released 2025-04-07T08:53:56Z
