//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExtractAlphaChannelsParameters_h
#define __ExtractAlphaChannelsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class EAChannels : public MetaEnumeration
{
public:

   enum { AllAlphaChannels,   // All alpha channels
          ActiveAlphaChannel, // Active (first) alpha channel only
          ChannelList,        // Act on channels specified through a list
          NumberOfModes,
          Default = AllAlphaChannels };

   EAChannels( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern EAChannels* TheEAChannelsParameter;

// ----------------------------------------------------------------------------

class EAChannelList : public MetaString
{
public:

   EAChannelList( MetaProcess* );

   String DefaultValue() const override;
   IsoString Id() const override;
   String AllowedCharacters() const override;
};

extern EAChannelList* TheEAChannelListParameter;

// ----------------------------------------------------------------------------

class EAExtract : public MetaBoolean
{
public:

   EAExtract( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EAExtract* TheEAExtractParameter;

// ----------------------------------------------------------------------------

class EADelete : public MetaBoolean
{
public:

   EADelete( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern EADelete* TheEADeleteParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ExtractAlphaChannelsParameters_h

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsParameters.h - Released 2025-04-07T08:53:56Z
