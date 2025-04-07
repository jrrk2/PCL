//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// UnsharpMaskParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __UnsharpMaskParameters_h
#define __UnsharpMaskParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class USMSigma : public MetaFloat
{
public:

   USMSigma( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern USMSigma* TheUSMSigmaParameter;

// ----------------------------------------------------------------------------

class USMAmount : public MetaFloat
{
public:

   USMAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern USMAmount* TheUSMAmountParameter;

// ----------------------------------------------------------------------------

class USMUseLuminance : public MetaBoolean
{
public:

   USMUseLuminance( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern USMUseLuminance* TheUSMUseLuminanceParameter;

// ----------------------------------------------------------------------------

class USMLinear : public MetaBoolean
{
public:

   USMLinear( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern USMLinear* TheUSMLinearParameter;

// ----------------------------------------------------------------------------

class USMDeringing : public MetaBoolean
{
public:

   USMDeringing( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern USMDeringing* TheUSMDeringingParameter;

// ----------------------------------------------------------------------------

class USMDeringingDark : public MetaFloat
{
public:

   USMDeringingDark( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern USMDeringingDark* TheUSMDeringingDarkParameter;

// ----------------------------------------------------------------------------

class USMDeringingBright : public MetaFloat
{
public:

   USMDeringingBright( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern USMDeringingBright* TheUSMDeringingBrightParameter;

// ----------------------------------------------------------------------------

class USMOutputDeringingMaps : public MetaBoolean
{
public:

   USMOutputDeringingMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern USMOutputDeringingMaps* TheUSMOutputDeringingMapsParameter;

// ----------------------------------------------------------------------------

class USMRangeLow : public MetaFloat
{
public:

   USMRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern USMRangeLow* TheUSMRangeLowParameter;

// ----------------------------------------------------------------------------

class USMRangeHigh : public MetaFloat
{
public:

   USMRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern USMRangeHigh* TheUSMRangeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __UnsharpMaskParameters_h

// ----------------------------------------------------------------------------
// EOF UnsharpMaskParameters.h - Released 2025-04-07T08:53:55Z
