//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// JPEG2000Module.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "JPEG2000Module.h"
#include "JPEG2000Format.h"

namespace pcl
{

// ----------------------------------------------------------------------------

JPEG2000Module::JPEG2000Module()
{
}

// -------------------------------------------------------------------------

const char* JPEG2000Module::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// -------------------------------------------------------------------------

IsoString JPEG2000Module::Name() const
{
   return "JPEG2000";
}

// -------------------------------------------------------------------------

String JPEG2000Module::Description() const
{
   return "PixInsight Standard JPEG2000 File Format Module";
}

// -------------------------------------------------------------------------

String JPEG2000Module::Company() const
{
   return "Pleiades Astrophoto";
}

// -------------------------------------------------------------------------

String JPEG2000Module::Author() const
{
   return "Juan Conejero, PTeam";
}

// -------------------------------------------------------------------------

String JPEG2000Module::Copyright() const
{
   return "Copyright (c) 2006-2025 Pleiades Astrophoto";
}

// -------------------------------------------------------------------------

String JPEG2000Module::TradeMarks() const
{
   return "PixInsight";
}

// -------------------------------------------------------------------------

String JPEG2000Module::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "JPEG2000-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "JPEG2000-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "JPEG2000-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "JPEG2000-pxm.dll";
#endif
}

// -------------------------------------------------------------------------

void JPEG2000Module::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::JPEG2000Module;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::JP2Format;
      new pcl::JPCFormat;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF JPEG2000Module.cpp - Released 2025-04-07T08:53:45Z
