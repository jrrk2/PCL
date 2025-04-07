//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// CloneStampModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "CloneStampInterface.h"
#include "CloneStampModule.h"
#include "CloneStampProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CloneStampModule::CloneStampModule()
{
}

// ----------------------------------------------------------------------------

const char* CloneStampModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString CloneStampModule::Name() const
{
   return "CloneStamp";
}

// ----------------------------------------------------------------------------

String CloneStampModule::Description() const
{
   return "PixInsight Standard CloneStamp Process Module";
}

// ----------------------------------------------------------------------------

String CloneStampModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String CloneStampModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String CloneStampModule::Copyright() const
{
   return "Copyright (c) 2007-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String CloneStampModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String CloneStampModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "CloneStamp-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "CloneStamp-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "CloneStamp-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "CloneStamp-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void CloneStampModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::CloneStampModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::CloneStampProcess;
      new pcl::CloneStampInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF CloneStampModule.cpp - Released 2025-04-07T08:53:55Z
