//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// PixelMathModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     9
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "PixelMathInterface.h"
#include "PixelMathModule.h"
#include "PixelMathProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

PixelMathModule::PixelMathModule()
{
}

// ----------------------------------------------------------------------------

const char* PixelMathModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString PixelMathModule::Name() const
{
   return "PixelMath";
}

// ----------------------------------------------------------------------------

String PixelMathModule::Description() const
{
   return "PixInsight Standard PixelMath Process Module";
}

// ----------------------------------------------------------------------------

String PixelMathModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String PixelMathModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String PixelMathModule::Copyright() const
{
   return "Copyright (c) 2006-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String PixelMathModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String PixelMathModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "PixelMath-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "PixelMath-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "PixelMath-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "PixelMath-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void PixelMathModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::PixelMathModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::PixelMathProcess;
      new pcl::PixelMathInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF PixelMathModule.cpp - Released 2025-04-07T08:53:56Z
