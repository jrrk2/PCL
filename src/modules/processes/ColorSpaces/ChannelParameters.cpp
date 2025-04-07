//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSpaceId::ColorSpaceId( MetaProcess* P )
   : MetaEnumeration( P )
{
}

IsoString ColorSpaceId::Id() const
{
   return "colorSpace";
}

size_type ColorSpaceId::NumberOfElements() const
{
   return NumberOfColorSpaces;
}

IsoString ColorSpaceId::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RGB:      return "RGB";
   case CIEXYZ:   return "CIEXYZ";
   case CIELab:   return "CIELab";
   case CIELch:   return "CIELch";
   case HSV:      return "HSV";
   case HSI:      return "HSI";
   }
}

int ColorSpaceId::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ColorSpaceId::DefaultValueIndex() const
{
   return Default;
}

IsoString ColorSpaceId::SpaceId( int cs )
{
   switch ( cs )
   {
   default:
   case RGB:      return "RGB";
   case CIEXYZ:   return "CIE XYZ";
   case CIELab:   return "CIE L*a*b*";
   case CIELch:   return "CIE L*c*h*";
   case HSV:      return "HSV";
   case HSI:      return "HSI";
   }
}

IsoString ColorSpaceId::ChannelId( int cs, int c )
{
   switch ( cs )
   {
   default:
   case RGB:      return (c == 0) ? "R"  : ((c == 1) ? "G"  : "B");
   case CIEXYZ:   return (c == 0) ? "X"  : ((c == 1) ? "Y"  : "Z");
   case CIELab:   return (c == 0) ? "L"  : ((c == 1) ? "a"  : "b");
   case CIELch:   return (c == 0) ? "L"  : ((c == 1) ? "c"  : "h");
   case HSV:      return (c == 0) ? "H"  : ((c == 1) ? "Sv" : "V");
   case HSI:      return (c == 0) ? "H"  : ((c == 1) ? "Si" : "I");
   }
}

// ----------------------------------------------------------------------------

ChannelTable::ChannelTable( MetaProcess* P )
   : MetaTable( P )
{
}

IsoString ChannelTable::Id() const
{
   return "channels";
}

size_type ChannelTable::MinLength() const
{
   return 3;
}

size_type ChannelTable::MaxLength() const
{
   return 3;
}

// ----------------------------------------------------------------------------

ChannelEnabled::ChannelEnabled( MetaTable* T )
   : MetaBoolean( T )
{
}

IsoString ChannelEnabled::Id() const
{
   return "enabled";
}

bool ChannelEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ChannelId::ChannelId( MetaTable* T )
   : MetaString( T )
{
}

IsoString ChannelId::Id() const
{
   return "id";
}

size_type ChannelId::MinLength() const
{
   return 0; // can be void
}

String ChannelId::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

ChannelSampleFormat::ChannelSampleFormat( MetaProcess* p )
   : MetaEnumeration( p )
{
}

IsoString ChannelSampleFormat::Id() const
{
   return "sampleFormat";
}

size_type ChannelSampleFormat::NumberOfElements() const
{
   return NumberOfSampleFormats;
}

IsoString ChannelSampleFormat::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case SameAsSource: return "SameAsSource";
   case I8:           return "i8";
   case I16:          return "i16";
   case I32:          return "i32";
   case F32:          return "f32";
   case F64:          return "f64";
   }
}

int ChannelSampleFormat::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ChannelSampleFormat::DefaultValueIndex() const
{
   return size_type( Default );
}

// ----------------------------------------------------------------------------

CSInheritAstrometricSolution::CSInheritAstrometricSolution( MetaProcess* P )
   : MetaBoolean( P )
{
}

IsoString CSInheritAstrometricSolution::Id() const
{
   return "inheritAstrometricSolution";
}

bool CSInheritAstrometricSolution::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelParameters.cpp - Released 2025-04-07T08:53:55Z
