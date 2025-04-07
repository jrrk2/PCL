//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ColorManagementSetupParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ColorManagementSetupParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CMSEnabled*                               TheCMSEnabledParameter = nullptr;
CMSDetectMonitorProfile*                  TheCMSDetectMonitorProfileParameter = nullptr;
CMSUpdateMonitorProfile*                  TheCMSUpdateMonitorProfileParameter = nullptr;
CMSDefaultRGBProfile*                     TheCMSDefaultRGBProfileParameter = nullptr;
CMSDefaultGrayProfile*                    TheCMSDefaultGrayProfileParameter = nullptr;
CMSDefaultRenderingIntent*                TheCMSDefaultRenderingIntentParameter = nullptr;
CMSOnProfileMismatch*                     TheCMSOnProfileMismatchParameter = nullptr;
CMSOnMissingProfile*                      TheCMSOnMissingProfileParameter = nullptr;
CMSDefaultEmbedProfilesInRGBImages*       TheCMSDefaultEmbedProfilesInRGBImagesParameter = nullptr;
CMSDefaultEmbedProfilesInGrayscaleImages* TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter = nullptr;
CMSUseLowResolutionCLUTs*                 TheCMSUseLowResolutionCLUTsParameter = nullptr;
CMSProofingProfile*                       TheCMSProofingProfileParameter = nullptr;
CMSProofingIntent*                        TheCMSProofingIntentParameter = nullptr;
CMSUseProofingBPC*                        TheCMSUseProofingBPCParameter = nullptr;
CMSDefaultProofingEnabled*                TheCMSDefaultProofingEnabledParameter = nullptr;
CMSDefaultGamutCheckEnabled*              TheCMSDefaultGamutCheckEnabledParameter = nullptr;
CMSGamutWarningColor*                     TheCMSGamutWarningColorParameter = nullptr;

// ----------------------------------------------------------------------------

CMSRenderingIntent::CMSRenderingIntent( MetaProcess* P ) : MetaEnumeration( P )
{
}

size_type CMSRenderingIntent::NumberOfElements() const
{
   return NumberOfRenderingIntents;
}

IsoString CMSRenderingIntent::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Perceptual:           return "Perceptual";
   case Saturation:           return "Saturation";
   case RelativeColorimetric: return "RelativeColorimetric";
   case AbsoluteColorimetric: return "AbsoluteColorimetric";
   }
}

int CMSRenderingIntent::ElementValue( size_type i ) const
{
   return int( i );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CMSEnabled::CMSEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSEnabledParameter = this;
}

IsoString CMSEnabled::Id() const
{
   return "enabled";
}

bool CMSEnabled::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSDetectMonitorProfile::CMSDetectMonitorProfile( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDetectMonitorProfileParameter = this;
}

IsoString CMSDetectMonitorProfile::Id() const
{
   return "detectMonitorProfile";
}

bool CMSDetectMonitorProfile::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSUpdateMonitorProfile::CMSUpdateMonitorProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSUpdateMonitorProfileParameter = this;
}

IsoString CMSUpdateMonitorProfile::Id() const
{
   return "updateMonitorProfile";
}

// ----------------------------------------------------------------------------

CMSDefaultRGBProfile::CMSDefaultRGBProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSDefaultRGBProfileParameter = this;
}

IsoString CMSDefaultRGBProfile::Id() const
{
   return "defaultRGBProfile";
}

// ----------------------------------------------------------------------------

CMSDefaultGrayProfile::CMSDefaultGrayProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSDefaultGrayProfileParameter = this;
}

IsoString CMSDefaultGrayProfile::Id() const
{
   return "defaultGrayscaleProfile";
}

// ----------------------------------------------------------------------------

CMSDefaultRenderingIntent::CMSDefaultRenderingIntent( MetaProcess* P ) : CMSRenderingIntent( P )
{
   TheCMSDefaultRenderingIntentParameter = this;
}

IsoString CMSDefaultRenderingIntent::Id() const
{
   return "defaultRenderingIntent";
}

size_type CMSDefaultRenderingIntent::DefaultValueIndex() const
{
   return DefaultForScreen;
}

// ----------------------------------------------------------------------------

CMSOnProfileMismatch::CMSOnProfileMismatch( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCMSOnProfileMismatchParameter = this;
}

IsoString CMSOnProfileMismatch::Id() const
{
   return "onProfileMismatch";
}

size_type CMSOnProfileMismatch::NumberOfElements() const
{
   return NumberOfOnProfileMismatchPolicies;
}

IsoString CMSOnProfileMismatch::ElementId( size_type i ) const
{
   switch ( i )
   {
   case AskUser:           return "AskUser";
   default:
   case KeepEmbedded:      return "KeepEmbedded";
   case ConvertToDefault:  return "ConvertToDefault";
   case DiscardEmbedded:   return "DiscardEmbedded";
   case DisableCM:         return "DisableCM";
   }
}

int CMSOnProfileMismatch::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CMSOnProfileMismatch::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

CMSOnMissingProfile::CMSOnMissingProfile( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCMSOnMissingProfileParameter = this;
}

IsoString CMSOnMissingProfile::Id() const
{
   return "onMissingProfile";
}

size_type CMSOnMissingProfile::NumberOfElements() const
{
   return NumberOfOnMissingProfilePolicies;
}

IsoString CMSOnMissingProfile::ElementId( size_type i ) const
{
   switch ( i )
   {
   case AskUser:        return "AskUser";
   default:
   case AssignDefault:  return "AssignDefault";
   case LeaveUntagged:  return "LeaveUntagged";
   case DisableCM:      return "DisableCM";
   }
}

int CMSOnMissingProfile::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CMSOnMissingProfile::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

CMSDefaultEmbedProfilesInRGBImages::CMSDefaultEmbedProfilesInRGBImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultEmbedProfilesInRGBImagesParameter = this;
}

IsoString CMSDefaultEmbedProfilesInRGBImages::Id() const
{
   return "defaultEmbedProfilesInRGBImages";
}

bool CMSDefaultEmbedProfilesInRGBImages::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSDefaultEmbedProfilesInGrayscaleImages::CMSDefaultEmbedProfilesInGrayscaleImages( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultEmbedProfilesInGrayscaleImagesParameter = this;
}

IsoString CMSDefaultEmbedProfilesInGrayscaleImages::Id() const
{
   return "defaultEmbedProfilesInGrayscaleImages";
}

bool CMSDefaultEmbedProfilesInGrayscaleImages::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSUseLowResolutionCLUTs::CMSUseLowResolutionCLUTs( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSUseLowResolutionCLUTsParameter = this;
}

IsoString CMSUseLowResolutionCLUTs::Id() const
{
   return "useLowResolutionCLUTs";
}

bool CMSUseLowResolutionCLUTs::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSProofingProfile::CMSProofingProfile( MetaProcess* P ) : MetaString( P )
{
   TheCMSProofingProfileParameter = this;
}

IsoString CMSProofingProfile::Id() const
{
   return "proofingProfile";
}

// ----------------------------------------------------------------------------

CMSProofingIntent::CMSProofingIntent( MetaProcess* P ) : CMSRenderingIntent( P )
{
   TheCMSProofingIntentParameter = this;
}

IsoString CMSProofingIntent::Id() const
{
   return "proofingIntent";
}

size_type CMSProofingIntent::DefaultValueIndex() const
{
   return DefaultForProofing;
}

// ----------------------------------------------------------------------------

CMSUseProofingBPC::CMSUseProofingBPC( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSUseProofingBPCParameter = this;
}

IsoString CMSUseProofingBPC::Id() const
{
   return "useProofingBPC";
}

bool CMSUseProofingBPC::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

CMSDefaultProofingEnabled::CMSDefaultProofingEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultProofingEnabledParameter = this;
}

IsoString CMSDefaultProofingEnabled::Id() const
{
   return "defaultProofingEnabled";
}

bool CMSDefaultProofingEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSDefaultGamutCheckEnabled::CMSDefaultGamutCheckEnabled( MetaProcess* P ) : MetaBoolean( P )
{
   TheCMSDefaultGamutCheckEnabledParameter = this;
}

IsoString CMSDefaultGamutCheckEnabled::Id() const
{
   return "defaultGamutCheckEnabled";
}

bool CMSDefaultGamutCheckEnabled::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

CMSGamutWarningColor::CMSGamutWarningColor( MetaProcess* P ) : MetaUInt32( P )
{
   TheCMSGamutWarningColorParameter = this;
}

IsoString CMSGamutWarningColor::Id() const
{
   return "gamutWarningColor";
}

double CMSGamutWarningColor::DefaultValue() const
{
   return 0xffa9a9a9; // Dark Gray
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupParameters.cpp - Released 2025-04-07T08:53:56Z
