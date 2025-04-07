//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelParameters_h
#define __ChannelParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ColorSpaceId : public MetaEnumeration
{
public:

   enum { RGB, CIEXYZ, CIELab, CIELch, HSV, HSI, NumberOfColorSpaces, Default = RGB };

   ColorSpaceId( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;

   static IsoString SpaceId( int cs );
   static IsoString ChannelId( int cs, int c );
};

// ----------------------------------------------------------------------------

class ChannelTable : public MetaTable
{
public:

   ChannelTable( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

// ----------------------------------------------------------------------------

class ChannelEnabled : public MetaBoolean
{
public:

   ChannelEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

class ChannelId : public MetaString
{
public:

   ChannelId( MetaTable* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

// ----------------------------------------------------------------------------

class ChannelSampleFormat : public MetaEnumeration
{
public:

   enum { SameAsSource,
          I8,     // unsigned 8-bit integer
          I16,    // unsigned 16-bit integer
          I32,    // unsigned 32-bit integer
          F32,    // IEEE 32-bit floating point (simple precision)
          F64,    // IEEE 64-bit floating point (double precision)
          NumberOfSampleFormats,
          Default = SameAsSource };

   ChannelSampleFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

// ----------------------------------------------------------------------------

class CSInheritAstrometricSolution : public MetaBoolean
{
public:

   CSInheritAstrometricSolution( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelParameters_h

// ----------------------------------------------------------------------------
// EOF ChannelParameters.h - Released 2025-04-07T08:53:55Z
