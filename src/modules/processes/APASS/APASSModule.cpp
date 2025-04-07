//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.5
// ----------------------------------------------------------------------------
// APASSModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     1
#define MODULE_VERSION_REVISION  5
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "APASSInterface.h"
#include "APASSModule.h"
#include "APASSParameters.h"
#include "APASSProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

APASSModule::APASSModule()
{
}

// ----------------------------------------------------------------------------

const char* APASSModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString APASSModule::Name() const
{
   return "APASS";
}

// ----------------------------------------------------------------------------

String APASSModule::Description() const
{
   return "PixInsight Standard APASS Process Module";
}

// ----------------------------------------------------------------------------

String APASSModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String APASSModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String APASSModule::Copyright() const
{
   return "Copyright (c) 2022-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String APASSModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String APASSModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "APASS-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "APASS-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "APASS-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "APASS-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void APASSModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void APASSModule::OnUnload()
{
   if ( TheAPASSProcess != nullptr ) // cannot be false
   {
      if ( TheAPASSProcess->PreferencesLoaded( ADataRelease::DR9 ) )
         TheAPASSProcess->SavePreferences( ADataRelease::DR9 );
      if ( TheAPASSProcess->PreferencesLoaded( ADataRelease::DR10 ) )
         TheAPASSProcess->SavePreferences( ADataRelease::DR10 );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::APASSModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::APASSProcess;
      new pcl::APASSInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF APASSModule.cpp - Released 2025-04-07T08:53:55Z
