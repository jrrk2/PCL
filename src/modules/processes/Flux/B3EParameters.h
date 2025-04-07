//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// B3EParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __B3EParameters_h
#define __B3EParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class B3EInputViewId1 : public MetaString
{
public:

   B3EInputViewId1( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern B3EInputViewId1* TheB3EInputViewId1Parameter;

class B3EInputViewId2 : public MetaString
{
public:

   B3EInputViewId2( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern B3EInputViewId2* TheB3EInputViewId2Parameter;

// ----------------------------------------------------------------------------

class B3EInputCenter1 : public MetaDouble
{
public:

   B3EInputCenter1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EInputCenter1* TheB3EInputCenter1Parameter;

// ----------------------------------------------------------------------------

class B3EInputCenter2 : public MetaDouble
{
public:

   B3EInputCenter2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EInputCenter2* TheB3EInputCenter2Parameter;

// ----------------------------------------------------------------------------

class B3EOutputCenter : public MetaDouble
{
public:

   B3EOutputCenter( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EOutputCenter* TheB3EOutputCenterParameter;

// ----------------------------------------------------------------------------

class B3EIntensityUnits : public MetaEnumeration
{
public:

   enum { PhotonsFrequency,
          EnergyFrequency,
          PhotonsWavelength,
          EnergyWavelength,
          NumberOfItems,
          Default = PhotonsWavelength };

   B3EIntensityUnits( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern B3EIntensityUnits* TheB3EIntensityUnitsParameter;

// ----------------------------------------------------------------------------

class B3ESyntheticImage : public MetaBoolean
{
public:

   B3ESyntheticImage( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3ESyntheticImage* TheB3ESyntheticImageParameter;

// ----------------------------------------------------------------------------

class B3EThermalMap : public MetaBoolean
{
public:

   B3EThermalMap( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3EThermalMap* TheB3EThermalMapParameter;

// ----------------------------------------------------------------------------

class B3EOutOfRangeMask : public MetaBoolean
{
public:

   B3EOutOfRangeMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3EOutOfRangeMask* TheB3EOutOfRangeMaskParameter;

// ----------------------------------------------------------------------------

class B3ESyntheticImageViewId : public MetaString
{
public:

   B3ESyntheticImageViewId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern B3ESyntheticImageViewId* TheB3ESyntheticImageViewIdParameter;

// ----------------------------------------------------------------------------

class B3EThermalMapViewId : public MetaString
{
public:

   B3EThermalMapViewId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern B3EThermalMapViewId* TheB3EThermalMapViewIdParameter;

// ----------------------------------------------------------------------------

class B3EOutOfRangeMaskViewId : public MetaString
{
public:

   B3EOutOfRangeMaskViewId( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern B3EOutOfRangeMaskViewId* TheB3EOutOfRangeMaskViewIdParameter;

// ----------------------------------------------------------------------------

/*
 * Background Calibration 1
 */

// ----------------------------------------------------------------------------

class B3ESubstractBackground1 : public MetaBoolean
{
public:

   B3ESubstractBackground1( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3ESubstractBackground1* TheB3ESubstractBackground1Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundReferenceViewId1 : public MetaString
{
public:

   B3EBackgroundReferenceViewId1( MetaProcess* );

   IsoString Id() const override;
};

extern B3EBackgroundReferenceViewId1* TheB3EBackgroundReferenceViewId1Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundLow1 : public MetaFloat
{
public:

   B3EBackgroundLow1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern B3EBackgroundLow1* TheB3EBackgroundLow1Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundHigh1 : public MetaFloat
{
public:

   B3EBackgroundHigh1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern B3EBackgroundHigh1* TheB3EBackgroundHigh1Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundUseROI1 : public MetaBoolean
{
public:

   B3EBackgroundUseROI1( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3EBackgroundUseROI1* TheB3EBackgroundUseROI1Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundROIX01 : public MetaInt32
{
public:

   B3EBackgroundROIX01( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIX01* TheB3EBackgroundROIX01Parameter;


// ----------------------------------------------------------------------------

class B3EBackgroundROIY01 : public MetaInt32
{
public:

   B3EBackgroundROIY01( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIY01* TheB3EBackgroundROIY01Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundROIX11 : public MetaInt32
{
public:

   B3EBackgroundROIX11( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIX11* TheB3EBackgroundROIX11Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundROIY11 : public MetaInt32
{
public:

   B3EBackgroundROIY11( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIY11* TheB3EBackgroundROIY11Parameter;

// ----------------------------------------------------------------------------

class B3EOutputBackgroundReferenceMask1 : public MetaBoolean
{
public:

   B3EOutputBackgroundReferenceMask1( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3EOutputBackgroundReferenceMask1* TheB3EOutputBackgroundReferenceMask1Parameter;

// ----------------------------------------------------------------------------

/*
 * Background Calibration 2
 */

// ----------------------------------------------------------------------------

class B3ESubstractBackground2 : public MetaBoolean
{
public:

   B3ESubstractBackground2( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3ESubstractBackground2* TheB3ESubstractBackground2Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundReferenceViewId2 : public MetaString
{
public:

   B3EBackgroundReferenceViewId2( MetaProcess* );

   IsoString Id() const override;
};

extern B3EBackgroundReferenceViewId2* TheB3EBackgroundReferenceViewId2Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundLow2 : public MetaFloat
{
public:

   B3EBackgroundLow2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern B3EBackgroundLow2* TheB3EBackgroundLow2Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundHigh2 : public MetaFloat
{
public:

   B3EBackgroundHigh2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern B3EBackgroundHigh2* TheB3EBackgroundHigh2Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundUseROI2 : public MetaBoolean
{
public:

   B3EBackgroundUseROI2( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3EBackgroundUseROI2* TheB3EBackgroundUseROI2Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundROIX02 : public MetaInt32
{
public:

   B3EBackgroundROIX02( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIX02* TheB3EBackgroundROIX02Parameter;


// ----------------------------------------------------------------------------

class B3EBackgroundROIY02 : public MetaInt32
{
public:

   B3EBackgroundROIY02( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIY02* TheB3EBackgroundROIY02Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundROIX12 : public MetaInt32
{
public:

   B3EBackgroundROIX12( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIX12* TheB3EBackgroundROIX12Parameter;

// ----------------------------------------------------------------------------

class B3EBackgroundROIY12 : public MetaInt32
{
public:

   B3EBackgroundROIY12( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern B3EBackgroundROIY12* TheB3EBackgroundROIY12Parameter;

// ----------------------------------------------------------------------------

class B3EOutputBackgroundReferenceMask2 : public MetaBoolean
{
public:

   B3EOutputBackgroundReferenceMask2( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern B3EOutputBackgroundReferenceMask2* TheB3EOutputBackgroundReferenceMask2Parameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __B3EParameters_h

// ----------------------------------------------------------------------------
// EOF B3EParameters.h - Released 2025-04-07T08:53:55Z
