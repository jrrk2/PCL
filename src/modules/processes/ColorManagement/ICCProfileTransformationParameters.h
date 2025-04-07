//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// ICCProfileTransformationParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ICCProfileTransformationParameters_h
#define __ICCProfileTransformationParameters_h

#include <pcl/MetaParameter.h>
#include <pcl/ICCProfileTransformation.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ICCTTargetProfile : public MetaString
{
public:

   ICCTTargetProfile( MetaProcess* );

   IsoString Id() const override;
};

extern ICCTTargetProfile* TheICCTTargetProfileParameter;

// ----------------------------------------------------------------------------

class ICCTToDefaultProfile : public MetaBoolean
{
public:

   ICCTToDefaultProfile( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICCTToDefaultProfile* TheICCTToDefaultProfileParameter;

// ----------------------------------------------------------------------------

class ICCTRenderingIntent : public MetaEnumeration
{
public:

   enum { Perceptual = ICCRenderingIntent::Perceptual,
          Saturation = ICCRenderingIntent::Saturation,
          RelativeColorimetric = ICCRenderingIntent::RelativeColorimetric,
          AbsoluteColorimetric = ICCRenderingIntent::AbsoluteColorimetric,
          NumberOfRenderingIntents,
          Default = RelativeColorimetric };

   ICCTRenderingIntent( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern ICCTRenderingIntent* TheICCTRenderingIntentParameter;

// ----------------------------------------------------------------------------

class ICCTUseBlackPointCompensation : public MetaBoolean
{
public:

   ICCTUseBlackPointCompensation( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICCTUseBlackPointCompensation* TheICCTUseBlackPointCompensationParameter;

// ----------------------------------------------------------------------------

class ICCTUseFloatingPointTransformation : public MetaBoolean
{
public:

   ICCTUseFloatingPointTransformation( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ICCTUseFloatingPointTransformation* TheICCTUseFloatingPointTransformationParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ICCProfileTransformationParameters_h

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationParameters.h - Released 2025-04-07T08:53:55Z
