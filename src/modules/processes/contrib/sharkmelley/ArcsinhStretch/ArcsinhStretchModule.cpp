//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "ArcsinhStretchInterface.h"
#include "ArcsinhStretchModule.h"
#include "ArcsinhStretchProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretchModule::ArcsinhStretchModule()
{
}

// ----------------------------------------------------------------------------

const char* ArcsinhStretchModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchModule::Name() const
{
   return "ArcsinhStretch";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Description() const
{
   return "ArcsinhStretch module. Apply hyperbolic arcsine stretch.";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Author() const
{
   return "Mark Shelley";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Copyright() const
{
   return "Copyright (c) 2017-2025 Mark Shelley";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "ArcsinhStretch-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "ArcsinhStretch-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ArcsinhStretch-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ArcsinhStretch-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ArcsinhStretchModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::ArcsinhStretchModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ArcsinhStretchProcess;
      new pcl::ArcsinhStretchInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchModule.cpp - Released 2025-04-07T08:53:56Z
