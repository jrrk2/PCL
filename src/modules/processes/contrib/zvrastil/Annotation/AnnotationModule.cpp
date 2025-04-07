//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationModule.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
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

#include "AnnotationInterface.h"
#include "AnnotationModule.h"
#include "AnnotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationModule::AnnotationModule()
{
}

// ----------------------------------------------------------------------------

const char* AnnotationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
      MODULE_VERSION_MINOR,
      MODULE_VERSION_REVISION,
      MODULE_VERSION_BUILD,
      MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString AnnotationModule::Name() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

String AnnotationModule::Description() const
{
   return "Annotation Process Module"; // Replace with your own description
}

// ----------------------------------------------------------------------------

String AnnotationModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String AnnotationModule::Author() const
{
   return "Zbynek Vrastil (PTeam)";
}

// ----------------------------------------------------------------------------

String AnnotationModule::Copyright() const
{
   return "Copyright (c) 2010-2025 Zbynek Vrastil";
}

// ----------------------------------------------------------------------------

String AnnotationModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String AnnotationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Annotation-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Annotation-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Annotation-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Annotation-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void AnnotationModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   new pcl::AnnotationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::AnnotationProcess;
      new pcl::AnnotationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF AnnotationModule.cpp - Released 2025-04-07T08:53:56Z
