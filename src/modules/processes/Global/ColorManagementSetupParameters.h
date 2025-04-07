//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ColorManagementSetupParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ColorManagementSetupParameters_h
#define __ColorManagementSetupParameters_h

#include <pcl/ICCProfileTransformation.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CMSRenderingIntent : public MetaEnumeration
{
public:

   enum { Perceptual = ICCRenderingIntent::Perceptual,
          Saturation = ICCRenderingIntent::Saturation,
          RelativeColorimetric = ICCRenderingIntent::RelativeColorimetric,
          AbsoluteColorimetric = ICCRenderingIntent::AbsoluteColorimetric,
          NumberOfRenderingIntents,
          DefaultForScreen = Perceptual,
          DefaultForProofing = RelativeColorimetric };

   CMSRenderingIntent( MetaProcess* );

   virtual IsoString Id() const = 0;

   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
};

// ----------------------------------------------------------------------------

class CMSEnabled : public MetaBoolean
{
public:

   CMSEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSEnabled* TheCMSEnabledParameter;

// ----------------------------------------------------------------------------

class CMSDetectMonitorProfile : public MetaBoolean
{
public:

   CMSDetectMonitorProfile( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDetectMonitorProfile* TheCMSDetectMonitorProfileParameter;

// ----------------------------------------------------------------------------

class CMSUpdateMonitorProfile : public MetaString
{
public:

   CMSUpdateMonitorProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSUpdateMonitorProfile* TheCMSUpdateMonitorProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultRGBProfile : public MetaString
{
public:

   CMSDefaultRGBProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSDefaultRGBProfile* TheCMSDefaultRGBProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultGrayProfile : public MetaString
{
public:

   CMSDefaultGrayProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSDefaultGrayProfile* TheCMSDefaultGrayProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultRenderingIntent : public CMSRenderingIntent
{
public:

   CMSDefaultRenderingIntent( MetaProcess* );

   IsoString Id() const override;
   size_type DefaultValueIndex() const override;
};

extern CMSDefaultRenderingIntent* TheCMSDefaultRenderingIntentParameter;

// ----------------------------------------------------------------------------

class CMSOnProfileMismatch : public MetaEnumeration
{
public:

   enum { AskUser,
          KeepEmbedded,
          ConvertToDefault,
          DiscardEmbedded,
          DisableCM,
          NumberOfOnProfileMismatchPolicies,
          Default = KeepEmbedded };

   CMSOnProfileMismatch( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern CMSOnProfileMismatch* TheCMSOnProfileMismatchParameter;

// ----------------------------------------------------------------------------

class CMSOnMissingProfile : public MetaEnumeration
{
public:

   enum { AskUser,
          AssignDefault,
          LeaveUntagged,
          DisableCM,
          NumberOfOnMissingProfilePolicies,
          Default = LeaveUntagged };

   CMSOnMissingProfile( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern CMSOnMissingProfile* TheCMSOnMissingProfileParameter;

// ----------------------------------------------------------------------------

class CMSDefaultEmbedProfilesInRGBImages : public MetaBoolean
{
public:

   CMSDefaultEmbedProfilesInRGBImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultEmbedProfilesInRGBImages* TheCMSDefaultEmbedProfilesInRGBImagesParameter;

// ----------------------------------------------------------------------------

class CMSDefaultEmbedProfilesInGrayscaleImages : public MetaBoolean
{
public:

   CMSDefaultEmbedProfilesInGrayscaleImages( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultEmbedProfilesInGrayscaleImages* TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter;

// ----------------------------------------------------------------------------

class CMSUseLowResolutionCLUTs : public MetaBoolean
{
public:

   CMSUseLowResolutionCLUTs( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSUseLowResolutionCLUTs* TheCMSUseLowResolutionCLUTsParameter;

// ----------------------------------------------------------------------------

class CMSProofingProfile : public MetaString
{
public:

   CMSProofingProfile( MetaProcess* );

   IsoString Id() const override;
};

extern CMSProofingProfile* TheCMSProofingProfileParameter;

// ----------------------------------------------------------------------------

class CMSProofingIntent : public CMSRenderingIntent
{
public:

   CMSProofingIntent( MetaProcess* );

   IsoString Id() const override;
   size_type DefaultValueIndex() const override;
};

extern CMSProofingIntent* TheCMSProofingIntentParameter;

// ----------------------------------------------------------------------------

class CMSUseProofingBPC : public MetaBoolean
{
public:

   CMSUseProofingBPC( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSUseProofingBPC* TheCMSUseProofingBPCParameter;

// ----------------------------------------------------------------------------

class CMSDefaultProofingEnabled : public MetaBoolean
{
public:

   CMSDefaultProofingEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultProofingEnabled* TheCMSDefaultProofingEnabledParameter;

// ----------------------------------------------------------------------------

class CMSDefaultGamutCheckEnabled : public MetaBoolean
{
public:

   CMSDefaultGamutCheckEnabled( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern CMSDefaultGamutCheckEnabled* TheCMSDefaultGamutCheckEnabledParameter;

// ----------------------------------------------------------------------------

class CMSGamutWarningColor : public MetaUInt32
{
public:

   CMSGamutWarningColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern CMSGamutWarningColor* TheCMSGamutWarningColorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ColorManagementSetupParameters_h

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupParameters.h - Released 2025-04-07T08:53:56Z
