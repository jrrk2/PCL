//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// NewImageParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NewImageParameters_h
#define __NewImageParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class NewImageIdentifier : public MetaString
{
public:

   NewImageIdentifier( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern NewImageIdentifier* TheNewImageIdentifierParameter;

// ----------------------------------------------------------------------------

class NewImageWidth : public MetaInt32
{
public:

   NewImageWidth( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern NewImageWidth* TheNewImageWidthParameter;

// ----------------------------------------------------------------------------

class NewImageHeight : public MetaInt32
{
public:

   NewImageHeight( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
};

extern NewImageHeight* TheNewImageHeightParameter;

// ----------------------------------------------------------------------------

class NewImageNumberOfChannels : public MetaInt32
{
public:

   NewImageNumberOfChannels( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern NewImageNumberOfChannels* TheNewImageNumberOfChannelsParameter;

// ----------------------------------------------------------------------------

class NewImageColorSpace : public MetaEnumeration
{
public:

   enum { RGB,    // RGB Color
          Gray,   // Grayscale
          Default = Gray };

   NewImageColorSpace( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern NewImageColorSpace* TheNewImageColorSpaceParameter;

// ----------------------------------------------------------------------------

class NewImageSampleFormat : public MetaEnumeration
{
public:

   enum { I8,     // unsigned 8-bit integer
          I16,    // unsigned 16-bit integer
          I32,    // unsigned 32-bit integer
          F32,    // IEEE 32-bit floating point (simple precision)
          F64,    // IEEE 64-bit floating point (double precision)
          Default = F32 };

   NewImageSampleFormat( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern NewImageSampleFormat* TheNewImageSampleFormatParameter;

// ----------------------------------------------------------------------------

class NewImageV0 : public MetaDouble
{
public:

   NewImageV0( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern NewImageV0* TheNewImageV0Parameter;

// ----------------------------------------------------------------------------

class NewImageV1 : public MetaDouble
{
public:

   NewImageV1( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern NewImageV1* TheNewImageV1Parameter;

// ----------------------------------------------------------------------------

class NewImageV2 : public MetaDouble
{
public:

   NewImageV2( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern NewImageV2* TheNewImageV2Parameter;

// ----------------------------------------------------------------------------

class NewImageVA : public MetaDouble
{
public:

   NewImageVA( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern NewImageVA* TheNewImageVAParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __NewImageParameters_h

// ----------------------------------------------------------------------------
// EOF NewImageParameters.h - Released 2025-04-07T08:53:56Z
