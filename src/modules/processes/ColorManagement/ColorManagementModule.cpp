//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// ColorManagementModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#include "AssignICCProfileInterface.h"
#include "AssignICCProfileProcess.h"
#include "ColorManagementModule.h"
#include "ICCProfileTransformationInterface.h"
#include "ICCProfileTransformationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementModule::ColorManagementModule()
{
}

// ----------------------------------------------------------------------------

const char* ColorManagementModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ColorManagementModule::Name() const
{
   return "ColorManagement";
}

// ----------------------------------------------------------------------------

String ColorManagementModule::Description() const
{
   return "PixInsight Standard ColorManagement Process Module";
}

// ----------------------------------------------------------------------------

String ColorManagementModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ColorManagementModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String ColorManagementModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ColorManagementModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String ColorManagementModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "ColorManagement-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "ColorManagement-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ColorManagement-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ColorManagement-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ColorManagementModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::ColorManagementModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::AssignICCProfileProcess;
      new pcl::AssignICCProfileInterface;
      new pcl::ICCProfileTransformationProcess;
      new pcl::ICCProfileTransformationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ColorManagementModule.cpp - Released 2025-04-07T08:53:55Z
