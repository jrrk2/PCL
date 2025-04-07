//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "FindingChartModule.h"
#include "FindingChartProcess.h"
#include "FindingChartInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FindingChartModule::FindingChartModule()
{
}

// ----------------------------------------------------------------------------

const char* FindingChartModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString FindingChartModule::Name() const
{
   return "FindingChart";
}

// ----------------------------------------------------------------------------

String FindingChartModule::Description() const
{
   return "PixInsight FindingChart Process Module";
}

// ----------------------------------------------------------------------------

String FindingChartModule::Company() const
{
   return "Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String FindingChartModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String FindingChartModule::Copyright() const
{
   return "Copyright (c) 2022-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String FindingChartModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String FindingChartModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "FindingChart-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "FindingChart-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "FindingChart-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "FindingChart-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void FindingChartModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::FindingChartModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::FindingChartProcess;
      new pcl::FindingChartInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF FindingChartModule.cpp - Released 2025-04-07T08:53:55Z
