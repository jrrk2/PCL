//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// BMPModule.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  5
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "BMPModule.h"
#include "BMPFormat.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BMPModule::BMPModule()
{
}

// -------------------------------------------------------------------------

const char* BMPModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// -------------------------------------------------------------------------

IsoString BMPModule::Name() const
{
   return "BMP";
}

// -------------------------------------------------------------------------

String BMPModule::Description() const
{
   return "PixInsight Standard BMP File Format Module";
}

// -------------------------------------------------------------------------

String BMPModule::Company() const
{
   return "Pleiades Astrophoto";
}

// -------------------------------------------------------------------------

String BMPModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// -------------------------------------------------------------------------

String BMPModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// -------------------------------------------------------------------------

String BMPModule::TradeMarks() const
{
   return "PixInsight";
}

// -------------------------------------------------------------------------

String BMPModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "BMP-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "BMP-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "BMP-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "BMP-pxm.dll";
#endif
}

// -------------------------------------------------------------------------

void BMPModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// -------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::BMPModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::BMPFormat;
      new pcl::GIFFormat;
      new pcl::ICOFormat;
      new pcl::MNGFormat;
      new pcl::PBMFormat;
      new pcl::PGMFormat;
      new pcl::PNGFormat;
      new pcl::PPMFormat;
      new pcl::XBMFormat;
      new pcl::XPMFormat;
      new pcl::TGAFormat;
      new pcl::WEBPFormat;
      new pcl::SVGFormat;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF BMPModule.cpp - Released 2025-04-07T08:53:45Z
