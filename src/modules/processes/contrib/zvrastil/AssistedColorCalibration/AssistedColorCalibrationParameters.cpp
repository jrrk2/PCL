//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AssistedColorCalibrationParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AssistedColorCalibrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ACCRedCorrectionFactor*   TheACCRedCorrectionFactor = nullptr;
ACCGreenCorrectionFactor* TheACCGreenCorrectionFactor = nullptr;
ACCBlueCorrectionFactor*  TheACCBlueCorrectionFactor = nullptr;
ACCBackgroundReference*   TheACCBackgroundReference = nullptr;
ACCHistogramShadows*      TheACCHistogramShadows = nullptr;
ACCHistogramHighlights*   TheACCHistogramHighlights = nullptr;
ACCHistogramMidtones*     TheACCHistogramMidtones = nullptr;
ACCSaturationBoost*       TheACCSaturationBoost = nullptr;

// ----------------------------------------------------------------------------

ACCRedCorrectionFactor::ACCRedCorrectionFactor( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCRedCorrectionFactor = this;
}

IsoString ACCRedCorrectionFactor::Id() const
{
   return "redCorrectionFactor";
}

int ACCRedCorrectionFactor::Precision() const
{
   return 3;
}

double ACCRedCorrectionFactor::DefaultValue() const
{
   return 1;
}

double ACCRedCorrectionFactor::MinimumValue() const
{
   return 0;
}

double ACCRedCorrectionFactor::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ACCGreenCorrectionFactor::ACCGreenCorrectionFactor( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCGreenCorrectionFactor = this;
}

IsoString ACCGreenCorrectionFactor::Id() const
{
   return "GreenCorrectionFactor";
}

int ACCGreenCorrectionFactor::Precision() const
{
   return 3;
}

double ACCGreenCorrectionFactor::DefaultValue() const
{
   return 1;
}

double ACCGreenCorrectionFactor::MinimumValue() const
{
   return 0;
}

double ACCGreenCorrectionFactor::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ACCBlueCorrectionFactor::ACCBlueCorrectionFactor( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCBlueCorrectionFactor = this;
}

IsoString ACCBlueCorrectionFactor::Id() const
{
   return "BlueCorrectionFactor";
}

int ACCBlueCorrectionFactor::Precision() const
{
   return 3;
}

double ACCBlueCorrectionFactor::DefaultValue() const
{
   return 1;
}

double ACCBlueCorrectionFactor::MinimumValue() const
{
   return 0;
}

double ACCBlueCorrectionFactor::MaximumValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ACCBackgroundReference::ACCBackgroundReference( MetaProcess* P )
   : MetaString( P )
{
   TheACCBackgroundReference = this;
}

IsoString ACCBackgroundReference::Id() const
{
   return "backgroundRef";
}

size_type ACCBackgroundReference::MinLength() const
{
   return 0;
}

String ACCBackgroundReference::DefaultValue() const
{
   return "";
}

// ----------------------------------------------------------------------------

ACCHistogramShadows::ACCHistogramShadows( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCHistogramShadows = this;
}

IsoString ACCHistogramShadows::Id() const
{
   return "HistogramShadows";
}

int ACCHistogramShadows::Precision() const
{
   return 5;
}

double ACCHistogramShadows::DefaultValue() const
{
   return 0;
}

double ACCHistogramShadows::MinimumValue() const
{
   return 0;
}

double ACCHistogramShadows::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ACCHistogramHighlights::ACCHistogramHighlights( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCHistogramHighlights = this;
}

IsoString ACCHistogramHighlights::Id() const
{
   return "HistogramHighlights";
}

int ACCHistogramHighlights::Precision() const
{
   return 5;
}

double ACCHistogramHighlights::DefaultValue() const
{
   return 1;
}

double ACCHistogramHighlights::MinimumValue() const
{
   return 0;
}

double ACCHistogramHighlights::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ACCHistogramMidtones::ACCHistogramMidtones( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCHistogramMidtones = this;
}

IsoString ACCHistogramMidtones::Id() const
{
   return "HistogramMidtones";
}

int ACCHistogramMidtones::Precision() const
{
   return 5;
}

double ACCHistogramMidtones::DefaultValue() const
{
   return 0.5;
}

double ACCHistogramMidtones::MinimumValue() const
{
   return 0;
}

double ACCHistogramMidtones::MaximumValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ACCSaturationBoost::ACCSaturationBoost( MetaProcess* P )
   : MetaFloat( P )
{
   TheACCSaturationBoost = this;
}

IsoString ACCSaturationBoost::Id() const
{
   return "SaturationBoost";
}

int ACCSaturationBoost::Precision() const
{
   return 3;
}

double ACCSaturationBoost::DefaultValue() const
{
   return 1;
}

double ACCSaturationBoost::MinimumValue() const
{
   return 1;
}

double ACCSaturationBoost::MaximumValue() const
{
   return 5;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationParameters.cpp - Released 2025-04-07T08:53:56Z
