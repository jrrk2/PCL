//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.9
// ----------------------------------------------------------------------------
// CometAlignmentModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     3
#define MODULE_VERSION_REVISION  9
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include "CometAlignmentModule.h"
#include "CometAlignmentInterface.h"
#include "CometAlignmentProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CometAlignmentModule::CometAlignmentModule()
{
}

// ----------------------------------------------------------------------------

const char* CometAlignmentModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString CometAlignmentModule::Name() const
{
   return "CometAlignment";
}

// ----------------------------------------------------------------------------

String CometAlignmentModule::Description() const
{
   return "PixInsight CometAlignment module";
}

// ----------------------------------------------------------------------------

String CometAlignmentModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String CometAlignmentModule::Author() const
{
   return "Nikolay Volkov / Juan Conejero (PTeam)";
}

// ----------------------------------------------------------------------------

String CometAlignmentModule::Copyright() const
{
   return "Copyright (c) 2012-2025 Nikolay Volkov / 2019-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String CometAlignmentModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String CometAlignmentModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "CometAlignment-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "CometAlignment-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "CometAlignment-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "CometAlignment-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void CometAlignmentModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::CometAlignmentModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::CometAlignmentProcess;
      new pcl::CometAlignmentInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF CometAlignmentModule.cpp - Released 2025-04-07T08:53:56Z
