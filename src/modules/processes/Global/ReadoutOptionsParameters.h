//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ReadoutOptionsParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ReadoutOptionsParameters_h
#define __ReadoutOptionsParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ReadoutOptionsData : public MetaEnumeration
{
public:

   // This enumeration corresponds to pcl::ReadoutData (see pcl/ReadoutOptions.h)

   ReadoutOptionsData( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ReadoutOptionsData* TheReadoutOptionsDataParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsMode : public MetaEnumeration
{
public:

   // This enumeration corresponds to pcl::ReadoutMode (see pcl/ReadoutOptions.h)

   ReadoutOptionsMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ReadoutOptionsMode* TheReadoutOptionsModeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsProbeSize : public MetaInt32
{
public:

   ReadoutOptionsProbeSize( MetaProcess* );

   IsoString Id() const override;
   bool NeedsValidation() const override; // to ensure an odd value
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutOptionsProbeSize* TheReadoutOptionsProbeSizeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsPreviewSize : public MetaInt32
{
public:

   ReadoutOptionsPreviewSize( MetaProcess* );

   IsoString Id() const override;
   bool NeedsValidation() const override; // to ensure an odd value
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutOptionsPreviewSize* TheReadoutOptionsPreviewSizeParameter;

// ----------------------------------------------------------------------------

class ReadoutOptionsPreviewZoomFactor : public MetaInt32
{
public:

   ReadoutOptionsPreviewZoomFactor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutOptionsPreviewZoomFactor* TheReadoutOptionsPreviewZoomFactorParameter;

// ----------------------------------------------------------------------------

class ReadoutRealPrecision : public MetaInt32
{
public:

   ReadoutRealPrecision( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutRealPrecision* TheReadoutRealPrecisionParameter;

// ----------------------------------------------------------------------------

class ReadoutIntegerRange : public MetaUInt32
{
public:

   ReadoutIntegerRange( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutIntegerRange* TheReadoutIntegerRangeParameter;

// ----------------------------------------------------------------------------

class ReadoutAlpha : public MetaBoolean
{
public:

   ReadoutAlpha( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutAlpha* TheReadoutAlphaParameter;

// ----------------------------------------------------------------------------

class ReadoutMask : public MetaBoolean
{
public:

   ReadoutMask( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutMask* TheReadoutMaskParameter;

// ----------------------------------------------------------------------------

class ReadoutPreview : public MetaBoolean
{
public:

   ReadoutPreview( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutPreview* TheReadoutPreviewParameter;

// ----------------------------------------------------------------------------

class ReadoutPreviewCenter : public MetaBoolean
{
public:

   ReadoutPreviewCenter( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutPreviewCenter* TheReadoutPreviewCenterParameter;

// ----------------------------------------------------------------------------

class ReadoutShowEquatorial : public MetaBoolean
{
public:

   ReadoutShowEquatorial( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutShowEquatorial* TheReadoutShowEquatorialParameter;

// ----------------------------------------------------------------------------

class ReadoutShowEcliptic : public MetaBoolean
{
public:

   ReadoutShowEcliptic( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutShowEcliptic* TheReadoutShowEclipticParameter;

// ----------------------------------------------------------------------------

class ReadoutShowGalactic : public MetaBoolean
{
public:

   ReadoutShowGalactic( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutShowGalactic* TheReadoutShowGalacticParameter;

// ----------------------------------------------------------------------------

class ReadoutCoordinateItems : public MetaInt32
{
public:

   ReadoutCoordinateItems( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutCoordinateItems* TheReadoutCoordinateItemsParameter;

// ----------------------------------------------------------------------------

class ReadoutCoordinatePrecision : public MetaInt32
{
public:

   ReadoutCoordinatePrecision( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ReadoutCoordinatePrecision* TheReadoutCoordinatePrecisionParameter;

// ----------------------------------------------------------------------------

class ReadoutBroadcast : public MetaBoolean
{
public:

   ReadoutBroadcast( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutBroadcast* TheReadoutBroadcastParameter;

// ----------------------------------------------------------------------------

class ReadoutReal : public MetaBoolean
{
public:

   ReadoutReal( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ReadoutReal* TheReadoutRealParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ReadoutOptionsParameters_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsParameters.h - Released 2025-04-07T08:53:56Z
