//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// FluxCalibrationParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FluxCalibrationParameters_h
#define __FluxCalibrationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCParameterMode : public MetaEnumeration
{
public:

   enum { Literal,
          StandardKeyword,
          CustomKeyword,
          NumberOfItems,
          Default = Literal };

   FCParameterMode( MetaProcess* P ) : MetaEnumeration( P )
   {
   }

   virtual IsoString Id() const = 0;

   size_type NumberOfElements() const override
   {
      return NumberOfItems;
   }

   IsoString ElementId( size_type index ) const override
   {
      switch ( index )
      {
      default:
      case Literal:         return "Literal";
      case StandardKeyword: return "StandardKeyword";
      case CustomKeyword:   return "CustomKeyword";
      }
   }

   int ElementValue( size_type index ) const override
   {
      return int( index );
   }

   size_type DefaultValueIndex() const override
   {
      return Default;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCWavelengthValue : public MetaFloat
{
public:

   FCWavelengthValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double DefaultValue() const override;
};

extern FCWavelengthValue* TheFCWavelengthValueParameter;

// ----------------------------------------------------------------------------

class FCWavelengthMode : public FCParameterMode
{
public:

   FCWavelengthMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCWavelengthMode* TheFCWavelengthModeParameter;

// ----------------------------------------------------------------------------

class FCWavelengthKeyword : public MetaString
{
public:

   FCWavelengthKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCWavelengthKeyword* TheFCWavelengthKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCTransmissivityValue : public MetaFloat
{
public:

   FCTransmissivityValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern FCTransmissivityValue* TheFCTransmissivityValueParameter;

// ----------------------------------------------------------------------------

class FCTransmissivityMode : public FCParameterMode
{
public:

   FCTransmissivityMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCTransmissivityMode* TheFCTransmissivityModeParameter;

// ----------------------------------------------------------------------------

class FCTransmissivityKeyword : public MetaString
{
public:

   FCTransmissivityKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCTransmissivityKeyword* TheFCTransmissivityKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCFilterWidthValue : public MetaFloat
{
public:

   FCFilterWidthValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double DefaultValue() const override;
};

extern FCFilterWidthValue* TheFCFilterWidthValueParameter;

// ----------------------------------------------------------------------------

class FCFilterWidthMode : public FCParameterMode
{
public:

   FCFilterWidthMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCFilterWidthMode* TheFCFilterWidthModeParameter;

// ----------------------------------------------------------------------------

class FCFilterWidthKeyword : public MetaString
{
public:

   FCFilterWidthKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCFilterWidthKeyword* TheFCFilterWidthKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCApertureValue : public MetaFloat
{
public:

   FCApertureValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double DefaultValue() const override;
};

extern FCApertureValue* TheFCApertureValueParameter;

// ----------------------------------------------------------------------------

class FCApertureMode : public FCParameterMode
{
public:

   FCApertureMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCApertureMode* TheFCApertureModeParameter;

// ----------------------------------------------------------------------------

class FCApertureKeyword : public MetaString
{
public:

   FCApertureKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCApertureKeyword* TheFCApertureKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCCentralObstructionValue : public MetaFloat
{
public:

   FCCentralObstructionValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double DefaultValue() const override;
};

extern FCCentralObstructionValue* TheFCCentralObstructionValueParameter;

// ----------------------------------------------------------------------------

class FCCentralObstructionMode : public FCParameterMode
{
public:

   FCCentralObstructionMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCCentralObstructionMode* TheFCCentralObstructionModeParameter;

// ----------------------------------------------------------------------------

class FCCentralObstructionKeyword : public MetaString
{
public:

   FCCentralObstructionKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCCentralObstructionKeyword* TheFCCentralObstructionKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCExposureTimeValue : public MetaFloat
{
public:

   FCExposureTimeValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double DefaultValue() const override;
};

extern FCExposureTimeValue* TheFCExposureTimeValueParameter;

// ----------------------------------------------------------------------------

class FCExposureTimeMode : public FCParameterMode
{
public:

   FCExposureTimeMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCExposureTimeMode* TheFCExposureTimeModeParameter;

// ----------------------------------------------------------------------------

class FCExposureTimeKeyword : public MetaString
{
public:

   FCExposureTimeKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCExposureTimeKeyword* TheFCExposureTimeKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCAtmosphericExtinctionValue : public MetaFloat
{
public:

   FCAtmosphericExtinctionValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern FCAtmosphericExtinctionValue* TheFCAtmosphericExtinctionValueParameter;

// ----------------------------------------------------------------------------

class FCAtmosphericExtinctionMode : public FCParameterMode
{
public:

   FCAtmosphericExtinctionMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCAtmosphericExtinctionMode* TheFCAtmosphericExtinctionModeParameter;

// ----------------------------------------------------------------------------

class FCAtmosphericExtinctionKeyword : public MetaString
{
public:

   FCAtmosphericExtinctionKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCAtmosphericExtinctionKeyword* TheFCAtmosphericExtinctionKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCSensorGainValue : public MetaFloat
{
public:

   FCSensorGainValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double DefaultValue() const override;
};

extern FCSensorGainValue* TheFCSensorGainValueParameter;

// ----------------------------------------------------------------------------

class FCSensorGainMode : public FCParameterMode
{
public:

   FCSensorGainMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCSensorGainMode* TheFCSensorGainModeParameter;

// ----------------------------------------------------------------------------

class FCSensorGainKeyword : public MetaString
{
public:

   FCSensorGainKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCSensorGainKeyword* TheFCSensorGainKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FCQuantumEfficiencyValue : public MetaFloat
{
public:

   FCQuantumEfficiencyValue( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern FCQuantumEfficiencyValue* TheFCQuantumEfficiencyValueParameter;

// ----------------------------------------------------------------------------

class FCQuantumEfficiencyMode : public FCParameterMode
{
public:

   FCQuantumEfficiencyMode( MetaProcess* P );

   IsoString Id() const override;
};

extern FCQuantumEfficiencyMode* TheFCQuantumEfficiencyModeParameter;

// ----------------------------------------------------------------------------

class FCQuantumEfficiencyKeyword : public MetaString
{
public:

   FCQuantumEfficiencyKeyword( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern FCQuantumEfficiencyKeyword* TheFCQuantumEfficiencyKeywordParameter;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __FluxCalibrationParameters_h

// ----------------------------------------------------------------------------
// EOF FluxCalibrationParameters.h - Released 2025-04-07T08:53:55Z
