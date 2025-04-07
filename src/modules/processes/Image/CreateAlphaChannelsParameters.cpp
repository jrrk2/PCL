//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// CreateAlphaChannelsParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CreateAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CAFromImage*               TheCAFromImageParameter = 0;
CASourceImageIdentifier*   TheCASourceImageIdentifierParameter = 0;
CAInvertSourceImage*       TheCAInvertSourceImageParameter = 0;
CACloseSourceImage*        TheCACloseSourceImageParameter = 0;
CATransparency*            TheCATransparencyParameter = 0;
CAReplaceExistingChannels* TheCAReplaceExistingChannelsParameter = 0;
CAChannelCount*            TheCAChannelCountParameter = 0;

// ----------------------------------------------------------------------------

CAFromImage::CAFromImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheCAFromImageParameter = this;
}

IsoString CAFromImage::Id() const
{
   return "fromImage";
}

bool CAFromImage::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CASourceImageIdentifier::CASourceImageIdentifier( MetaProcess* P ) : MetaString( P )
{
   TheCASourceImageIdentifierParameter = this;
}

IsoString CASourceImageIdentifier::Id() const
{
   return "sourceId";
}

size_type CASourceImageIdentifier::MinLength() const
{
   return 0; // can be void
}

String CASourceImageIdentifier::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

CAInvertSourceImage::CAInvertSourceImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheCAInvertSourceImageParameter = this;
}

IsoString CAInvertSourceImage::Id() const
{
   return "invertSource";
}

bool CAInvertSourceImage::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CACloseSourceImage::CACloseSourceImage( MetaProcess* P ) : MetaBoolean( P )
{
   TheCACloseSourceImageParameter = this;
}

IsoString CACloseSourceImage::Id() const
{
   return "closeSource";
}

bool CACloseSourceImage::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CATransparency::CATransparency( MetaProcess* P ) : MetaDouble( P )
{
   TheCATransparencyParameter = this;
}

IsoString CATransparency::Id() const
{
   return "transparency";
}

int CATransparency::Precision() const
{
   return 8;
}

double CATransparency::DefaultValue() const
{
   return 1;
}

double CATransparency::MinimumValue() const
{
   return 0;
}

double CATransparency::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

CAReplaceExistingChannels::CAReplaceExistingChannels( MetaProcess* P ) : MetaBoolean( P )
{
   TheCAReplaceExistingChannelsParameter = this;
}

IsoString CAReplaceExistingChannels::Id() const
{
   return "replace";
}

bool CAReplaceExistingChannels::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CAChannelCount::CAChannelCount( MetaProcess* P ) : MetaInt32( P )
{
   TheCAChannelCountParameter = this;
}

IsoString CAChannelCount::Id() const
{
   return "count";
}

double CAChannelCount::DefaultValue() const
{
   return 1;
}

double CAChannelCount::MinimumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsParameters.cpp - Released 2025-04-07T08:53:56Z
