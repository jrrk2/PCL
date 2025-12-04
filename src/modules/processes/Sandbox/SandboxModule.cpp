//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "SandboxModule.h"
#include "SandboxProcess.h"
#include "SandboxInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxModule::SandboxModule()
{
}

// ----------------------------------------------------------------------------

const char* SandboxModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString SandboxModule::Name() const
{
   /*
    * Replace with the actual name of this module. Must be unique.
    */
   return "Sandbox";
}

// ----------------------------------------------------------------------------

String SandboxModule::Description() const
{
   /*
    * Replace with a short (one-line) description of this module.
    */
   return "PixInsight Sandbox Process Module";
}

// ----------------------------------------------------------------------------

String SandboxModule::Company() const
{
   /*
    * Replace with the name of the company that has developed this module, or
    * return an empty String() if not applicable.
    */
   return "Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String SandboxModule::Author() const
{
   /*
    * Comma-separated list of module authors.
    * Replace with the name(s) of the actual author(s) of this module.
    */
   return "PTeam";
}

// ----------------------------------------------------------------------------

String SandboxModule::Copyright() const
{
   /*
    * Replace with the actual copyright information.
    */
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String SandboxModule::TradeMarks() const
{
   /*
    * Comma-separated list of trade marks.
    * PixInsight should always be included in the list.
    */
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String SandboxModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Sandbox-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Sandbox-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Sandbox-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Sandbox-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void SandboxModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

/*
 * Module installation routine.
 *
 * If this routine is defined as a public symbol in a module, the PixInsight
 * core application will call it just after loading and initializing the module
 * shared object or dynamic-link library.
 *
 * The mode argument specifies the kind of installation being performed by the
 * core application. See the pcl::InstallMode namespace for more information.
 */
PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   /*
    * When the PixInsight application installs this module, we just have to
    * instantiate the meta objects describing it.
    */
   new pcl::SandboxModule;

   /*
    * The mode argument tells us which kind of installation is being requested
    * by the PixInsight application. Incomplete installation requests only need
    * module descriptions.
    */
   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::SandboxProcess;
      new pcl::SandboxInterface;
   }

   /*
    * Return zero to signal successful installation
    */
   return 0;
}

// ----------------------------------------------------------------------------
// EOF SandboxModule.cpp - Released 2025-04-07T08:53:56Z
