//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  6
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "MergeCFAInterface.h"
#include "MergeCFAProcess.h"
#include "SplitCFAInterface.h"
#include "SplitCFAModule.h"
#include "SplitCFAProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFAModule::SplitCFAModule()
{
}

// ----------------------------------------------------------------------------

const char* SplitCFAModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString SplitCFAModule::Name() const
{
   return "SplitCFA";
}

// ----------------------------------------------------------------------------

String SplitCFAModule::Description() const
{
   return "SplitCFA module";
}

// ----------------------------------------------------------------------------

String SplitCFAModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String SplitCFAModule::Author() const
{
   return "Nikolay Volkov (PTeam)";
}

// ----------------------------------------------------------------------------

String SplitCFAModule::Copyright() const
{
   return "Copyright (c) 2013-2025 Nikolay Volkov";
}

// ----------------------------------------------------------------------------

String SplitCFAModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String SplitCFAModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "SplitCFA-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "SplitCFA-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "SplitCFA-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "SplitCFA-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void SplitCFAModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::SplitCFAModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::SplitCFAProcess;
      new pcl::SplitCFAInterface;
      new pcl::MergeCFAProcess;
      new pcl::MergeCFAInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF SplitCFAModule.cpp - Released 2025-04-07T08:53:56Z
