//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SandboxParameters_h
#define __SandboxParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SandboxParameterOne : public MetaFloat
{
public:

   SandboxParameterOne( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SandboxParameterOne* TheSandboxParameterOneParameter;

// ----------------------------------------------------------------------------

class SandboxParameterTwo : public MetaInt32
{
public:

   SandboxParameterTwo( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern SandboxParameterTwo* TheSandboxParameterTwoParameter;

// ----------------------------------------------------------------------------

class SandboxParameterThree : public MetaBoolean
{
public:

   SandboxParameterThree( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern SandboxParameterThree* TheSandboxParameterThreeParameter;

// ----------------------------------------------------------------------------

class SandboxParameterFour : public MetaEnumeration
{
public:

   enum { FirstItem,
          SecondItem,
          ThirdItem,
          NumberOfItems,
          Default = FirstItem };

   SandboxParameterFour( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern SandboxParameterFour* TheSandboxParameterFourParameter;

// ----------------------------------------------------------------------------

class SandboxParameterFive : public MetaString
{
public:

   SandboxParameterFive( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
};

extern SandboxParameterFive* TheSandboxParameterFiveParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SandboxParameters_h

// ----------------------------------------------------------------------------
// EOF SandboxParameters.h - Released 2025-04-07T08:53:56Z
