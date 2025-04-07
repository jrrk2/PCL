//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.1
// ----------------------------------------------------------------------------
// CosmeticCorrectionModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     4
#define MODULE_VERSION_REVISION  1
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "CosmeticCorrectionModule.h"
#include "CosmeticCorrectionInterface.h"
#include "CosmeticCorrectionProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CosmeticCorrectionModule::CosmeticCorrectionModule()
{
}

// ----------------------------------------------------------------------------

const char* CosmeticCorrectionModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString CosmeticCorrectionModule::Name() const
{
   return "CosmeticCorrection";
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionModule::Description() const
{
   return "CosmeticCorrection module";
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionModule::Author() const
{
   return "Nikolay Volkov (PTeam)";
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionModule::Copyright() const
{
   return "Copyright (c) 2011-2025 Nikolay Volkov";
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String CosmeticCorrectionModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "CosmeticCorrection-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "CosmeticCorrection-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "CosmeticCorrection-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "CosmeticCorrection-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void CosmeticCorrectionModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::CosmeticCorrectionModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::CosmeticCorrectionProcess;
      new pcl::CosmeticCorrectionInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionModule.cpp - Released 2025-04-07T08:53:56Z
