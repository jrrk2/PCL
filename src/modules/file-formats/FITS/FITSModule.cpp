//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// FITSModule.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "FITSModule.h"
#include "FITSFormat.h"

namespace pcl
{

// -------------------------------------------------------------------------

FITSModule::FITSModule()
{
}

// -------------------------------------------------------------------------

const char* FITSModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// -------------------------------------------------------------------------

IsoString FITSModule::Name() const
{
   return "FITS";
}

// -------------------------------------------------------------------------

String FITSModule::Description() const
{
   return "PixInsight Standard FITS File Format Module";
}

// -------------------------------------------------------------------------

String FITSModule::Company() const
{
   return "Pleiades Astrophoto";
}

// -------------------------------------------------------------------------

String FITSModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// -------------------------------------------------------------------------

String FITSModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// -------------------------------------------------------------------------

String FITSModule::TradeMarks() const
{
   return "PixInsight";
}

// -------------------------------------------------------------------------

String FITSModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "FITS-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "FITS-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "FITS-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "FITS-pxm.dll";
#endif
}

// -------------------------------------------------------------------------

void FITSModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::FITSModule;
   if ( mode == pcl::InstallMode::FullInstall )
      new pcl::FITSFormat;
   return 0;
}

// ----------------------------------------------------------------------------
// EOF FITSModule.cpp - Released 2025-04-07T08:53:45Z
