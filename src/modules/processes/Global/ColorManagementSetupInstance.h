//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ColorManagementSetupInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ColorManagementSetupInstance_h
#define __ColorManagementSetupInstance_h

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ColorManagementSetupInstance : public ProcessImplementation
{
public:

   ColorManagementSetupInstance( const MetaProcess* );
   ColorManagementSetupInstance( const ColorManagementSetupInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( pcl::String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void LoadCurrentSettings();

private:

   pcl_bool p_enabled;
   pcl_bool p_detectMonitorProfile;
   String   p_updateMonitorProfile;
   String   p_defaultRGBProfile;
   String   p_defaultGrayscaleProfile;
   pcl_enum p_defaultRenderingIntent;
   pcl_enum p_onProfileMismatch;
   pcl_enum p_onMissingProfile;
   pcl_bool p_defaultEmbedProfilesInRGBImages;
   pcl_bool p_defaultEmbedProfilesInGrayscaleImages;
   pcl_bool p_useLowResolutionCLUTs;
   String   p_proofingProfile;
   pcl_enum p_proofingIntent;
   pcl_bool p_useProofingBPC;
   pcl_bool p_defaultProofingEnabled;
   pcl_bool p_defaultGamutCheckEnabled;
   uint32   p_gamutWarningColor;

   friend class ColorManagementSetupProcess;
   friend class ColorManagementSetupInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorManagementSetupInstance_h

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupInstance.h - Released 2025-04-07T08:53:56Z
