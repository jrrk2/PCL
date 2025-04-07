//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AssistedColorCalibrationParameters.h - Released 2025-04-07T08:53:56Z
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

#ifndef __AssistedColorCalibrationParameters_h
#define __AssistedColorCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ACCRedCorrectionFactor : public MetaFloat
{
public:

   ACCRedCorrectionFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCRedCorrectionFactor* TheACCRedCorrectionFactor;

// ----------------------------------------------------------------------------

class ACCGreenCorrectionFactor : public MetaFloat
{
public:

   ACCGreenCorrectionFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCGreenCorrectionFactor* TheACCGreenCorrectionFactor;

// ----------------------------------------------------------------------------

class ACCBlueCorrectionFactor : public MetaFloat
{
public:

   ACCBlueCorrectionFactor( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCBlueCorrectionFactor* TheACCBlueCorrectionFactor;

// ----------------------------------------------------------------------------

class ACCBackgroundReference : public MetaString
{
public:

   ACCBackgroundReference( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String DefaultValue() const override;
};

extern ACCBackgroundReference* TheACCBackgroundReference;

// ----------------------------------------------------------------------------

class ACCHistogramShadows : public MetaFloat
{
public:

   ACCHistogramShadows( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCHistogramShadows* TheACCHistogramShadows;

// ----------------------------------------------------------------------------

class ACCHistogramHighlights : public MetaFloat
{
public:

   ACCHistogramHighlights( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCHistogramHighlights* TheACCHistogramHighlights;

// ----------------------------------------------------------------------------

class ACCHistogramMidtones : public MetaFloat
{
public:

   ACCHistogramMidtones( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCHistogramMidtones* TheACCHistogramMidtones;

// ----------------------------------------------------------------------------

class ACCSaturationBoost : public MetaFloat
{
public:

   ACCSaturationBoost( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ACCSaturationBoost* TheACCSaturationBoost;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __AssistedColorCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationParameters.h - Released 2025-04-07T08:53:56Z
