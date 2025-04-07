//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGenerationModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     3
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "EphemerisGeneratorInterface.h"
#include "EphemerisGeneratorProcess.h"
#include "EphemerisGenerationModule.h"

namespace pcl
{

// ----------------------------------------------------------------------------

EphemerisGenerationModule::EphemerisGenerationModule()
{
}

// ----------------------------------------------------------------------------

const char* EphemerisGenerationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString EphemerisGenerationModule::Name() const
{
   return "EphemerisGeneration";
}

// ----------------------------------------------------------------------------

String EphemerisGenerationModule::Description() const
{
   return "PixInsight EphemerisGeneration Process Module";
}

// ----------------------------------------------------------------------------

String EphemerisGenerationModule::Company() const
{
   return "Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String EphemerisGenerationModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String EphemerisGenerationModule::Copyright() const
{
   return "Copyright (c) 2024-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String EphemerisGenerationModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String EphemerisGenerationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "EphemerisGeneration-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "EphemerisGeneration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "EphemerisGeneration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "EphemerisGeneration-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void EphemerisGenerationModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::EphemerisGenerationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::EphemerisGeneratorProcess;
      new pcl::EphemerisGeneratorInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF EphemerisGenerationModule.cpp - Released 2025-04-07T08:53:55Z
