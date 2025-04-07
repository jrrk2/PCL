//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// SampleFormatConversionParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SampleFormatConversionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversion* TheSampleFormatConversionParameter = 0;

// ----------------------------------------------------------------------------

SampleFormatConversion::SampleFormatConversion( MetaProcess* p ) : MetaEnumeration( p )
{
   TheSampleFormatConversionParameter = this;
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversion::Id() const
{
   return "format";
}

// ----------------------------------------------------------------------------

size_type SampleFormatConversion::NumberOfElements() const
{
   return 5;
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversion::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case To8Bit:   return "To8Bit";
   case To16Bit:  return "To16Bit";
   case To32Bit:  return "To32Bit";
   case ToFloat:  return "ToFloat";
   case ToDouble: return "ToDouble";
   }
}

// ----------------------------------------------------------------------------

int SampleFormatConversion::ElementValue( size_type i ) const
{
   return int( i );
}

// ----------------------------------------------------------------------------

size_type SampleFormatConversion::DefaultValueIndex() const
{
   return ToFloat;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionParameters.cpp - Released 2025-04-07T08:53:56Z
