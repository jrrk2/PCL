//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ExponentialTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialFunctionType*      TheExponentialFunctionTypeParameter = nullptr;
ExponentialFunctionOrder*     TheExponentialFunctionOrderParameter = nullptr;
ExponentialFunctionSmoothing* TheExponentialFunctionSmoothingParameter = nullptr;
ExponentialFunctionMask*      TheExponentialFunctionMaskParameter = nullptr;

// ----------------------------------------------------------------------------

ExponentialFunctionType::ExponentialFunctionType( MetaProcess* P ) : MetaEnumeration( P )
{
   TheExponentialFunctionTypeParameter = this;
}

IsoString ExponentialFunctionType::Id() const
{
   return "functionType";
}

size_type ExponentialFunctionType::NumberOfElements() const
{
   return NumberOfExponentialFunctions;
}

IsoString ExponentialFunctionType::ElementId( size_type i ) const
{
   switch ( i )
   {
   case PIP: return "PIP";
   case SMI: return "SMI";
   }

   return IsoString();
}

int ExponentialFunctionType::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ExponentialFunctionType::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

ExponentialFunctionOrder::ExponentialFunctionOrder( MetaProcess* P ) : MetaFloat( P )
{
   TheExponentialFunctionOrderParameter = this;
}

IsoString ExponentialFunctionOrder::Id() const
{
   return "order";
}

int ExponentialFunctionOrder::Precision() const
{
   return 2;
}

double ExponentialFunctionOrder::DefaultValue() const
{
   return 1;
}

double ExponentialFunctionOrder::MinimumValue() const
{
   return 0.1;
}

double ExponentialFunctionOrder::MaximumValue() const
{
   return 6;
}

// ----------------------------------------------------------------------------

ExponentialFunctionSmoothing::ExponentialFunctionSmoothing( MetaProcess* P ) : MetaFloat( P )
{
   TheExponentialFunctionSmoothingParameter = this;
}

IsoString ExponentialFunctionSmoothing::Id() const
{
   return "sigma";
}

int ExponentialFunctionSmoothing::Precision() const
{
   return 2;
}

double ExponentialFunctionSmoothing::DefaultValue() const
{
   return 0;
}

double ExponentialFunctionSmoothing::MinimumValue() const
{
   return 0;
}

double ExponentialFunctionSmoothing::MaximumValue() const
{
   return 50;
}

// ----------------------------------------------------------------------------

ExponentialFunctionMask::ExponentialFunctionMask( MetaProcess* P ) : MetaBoolean( P )
{
   TheExponentialFunctionMaskParameter = this;
}

IsoString ExponentialFunctionMask::Id() const
{
   return "useLightnessMask";
}

IsoString ExponentialFunctionMask::Aliases() const
{
   return "luminanceMask";
}

bool ExponentialFunctionMask::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationParameters.cpp - Released 2025-04-07T08:53:56Z
