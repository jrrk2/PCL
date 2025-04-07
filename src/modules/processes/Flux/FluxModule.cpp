//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// FluxModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  1
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "FluxModule.h"
#include "B3EProcess.h"
#include "B3EInterface.h"
#include "FluxCalibrationProcess.h"
#include "FluxCalibrationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FluxModule::FluxModule()
{
}

// ----------------------------------------------------------------------------

const char* FluxModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString FluxModule::Name() const
{
   return "Flux";
}

// ----------------------------------------------------------------------------

String FluxModule::Description() const
{
   return "PixInsight Flux Process Module";
}

// ----------------------------------------------------------------------------

String FluxModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String FluxModule::Author() const
{
   return "PTeam";
}

// ----------------------------------------------------------------------------

String FluxModule::Copyright() const
{
   return "Copyright (c) 2012-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String FluxModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String FluxModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Flux-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Flux-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Flux-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Flux-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void FluxModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::FluxModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::B3EProcess;
      new pcl::B3EInterface;
      new pcl::FluxCalibrationProcess;
      new pcl::FluxCalibrationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF FluxModule.cpp - Released 2025-04-07T08:53:55Z
