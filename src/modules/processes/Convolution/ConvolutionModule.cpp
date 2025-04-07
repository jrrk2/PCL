//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     1
#define MODULE_VERSION_REVISION  3
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "ConvolutionInterface.h"
#include "ConvolutionModule.h"
#include "ConvolutionProcess.h"
#include "LarsonSekaninaInterface.h"
#include "LarsonSekaninaProcess.h"
#include "UnsharpMaskInterface.h"
#include "UnsharpMaskProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConvolutionModule::ConvolutionModule()
{
}

// ----------------------------------------------------------------------------

const char* ConvolutionModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ConvolutionModule::Name() const
{
   return "Convolution";
}

// ----------------------------------------------------------------------------

String ConvolutionModule::Description() const
{
   return "PixInsight Convolution Process Module"; // Replace with your own description
}

// ----------------------------------------------------------------------------

String ConvolutionModule::Company() const
{
   return "Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String ConvolutionModule::Author() const
{
   return "Convolution: Juan Conejero / Javier Sanchis Mu&ntilde;oz\n"
          "UnsharpMask: Juan Conejero / Carlos Milovic F.\n"
          "LarsonSekanina: Carlos Milovic F. / Juan Conejero";
}

// ----------------------------------------------------------------------------

String ConvolutionModule::Copyright() const
{
   return "Copyright (c) 2006-2025 Pleiades Astrophoto S.L.";
}

// ----------------------------------------------------------------------------

String ConvolutionModule::TradeMarks() const
{
   return "Pixinsight";
}

// ----------------------------------------------------------------------------

String ConvolutionModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Convolution-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Convolution-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Convolution-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Convolution-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ConvolutionModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::ConvolutionModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ConvolutionProcess;
      new pcl::ConvolutionInterface;
      new pcl::UnsharpMaskProcess;
      new pcl::UnsharpMaskInterface;
      new pcl::LarsonSekaninaProcess;
      new pcl::LarsonSekaninaInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ConvolutionModule.cpp - Released 2025-04-07T08:53:55Z
