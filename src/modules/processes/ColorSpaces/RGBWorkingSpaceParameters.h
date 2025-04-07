//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// RGBWorkingSpaceParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RGBWorkingSpaceParameters_h
#define __RGBWorkingSpaceParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class RGBWSChannelTable : public MetaTable
{
public:

   RGBWSChannelTable( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   size_type MaxLength() const override;
};

extern RGBWSChannelTable*  TheRGBWSChannelTableParameter;

// ----------------------------------------------------------------------------

class RGBWSLuminanceCoefficient : public MetaFloat
{
public:

   RGBWSLuminanceCoefficient( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RGBWSLuminanceCoefficient*  TheRGBWSLuminanceCoefficientParameter;

// ----------------------------------------------------------------------------

class RGBWSChromaticityX : public MetaFloat
{
public:

   RGBWSChromaticityX( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RGBWSChromaticityX*   TheRGBWSChromaticityXParameter;

// ----------------------------------------------------------------------------

class RGBWSChromaticityY : public MetaFloat
{
public:

   RGBWSChromaticityY( MetaTable* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RGBWSChromaticityY*   TheRGBWSChromaticityYParameter;

// ----------------------------------------------------------------------------

class RGBWSGamma : public MetaFloat
{
public:

   RGBWSGamma( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RGBWSGamma*  TheRGBWSGammaParameter;

// ----------------------------------------------------------------------------

class RGBWSsRGBG : public MetaBoolean
{
public:

   RGBWSsRGBG( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RGBWSsRGBG*  TheRGBWSsRGBGParameter;

// ----------------------------------------------------------------------------

class RGBWSApplyGlobal : public MetaBoolean
{
public:

   RGBWSApplyGlobal( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RGBWSApplyGlobal*   TheRGBWSApplyGlobalParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __RGBWorkingSpaceParameters_h

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceParameters.h - Released 2025-04-07T08:53:55Z
