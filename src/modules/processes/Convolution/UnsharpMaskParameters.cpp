//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// UnsharpMaskParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "UnsharpMaskParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

USMSigma*               TheUSMSigmaParameter = nullptr;
USMAmount*              TheUSMAmountParameter = nullptr;
USMUseLuminance*        TheUSMUseLuminanceParameter = nullptr;
USMLinear*              TheUSMLinearParameter = nullptr;
USMDeringing*           TheUSMDeringingParameter = nullptr;
USMDeringingDark*       TheUSMDeringingDarkParameter = nullptr;
USMDeringingBright*     TheUSMDeringingBrightParameter = nullptr;
USMOutputDeringingMaps* TheUSMOutputDeringingMapsParameter = nullptr;
USMRangeLow*            TheUSMRangeLowParameter = nullptr;
USMRangeHigh*           TheUSMRangeHighParameter = nullptr;

// ----------------------------------------------------------------------------

USMSigma::USMSigma( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMSigmaParameter = this;
}

IsoString USMSigma::Id() const
{
   return "sigma";
}

int USMSigma::Precision() const
{
   return 2;
}

double USMSigma::MinimumValue() const
{
   return 0.10;
}

double USMSigma::MaximumValue() const
{
   return 250;
}

double USMSigma::DefaultValue() const
{
   return 2.00;
}

// ----------------------------------------------------------------------------

USMAmount::USMAmount( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMAmountParameter = this;
}

IsoString USMAmount::Id() const
{
   return "amount";
}

int USMAmount::Precision() const
{
   return 2;
}

double USMAmount::MinimumValue() const
{
   return 0.10;
}

double USMAmount::MaximumValue() const
{
   return 1;
}

double USMAmount::DefaultValue() const
{
   return 0.80;
}

// ----------------------------------------------------------------------------

USMUseLuminance::USMUseLuminance( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMUseLuminanceParameter = this;
}

IsoString USMUseLuminance::Id() const
{
   return "useLuminance";
}

bool USMUseLuminance::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

USMLinear::USMLinear( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMLinearParameter = this;
}

IsoString USMLinear::Id() const
{
   return "linear";
}

bool USMLinear::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

USMDeringing::USMDeringing( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMDeringingParameter = this;
}

IsoString USMDeringing::Id() const
{
   return "deringing";
}

bool USMDeringing::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

USMDeringingDark::USMDeringingDark( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMDeringingDarkParameter = this;
}

IsoString USMDeringingDark::Id() const
{
   return "deringingDark";
}

int USMDeringingDark::Precision() const
{
   return 4;
}

double USMDeringingDark::DefaultValue() const
{
   return 0.1000;
}

double USMDeringingDark::MinimumValue() const
{
   return 0;
}

double USMDeringingDark::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

USMDeringingBright::USMDeringingBright( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMDeringingBrightParameter = this;
}

IsoString USMDeringingBright::Id() const
{
   return "deringingBright";
}

int USMDeringingBright::Precision() const
{
   return 4;
}

double USMDeringingBright::DefaultValue() const
{
   return 0;
}

double USMDeringingBright::MinimumValue() const
{
   return 0;
}

double USMDeringingBright::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

USMOutputDeringingMaps::USMOutputDeringingMaps( MetaProcess* P ) : MetaBoolean( P )
{
   TheUSMOutputDeringingMapsParameter = this;
}

IsoString USMOutputDeringingMaps::Id() const
{
   return "outputDeringingMaps";
}

bool USMOutputDeringingMaps::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

USMRangeLow::USMRangeLow( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMRangeLowParameter = this;
}

IsoString USMRangeLow::Id() const
{
   return "rangeLow";
}

int USMRangeLow::Precision() const
{
   return 7;
}

double USMRangeLow::MinimumValue() const
{
   return 0;
}

double USMRangeLow::MaximumValue() const
{
   return 1;
}

double USMRangeLow::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

USMRangeHigh::USMRangeHigh( MetaProcess* P ) : MetaFloat( P )
{
   TheUSMRangeHighParameter = this;
}

IsoString USMRangeHigh::Id() const
{
   return "rangeHigh";
}

int USMRangeHigh::Precision() const
{
   return 7;
}

double USMRangeHigh::MinimumValue() const
{
   return 0;
}

double USMRangeHigh::MaximumValue() const
{
   return 1;
}

double USMRangeHigh::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF UnsharpMaskParameters.cpp - Released 2025-04-07T08:53:55Z
