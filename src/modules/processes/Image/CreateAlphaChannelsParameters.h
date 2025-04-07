//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// CreateAlphaChannelsParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CreateAlphaChannelsParameters_h
#define __CreateAlphaChannelsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CAFromImage : public MetaBoolean
{
public:

   CAFromImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CAFromImage* TheCAFromImageParameter;

// ----------------------------------------------------------------------------

class CASourceImageIdentifier : public MetaString
{
public:

   CASourceImageIdentifier( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;   // 0 = automatic
   String AllowedCharacters() const override;
};

extern CASourceImageIdentifier* TheCASourceImageIdentifierParameter;

// ----------------------------------------------------------------------------

class CAInvertSourceImage : public MetaBoolean
{
public:

   CAInvertSourceImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CAInvertSourceImage* TheCAInvertSourceImageParameter;

// ----------------------------------------------------------------------------

class CACloseSourceImage : public MetaBoolean
{
public:

   CACloseSourceImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CACloseSourceImage* TheCACloseSourceImageParameter;

// ----------------------------------------------------------------------------

class CATransparency : public MetaDouble
{
public:

   CATransparency( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CATransparency* TheCATransparencyParameter;

// ----------------------------------------------------------------------------

class CAReplaceExistingChannels : public MetaBoolean
{
public:

   CAReplaceExistingChannels( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CAReplaceExistingChannels* TheCAReplaceExistingChannelsParameter;

// ----------------------------------------------------------------------------

class CAChannelCount : public MetaInt32
{
public:

   CAChannelCount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern CAChannelCount* TheCAChannelCountParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CreateAlphaChannelsParameters_h

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsParameters.h - Released 2025-04-07T08:53:56Z
