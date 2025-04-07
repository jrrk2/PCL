//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 1.0.5
// ----------------------------------------------------------------------------
// RestorationFilterModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  5
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "RestorationFilterModule.h"
#include "RestorationFilterProcess.h"
#include "RestorationFilterInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RestorationFilterModule::RestorationFilterModule()
{
}

// ----------------------------------------------------------------------------

const char* RestorationFilterModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString RestorationFilterModule::Name() const
{
   return "RestorationFilters";
}

// ----------------------------------------------------------------------------

String RestorationFilterModule::Description() const
{
   return "PixInsight Standard RestorationFilters Process Module";
}

// ----------------------------------------------------------------------------

String RestorationFilterModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String RestorationFilterModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String RestorationFilterModule::Copyright() const
{
   return "Copyright (c) 2007-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String RestorationFilterModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String RestorationFilterModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "RestorationFilters-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "RestorationFilters-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "RestorationFilters-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "RestorationFilters-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void RestorationFilterModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::RestorationFilterModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::RestorationFilterProcess;
      new pcl::RestorationFilterInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF RestorationFilterModule.cpp - Released 2025-04-07T08:53:56Z
