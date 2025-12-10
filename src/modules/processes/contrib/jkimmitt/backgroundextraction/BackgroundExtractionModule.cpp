// ****************************************************************************
// PixInsight Background Extraction Module
// ****************************************************************************
// BackgroundExtractionModule.cpp - Module initialization and registration
// ****************************************************************************

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       10

#include "BackgroundExtractionModule.h"
#include "BackgroundExtractionProcess.h"
#include "BackgroundExtractionInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BackgroundExtractionModule* TheBackgroundExtractionModule = nullptr;

// ----------------------------------------------------------------------------

BackgroundExtractionModule::BackgroundExtractionModule()
{
}

// ----------------------------------------------------------------------------

BackgroundExtractionModule::~BackgroundExtractionModule()
{
}

// ----------------------------------------------------------------------------

const char* BackgroundExtractionModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString BackgroundExtractionModule::Name() const
{
   return "BackgroundExtraction";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionModule::Description() const
{
   return "PixInsight Background Extraction Module - "
          "Advanced background modeling and extraction using gradient domain methods";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionModule::Company() const
{
   return "Your Company Name";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionModule::Author() const
{
   return "Your Name";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionModule::Copyright() const
{
   return "Copyright (c) 2025 Your Name";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String BackgroundExtractionModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "BackgroundExtraction-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "BackgroundExtraction-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "BackgroundExtraction-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "BackgroundExtraction-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void BackgroundExtractionModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// Module Installation/Deinstallation Routines
// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::BackgroundExtractionModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::BackgroundExtractionProcess;
      new pcl::BackgroundExtractionInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
