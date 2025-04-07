//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ChannelMatchParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelMatchParameters_h
#define __ChannelMatchParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CMChannels : public MetaTable
{
public:

   CMChannels( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

extern CMChannels* TheCMChannelsParameter;

// ----------------------------------------------------------------------------

class CMEnabled : public MetaBoolean
{
public:

   CMEnabled( MetaTable* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMEnabled* TheCMEnabledParameter;

// ----------------------------------------------------------------------------

class CMXOffset : public MetaFloat
{
public:

   CMXOffset( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CMXOffset* TheCMXOffsetParameter;

// ----------------------------------------------------------------------------

class CMYOffset : public MetaFloat
{
public:

   CMYOffset( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CMYOffset* TheCMYOffsetParameter;

// ----------------------------------------------------------------------------

class CMFactor : public MetaDouble
{
public:

   CMFactor( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern CMFactor* TheCMFactorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ChannelMatchParameters_h

// ----------------------------------------------------------------------------
// EOF ChannelMatchParameters.h - Released 2025-04-07T08:53:55Z
