//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GREYCstorationModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "GREYCstorationInterface.h"
#include "GREYCstorationModule.h"
#include "GREYCstorationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GREYCstorationModule::GREYCstorationModule()
{
}

// ----------------------------------------------------------------------------

const char* GREYCstorationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString GREYCstorationModule::Name() const
{
   return "GREYCstoration";
}

// ----------------------------------------------------------------------------

String GREYCstorationModule::Description() const
{
   return "PixInsight GREYCstoration Process Module";
}

// ----------------------------------------------------------------------------

String GREYCstorationModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GREYCstorationModule::Author() const
{
   return "CImg: David Tschumperl&eacute; / PCL module: Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String GREYCstorationModule::Copyright() const
{
   return "CImg: Copyright (c) David Tschumperl&eacute; / PCL: Copyright (c) 2003-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GREYCstorationModule::TradeMarks() const
{
   return "GREYCstoration, CImg, PixInsight";
}

// ----------------------------------------------------------------------------

String GREYCstorationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "GREYCstoration-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "GREYCstoration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "GREYCstoration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "GREYCstoration-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void GREYCstorationModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::GREYCstorationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::GREYCstorationProcess;
      new pcl::GREYCstorationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GREYCstorationModule.cpp - Released 2025-04-07T08:53:56Z
