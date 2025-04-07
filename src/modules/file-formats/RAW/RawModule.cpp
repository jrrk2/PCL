//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawModule.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     6
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "RawModule.h"
#include "RawFormat.h"

namespace pcl
{

// ----------------------------------------------------------------------------

const char* RawModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString RawModule::Name() const
{
   return "RAW";
}

// ----------------------------------------------------------------------------

String RawModule::Description() const
{
   return "PixInsight Standard RAW File Format Module";
}

// ----------------------------------------------------------------------------

String RawModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String RawModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String RawModule::Copyright() const
{
   return "Copyright (c) 2006-2025, Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String RawModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String RawModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "RAW-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "RAW-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "RAW-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "RAW-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void RawModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// -------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::RawModule;
   if ( mode == pcl::InstallMode::FullInstall )
      new pcl::RawFormat;
   return 0;
}

// ----------------------------------------------------------------------------
// EOF RawModule.cpp - Released 2025-04-07T08:53:45Z
