//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "NetworkServiceInterface.h"
#include "NetworkServiceModule.h"
#include "NetworkServiceProcess.h"
#include "NetworkServiceTask.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NetworkServiceModule::NetworkServiceModule()
{
}

// ----------------------------------------------------------------------------

const char* NetworkServiceModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString NetworkServiceModule::Name() const
{
   return "NetworkService";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Description() const
{
   return "PixInsight NetworkService Process Module";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::Copyright() const
{
   return "Copyright (c) 2018-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String NetworkServiceModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "NetworkService-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "NetworkService-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "NetworkService-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "NetworkService-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void NetworkServiceModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void NetworkServiceModule::OnUnload()
{
   if ( TheNetworkServiceProcess != nullptr )
      while ( TheNetworkServiceProcess->IsRunning() )
      {
         ProcessEvents();
         TheNetworkServiceProcess->Stop();
         Sleep( 150 );
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::NetworkServiceModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      /*
       * Instantiate the task class(es) implemented in a custom version of the
       * NetworkService module (and remove the following instantiation of the
       * default NetworkServiceTask class).
       */
      new pcl::NetworkServiceTask;

      new pcl::NetworkServiceProcess;
      new pcl::NetworkServiceInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF NetworkServiceModule.cpp - Released 2025-04-07T08:53:56Z
