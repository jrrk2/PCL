//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIClientModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
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

#include "INDICCDFrameInterface.h"
#include "INDICCDFrameProcess.h"
#include "INDIClientModule.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIDeviceControllerProcess.h"
#include "INDIMountInterface.h"
#include "INDIMountProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

INDIClientModule::INDIClientModule()
{
}

// ----------------------------------------------------------------------------

const char* INDIClientModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString INDIClientModule::Name() const
{
   return "INDIClient";
}

// ----------------------------------------------------------------------------

String INDIClientModule::Description() const
{
   return "PixInsight INDIClient Process Module";
}

// ----------------------------------------------------------------------------

String INDIClientModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String INDIClientModule::Author() const
{
   return "Klaus Kretzschmar (PTeam), Juan Conejero (PTeam)";
}

// ----------------------------------------------------------------------------

String INDIClientModule::Copyright() const
{
   return "Copyright (c) 2014-2025 Klaus Kretzschmar";
}

// ----------------------------------------------------------------------------

String INDIClientModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String INDIClientModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "INDIClient-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "INDIClient-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "INDIClient-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "INDIClient-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void INDIClientModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::INDIClientModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::INDIDeviceControllerProcess;
      new pcl::INDIDeviceControllerInterface;
      new pcl::INDICCDFrameProcess;
      new pcl::INDICCDFrameInterface;
      new pcl::INDIMountProcess;
      new pcl::INDIMountInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF INDIClientModule.cpp - Released 2025-04-07T08:53:56Z
