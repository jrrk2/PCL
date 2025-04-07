//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ImageModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     3
#define MODULE_VERSION_REVISION  5
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "CreateAlphaChannelsInterface.h"
#include "CreateAlphaChannelsProcess.h"
#include "DynamicPSFInterface.h"
#include "DynamicPSFProcess.h"
#include "ExtractAlphaChannelsInterface.h"
#include "ExtractAlphaChannelsProcess.h"
#include "FITSHeaderAction.h"
#include "FITSHeaderInterface.h"
#include "FITSHeaderProcess.h"
#include "ImageIdentifierInterface.h"
#include "ImageIdentifierProcess.h"
#include "ImageModule.h"
#include "NewImageAction.h"
#include "NewImageInterface.h"
#include "NewImageProcess.h"
#include "SampleFormatConversionInterface.h"
#include "SampleFormatConversionProcess.h"
#include "StatisticsInterface.h"
#include "StatisticsProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

const char* ImageModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ImageModule::Name() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

String ImageModule::Description() const
{
   return "PixInsight Standard Image Process Module";
}

// ----------------------------------------------------------------------------

String ImageModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ImageModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String ImageModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String ImageModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String ImageModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Image-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Image-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Image-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Image-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ImageModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void ImageModule::OnLoad()
{
   new NewImageAction;
   new FITSHeaderAction;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ImageModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::NewImageProcess;
      new pcl::NewImageInterface;
      new pcl::ImageIdentifierProcess;
      new pcl::ImageIdentifierInterface;
      new pcl::SampleFormatConversionProcess;
      new pcl::SampleFormatConversionInterface;
      new pcl::CreateAlphaChannelsProcess;
      new pcl::CreateAlphaChannelsInterface;
      new pcl::ExtractAlphaChannelsProcess;
      new pcl::ExtractAlphaChannelsInterface;
      new pcl::FITSHeaderProcess;
      new pcl::FITSHeaderInterface;
      new pcl::StatisticsProcess;
      new pcl::StatisticsInterface;
      new pcl::DynamicPSFProcess;
      new pcl::DynamicPSFInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ImageModule.cpp - Released 2025-04-07T08:53:56Z
