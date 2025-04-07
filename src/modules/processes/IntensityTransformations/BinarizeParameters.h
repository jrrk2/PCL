//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BinarizeParameters_h
#define __BinarizeParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class BinarizeLevelR : public MetaDouble
{
public:

   BinarizeLevelR( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern BinarizeLevelR* TheBinarizeLevelRParameter;

// ----------------------------------------------------------------------------

class BinarizeLevelG : public MetaDouble
{
public:

   BinarizeLevelG( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern BinarizeLevelG* TheBinarizeLevelGParameter;

// ----------------------------------------------------------------------------

class BinarizeLevelB : public MetaDouble
{
public:

   BinarizeLevelB( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern BinarizeLevelB* TheBinarizeLevelBParameter;

// ----------------------------------------------------------------------------

class BinarizeIsGlobal : public MetaBoolean
{
public:

   BinarizeIsGlobal( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern BinarizeIsGlobal* TheBinarizeIsGlobalParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __BinarizeParameters_h

// ----------------------------------------------------------------------------
// EOF BinarizeParameters.h - Released 2025-04-07T08:53:56Z
