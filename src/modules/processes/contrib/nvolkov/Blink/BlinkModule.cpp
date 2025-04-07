//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// BlinkModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  5
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "BlinkModule.h"
#include "BlinkProcess.h"
#include "BlinkInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BlinkModule::BlinkModule()
{
}

// ----------------------------------------------------------------------------

const char* BlinkModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString BlinkModule::Name() const
{
   return "Blink";
}

// ----------------------------------------------------------------------------

String BlinkModule::Description() const
{
   return "Blink module by Nikolay Volkov - Animates a set of images like a movie.";
}

// ----------------------------------------------------------------------------

String BlinkModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String BlinkModule::Author() const
{
   return "Nikolay Volkov (PTeam)";
}

// ----------------------------------------------------------------------------

String BlinkModule::Copyright() const
{
   return "Copyright (C) 2011-2025 Nikolay Volkov";
}

// ----------------------------------------------------------------------------

String BlinkModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String BlinkModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Blink-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Blink-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Blink-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Blink-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void BlinkModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::BlinkModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::BlinkProcess;
      new pcl::BlinkInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF BlinkModule.cpp - Released 2025-04-07T08:53:56Z
