//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "BinarizeParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeLevelR*   TheBinarizeLevelRParameter = 0;
BinarizeLevelG*   TheBinarizeLevelGParameter = 0;
BinarizeLevelB*   TheBinarizeLevelBParameter = 0;
BinarizeIsGlobal* TheBinarizeIsGlobalParameter = 0;

// ----------------------------------------------------------------------------

BinarizeLevelR::BinarizeLevelR( MetaProcess* P ) : MetaDouble( P )
{
   TheBinarizeLevelRParameter = this;
}

IsoString BinarizeLevelR::Id() const
{
   return "thresholdRK";
}

int BinarizeLevelR::Precision() const
{
   return 8;
}

double BinarizeLevelR::MinimumValue() const
{
   return 0;
}

double BinarizeLevelR::MaximumValue() const
{
   return 1;
}

double BinarizeLevelR::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

BinarizeLevelG::BinarizeLevelG( MetaProcess* P ) : MetaDouble( P )
{
   TheBinarizeLevelGParameter = this;
}

IsoString BinarizeLevelG::Id() const
{
   return "thresholdG";
}

int BinarizeLevelG::Precision() const
{
   return 8;
}

double BinarizeLevelG::MinimumValue() const
{
   return 0;
}

double BinarizeLevelG::MaximumValue() const
{
   return 1;
}

double BinarizeLevelG::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

BinarizeLevelB::BinarizeLevelB( MetaProcess* P ) : MetaDouble( P )
{
   TheBinarizeLevelBParameter = this;
}

IsoString BinarizeLevelB::Id() const
{
   return "thresholdB";
}

int BinarizeLevelB::Precision() const
{
   return 8;
}

double BinarizeLevelB::MinimumValue() const
{
   return 0;
}

double BinarizeLevelB::MaximumValue() const
{
   return 1;
}

double BinarizeLevelB::DefaultValue() const
{
   return 0.5;
}

// ----------------------------------------------------------------------------

BinarizeIsGlobal::BinarizeIsGlobal( MetaProcess* P ) : MetaBoolean( P )
{
   TheBinarizeIsGlobalParameter = this;
}

IsoString BinarizeIsGlobal::Id() const
{
   return "isGlobal";
}

bool BinarizeIsGlobal::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeParameters.cpp - Released 2025-04-07T08:53:56Z
