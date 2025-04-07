//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "LocalHistogramEqualizationInterface.h"
#include "LocalHistogramEqualizationModule.h"
#include "LocalHistogramEqualizationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LocalHistogramEqualizationModule::LocalHistogramEqualizationModule()
{
}

// ----------------------------------------------------------------------------

const char* LocalHistogramEqualizationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString LocalHistogramEqualizationModule::Name() const
{
   return "LocalHistogramEqualization";
}

// ----------------------------------------------------------------------------

String LocalHistogramEqualizationModule::Description() const
{
   return "PixInsight LocalHistogramEqualization Process Module";
}

// ----------------------------------------------------------------------------

String LocalHistogramEqualizationModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String LocalHistogramEqualizationModule::Author() const
{
   return "Zbynek Vrastil (PTeam)";
}

// ----------------------------------------------------------------------------

String LocalHistogramEqualizationModule::Copyright() const
{
   return "Copyright (c) 2011-2020, Zbynek Vrastil";
}

// ----------------------------------------------------------------------------

String LocalHistogramEqualizationModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String LocalHistogramEqualizationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "LocalHistogramEqualization-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "LocalHistogramEqualization-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "LocalHistogramEqualization-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "LocalHistogramEqualization-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void LocalHistogramEqualizationModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::LocalHistogramEqualizationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::LocalHistogramEqualizationProcess;
      new pcl::LocalHistogramEqualizationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationModule.cpp - Released 2025-04-07T08:53:56Z
