//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ColorSpacesModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     2
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "ColorSpacesModule.h"
#include "GrayscaleRGBProcesses.h"
#include "GrayscaleRGBActions.h"
#include "ChannelExtractionProcess.h"
#include "ChannelExtractionInterface.h"
#include "ChannelCombinationProcess.h"
#include "ChannelCombinationInterface.h"
#include "RGBWorkingSpaceProcess.h"
#include "RGBWorkingSpaceInterface.h"
#include "LRGBCombinationProcess.h"
#include "LRGBCombinationInterface.h"
#include "ExtractCIELAction.h"
#include "SplitRGBChannelsAction.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSpacesModule::ColorSpacesModule()
{
}

// ----------------------------------------------------------------------------

const char* ColorSpacesModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ColorSpacesModule::Name() const
{
   return "ColorSpaces";
}

// ----------------------------------------------------------------------------

String ColorSpacesModule::Description() const
{
   return "PixInsight Standard ColorSpaces Module";
}

// ----------------------------------------------------------------------------

String ColorSpacesModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ColorSpacesModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String ColorSpacesModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ColorSpacesModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String ColorSpacesModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "ColorSpaces-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "ColorSpaces-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ColorSpaces-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ColorSpaces-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ColorSpacesModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void ColorSpacesModule::OnLoad()
{
   new ConvertToGrayscaleAction;
   new ConvertToRGBColorAction;
   new ExtractCIELAction;
   new SplitRGBChannelsAction;
   new ExtractRedChannelAction;
   new ExtractGreenChannelAction;
   new ExtractBlueChannelAction;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ColorSpacesModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ConvertToGrayscaleProcess;
      new pcl::ConvertToRGBColorProcess;
      new pcl::ChannelExtractionProcess;
      new pcl::ChannelExtractionInterface;
      new pcl::ChannelCombinationProcess;
      new pcl::ChannelCombinationInterface;
      new pcl::RGBWorkingSpaceProcess;
      new pcl::RGBWorkingSpaceInterface;
      new pcl::LRGBCombinationProcess;
      new pcl::LRGBCombinationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ColorSpacesModule.cpp - Released 2025-04-07T08:53:55Z
