//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SubframeSelectorInterface.h"
#include "SubframeSelectorMeasurementsInterface.h"
#include "SubframeSelectorModule.h"
#include "SubframeSelectorProcess.h"

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     9
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

namespace pcl
{

// ----------------------------------------------------------------------------

const char* SubframeSelectorModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorModule::Name() const
{
   return "SubframeSelector";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Description() const
{
   return
   "Facilitates subframe evaluation, selection and weighting based on several subframe \
   quality related measurements, including estimates of star profile <i>full width at \
   half maximum</i> (FWHM), star profile <i>eccentricity</i> and subframe \
   <i>signal to noise ratio weight</i>. Approved/rejected subframes may be copied/moved \
   to output directories for postprocessing. Subframe weights may be recorded in the \
   FITS header of the copies.";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Company() const
{
   return IsoString();
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Author() const
{
   return "Cameron Leger / Juan Conejero";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Copyright() const
{
   return "Copyright (c) 2017-2025, Cameron Leger / Juan Conejero";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "SubframeSelector-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "SubframeSelector-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "SubframeSelector-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "SubframeSelector-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void SubframeSelectorModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void SubframeSelectorModule::OnUnload()
{
   if ( TheSubframeSelectorCache != nullptr )
      if ( TheSubframeSelectorCache->IsPersistent() )
         if ( TheSubframeSelectorCache->IsModified() )
            TheSubframeSelectorCache->Save();

   if ( TheSubframeSelectorMeasurementsInterface != nullptr )
      TheSubframeSelectorMeasurementsInterface->Cleanup();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::SubframeSelectorModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::SubframeSelectorProcess;
      new pcl::SubframeSelectorInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF SubframeSelectorModule.cpp - Released 2025-04-07T08:53:56Z
