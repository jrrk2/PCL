//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.1.3
// ----------------------------------------------------------------------------
// XISFModule.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     1
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "XISFModule.h"
#include "XISFFormat.h"

namespace pcl
{

// -------------------------------------------------------------------------

const char* XISFModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString XISFModule::Name() const
{
   return "XISF";
}

// ----------------------------------------------------------------------------

String XISFModule::Description() const
{
   return "Extensible Image Serialization Format (XISF) - PixInsight Standard XISF File Format Module";
}

// ----------------------------------------------------------------------------

String XISFModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String XISFModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String XISFModule::Copyright() const
{
   return "Copyright (c) 2014-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String XISFModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String XISFModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "XISF-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "XISF-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "XISF-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "XISF-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void XISFModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// -------------------------------------------------------------------------

} // pcl

// -------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::XISFModule;
   if ( mode == pcl::InstallMode::FullInstall )
      new pcl::XISFFormat;
   return 0;
}

// ----------------------------------------------------------------------------
// EOF XISFModule.cpp - Released 2025-04-07T08:53:45Z
