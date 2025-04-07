//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// NoiseGenerationModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
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

#include "NoiseGenerationModule.h"
#include "NoiseGeneratorInterface.h"
#include "NoiseGeneratorProcess.h"
#include "SimplexNoiseInterface.h"
#include "SimplexNoiseProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NoiseGenerationModule::NoiseGenerationModule()
{
}

// ----------------------------------------------------------------------------

const char* NoiseGenerationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString NoiseGenerationModule::Name() const
{
   return "NoiseGeneration";
}

// ----------------------------------------------------------------------------

String NoiseGenerationModule::Description() const
{
   return "PixInsight Standard NoiseGeneration Process Module";
}

// ----------------------------------------------------------------------------

String NoiseGenerationModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String NoiseGenerationModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String NoiseGenerationModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String NoiseGenerationModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String NoiseGenerationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "NoiseGeneration-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "NoiseGeneration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "NoiseGeneration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "NoiseGeneration-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void NoiseGenerationModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::NoiseGenerationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::NoiseGeneratorProcess;
      new pcl::NoiseGeneratorInterface;
      new pcl::SimplexNoiseProcess;
      new pcl::SimplexNoiseInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF NoiseGenerationModule.cpp - Released 2025-04-07T08:53:56Z
