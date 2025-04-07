//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SandboxParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxParameterOne*   TheSandboxParameterOneParameter = nullptr;
SandboxParameterTwo*   TheSandboxParameterTwoParameter = nullptr;
SandboxParameterThree* TheSandboxParameterThreeParameter = nullptr;
SandboxParameterFour*  TheSandboxParameterFourParameter = nullptr;
SandboxParameterFive*  TheSandboxParameterFiveParameter = nullptr;

// ----------------------------------------------------------------------------

SandboxParameterOne::SandboxParameterOne( MetaProcess* P ) : MetaFloat( P )
{
   TheSandboxParameterOneParameter = this;
}

IsoString SandboxParameterOne::Id() const
{
   return "sampleOne";
}

int SandboxParameterOne::Precision() const
{
   return 3;
}

double SandboxParameterOne::DefaultValue() const
{
   return 0;
}

double SandboxParameterOne::MinimumValue() const
{
   return 0;
}

double SandboxParameterOne::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

SandboxParameterTwo::SandboxParameterTwo( MetaProcess* P ) : MetaInt32( P )
{
   TheSandboxParameterTwoParameter = this;
}

IsoString SandboxParameterTwo::Id() const
{
   return "sampleTwo";
}

double SandboxParameterTwo::DefaultValue() const
{
   return 1;
}

double SandboxParameterTwo::MinimumValue() const
{
   return 1;
}

double SandboxParameterTwo::MaximumValue() const
{
   return 100;
}

// ----------------------------------------------------------------------------

SandboxParameterThree::SandboxParameterThree( MetaProcess* P ) : MetaBoolean( P )
{
   TheSandboxParameterThreeParameter = this;
}

IsoString SandboxParameterThree::Id() const
{
   return "sampleThree";
}

bool SandboxParameterThree::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

SandboxParameterFour::SandboxParameterFour( MetaProcess* P ) : MetaEnumeration( P )
{
   TheSandboxParameterFourParameter = this;
}

IsoString SandboxParameterFour::Id() const
{
   return "sampleFour";
}

size_type SandboxParameterFour::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString SandboxParameterFour::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case FirstItem:  return "FirstItem";
   case SecondItem: return "SecondItem";
   case ThirdItem:  return "ThirdItem";
   }
}

int SandboxParameterFour::ElementValue( size_type i ) const
{
   return int( i );
}

size_type SandboxParameterFour::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

SandboxParameterFive::SandboxParameterFive( MetaProcess* P ) : MetaString( P )
{
   TheSandboxParameterFiveParameter = this;
}

IsoString SandboxParameterFive::Id() const
{
   return "sampleFive";
}

size_type SandboxParameterFive::MinLength() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxParameters.cpp - Released 2025-04-07T08:53:56Z
