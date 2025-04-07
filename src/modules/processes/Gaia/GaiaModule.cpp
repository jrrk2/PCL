//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Gaia Process Module Version 1.2.6
// ----------------------------------------------------------------------------
// GaiaModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Gaia PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  6
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "GaiaInterface.h"
#include "GaiaModule.h"
#include "GaiaParameters.h"
#include "GaiaProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GaiaModule::GaiaModule()
{
}

// ----------------------------------------------------------------------------

const char* GaiaModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString GaiaModule::Name() const
{
   return "Gaia";
}

// ----------------------------------------------------------------------------

String GaiaModule::Description() const
{
   return "PixInsight Standard Gaia Process Module";
}

// ----------------------------------------------------------------------------

String GaiaModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GaiaModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String GaiaModule::Copyright() const
{
   return "Copyright (c) 2022-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GaiaModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String GaiaModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Gaia-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Gaia-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Gaia-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Gaia-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void GaiaModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void GaiaModule::OnUnload()
{
   if ( TheGaiaProcess != nullptr ) // cannot be false
   {
      if ( TheGaiaProcess->PreferencesLoaded( GDataRelease::DR2 ) )
         TheGaiaProcess->SavePreferences( GDataRelease::DR2 );
      if ( TheGaiaProcess->PreferencesLoaded( GDataRelease::EDR3 ) )
         TheGaiaProcess->SavePreferences( GDataRelease::EDR3 );
      if ( TheGaiaProcess->PreferencesLoaded( GDataRelease::DR3 ) )
         TheGaiaProcess->SavePreferences( GDataRelease::DR3 );
      if ( TheGaiaProcess->PreferencesLoaded( GDataRelease::DR3SP ) )
         TheGaiaProcess->SavePreferences( GDataRelease::DR3SP );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GaiaModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::GaiaProcess;
      new pcl::GaiaInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GaiaModule.cpp - Released 2025-04-07T08:53:55Z
