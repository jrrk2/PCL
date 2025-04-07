//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ExtractAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

EAChannels*    TheEAChannelsParameter = 0;
EAChannelList* TheEAChannelListParameter = 0;
EAExtract*     TheEAExtractParameter = 0;
EADelete*      TheEADeleteParameter = 0;

// ----------------------------------------------------------------------------

EAChannels::EAChannels( MetaProcess* P ) : MetaEnumeration( P )
{
   TheEAChannelsParameter = this;
}

IsoString EAChannels::Id() const
{
   return "channels";
}

size_type EAChannels::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString EAChannels::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case AllAlphaChannels:     return "AllAlphaChannels";
   case ActiveAlphaChannel:   return "ActiveAlphaChannel";
   case ChannelList:          return "ChannelList";
   }
}

int EAChannels::ElementValue( size_type i ) const
{
   return int( i );
}

size_type EAChannels::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

EAChannelList::EAChannelList( MetaProcess* P ) : MetaString( P )
{
   TheEAChannelListParameter = this;
}

IsoString EAChannelList::Id() const
{
   return "channelList";
}

String EAChannelList::DefaultValue() const
{
   return "0";
}

String EAChannelList::AllowedCharacters() const
{
   return "0123456789, ";
}

// ----------------------------------------------------------------------------

EAExtract::EAExtract( MetaProcess* P ) : MetaBoolean( P )
{
   TheEAExtractParameter = this;
}

IsoString EAExtract::Id() const
{
   return "extract";
}

bool EAExtract::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

EADelete::EADelete( MetaProcess* P ) : MetaBoolean( P )
{
   TheEADeleteParameter = this;
}

IsoString EADelete::Id() const
{
   return "delete";
}

bool EADelete::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsParameters.cpp - Released 2025-04-07T08:53:56Z
