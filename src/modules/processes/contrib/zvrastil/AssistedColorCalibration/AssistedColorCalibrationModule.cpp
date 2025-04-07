//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AssistedColorCalibrationModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "AssistedColorCalibrationInterface.h"
#include "AssistedColorCalibrationModule.h"
#include "AssistedColorCalibrationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AssistedColorCalibrationModule::AssistedColorCalibrationModule()
{
}

// ----------------------------------------------------------------------------

const char* AssistedColorCalibrationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString AssistedColorCalibrationModule::Name() const
{
   return "AssistedColorCalibration";
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationModule::Description() const
{
   return "AssistedColorCalibration Process Module"; // Replace with your own description
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationModule::Author() const
{
   return "Zbynek Vrastil (PTeam)";
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationModule::Copyright() const
{
   return "Copyright (c) 2010-2025 Zbynek Vrastil";
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "AssistedColorCalibration-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "AssistedColorCalibration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "AssistedColorCalibration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "AssistedColorCalibration-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::AssistedColorCalibrationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::AssistedColorCalibrationProcess;
      new pcl::AssistedColorCalibrationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationModule.cpp - Released 2025-04-07T08:53:56Z
