//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.12.1
// ----------------------------------------------------------------------------
// DebayerModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     12
#define MODULE_VERSION_REVISION  1
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "DebayerModule.h"
#include "DebayerProcess.h"
#include "DebayerInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerModule::DebayerModule()
{
}

// ----------------------------------------------------------------------------

const char* DebayerModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString DebayerModule::Name() const
{
   return "Debayer";
}

// ----------------------------------------------------------------------------

String DebayerModule::Description() const
{
   return "PixInsight Debayer Module. In memory of Sander Pool.";
}

// ----------------------------------------------------------------------------

String DebayerModule::Company() const
{
   return "Tungsten Technologies / Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String DebayerModule::Author() const
{
   return "Sander Pool / Zbynek Vrastil / Juan Conejero";
}

// ----------------------------------------------------------------------------

String DebayerModule::Copyright() const
{
   return "Copyright (c) Sander Pool, 2009 / (c) Zbynek Vrastil, 2011 / (c) Pleiades Astrophoto, 2009-2025";
}

// ----------------------------------------------------------------------------

String DebayerModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String DebayerModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Debayer-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Debayer-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Debayer-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Debayer-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void DebayerModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::DebayerModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::DebayerProcess;
      new pcl::DebayerInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF DebayerModule.cpp - Released 2025-04-07T08:53:56Z
