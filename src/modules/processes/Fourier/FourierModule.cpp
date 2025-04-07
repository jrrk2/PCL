//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  4
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "FourierModule.h"
#include "FourierTransformProcess.h"
#include "FourierTransformInterface.h"
#include "InverseFourierTransformProcess.h"
#include "InverseFourierTransformInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FourierModule::FourierModule()
{
}

// ----------------------------------------------------------------------------

const char* FourierModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString FourierModule::Name() const
{
   return "Fourier";
}

// ----------------------------------------------------------------------------

String FourierModule::Description() const
{
   return "PixInsight Standard Fourier Process Module";
}

// ----------------------------------------------------------------------------

String FourierModule::Company() const
{
   return "Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String FourierModule::Author() const
{
   return "Juan Conejero, PTeam / Carlos Milovic F., PTeam";
}

// ----------------------------------------------------------------------------

String FourierModule::Copyright() const
{
   return "Copyright (c) 2011-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String FourierModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String FourierModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Fourier-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Fourier-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Fourier-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Fourier-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void FourierModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::FourierModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::FourierTransformProcess;
      new pcl::FourierTransformInterface;
      new pcl::InverseFourierTransformProcess;
      new pcl::InverseFourierTransformInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF FourierModule.cpp - Released 2025-04-07T08:53:55Z
