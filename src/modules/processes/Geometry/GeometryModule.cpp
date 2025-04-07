//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// GeometryModule.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     4
#define MODULE_VERSION_REVISION  2
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2025
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       7

#include <pcl/AstrometricMetadata.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/View.h>

#include "ChannelMatchInterface.h"
#include "ChannelMatchProcess.h"
#include "CropInterface.h"
#include "CropProcess.h"
#include "DynamicCropInterface.h"
#include "DynamicCropProcess.h"
#include "FastRotationActions.h"
#include "FastRotationInterface.h"
#include "FastRotationProcess.h"
#include "GeometryModule.h"
#include "IntegerResampleInterface.h"
#include "IntegerResampleProcess.h"
#include "ResampleInterface.h"
#include "ResampleProcess.h"
#include "RotationInterface.h"
#include "RotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GeometryModule::GeometryModule()
{
}

// ----------------------------------------------------------------------------

const char* GeometryModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString GeometryModule::Name() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

String GeometryModule::Description() const
{
   return "PixInsight Standard Geometry Process Module";
}

// ----------------------------------------------------------------------------

String GeometryModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GeometryModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String GeometryModule::Copyright() const
{
   return "Copyright (c) 2005-2025 Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GeometryModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String GeometryModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Geometry-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Geometry-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Geometry-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Geometry-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void GeometryModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void GeometryModule::OnLoad()
{
   new Rotate180Action;
   new Rotate90CWAction;
   new Rotate90CCWAction;
   new HorizontalMirrorAction;
   new VerticalMirrorAction;
}

// ----------------------------------------------------------------------------

bool WarnOnAstrometryMetadataOrPreviewsOrMask( const ImageWindow& window, const IsoString& processId, bool noGUIMessages )
{
   bool hasAstrometry = window.HasAstrometricSolution();
   bool hasPreviews = window.HasPreviews();
   bool hasMaskReferences = window.HasMaskReferences();
   bool hasMask = !window.Mask().IsNull();

   if ( hasAstrometry || hasPreviews || hasMaskReferences || hasMask )
   {
      if ( !noGUIMessages )
         if ( MessageBox( "<p>" + window.MainView().Id() + "</p>"
                          "<p>The following items will be deleted as a result of the geometric transformation:</p>"
                          "<ul>"
                          + (hasAstrometry ?     "<li>Astrometric solution</li>" : "")
                          + (hasPreviews ?       "<li>Previews</li>" : "")
                          + (hasMaskReferences ? "<li>Mask references</li>" : "")
                          + (hasMask ?           "<li>Mask</li>" : "")
                          + "</ul>"
                          "<p><b>Some of these side effects could be irreversible. Proceed?</b></p>",
                          processId,
                          StdIcon::Warning,
                          StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
         {
            return false;
         }

      Console console;
      if ( hasAstrometry )
         console.WarningLn( "<end><cbr><br>** Warning: " + processId + ": Existing astrometric solution will be deleted." );
      if ( hasPreviews )
         console.WarningLn( "** Warning: " + processId + ": Existing previews will be deleted." );
      if ( hasMaskReferences )
         console.WarningLn( "** Warning: " + processId + ": Existing mask references will be deleted." );
      if ( hasMaskReferences )
         console.WarningLn( "** Warning: " + processId + ": Existing mask will be removed." );
   }

   return true;
}

// ----------------------------------------------------------------------------

void DeleteAstrometryMetadataAndPreviewsAndMask( ImageWindow& window,
                                                 bool deleteCenterMetadata, bool deleteScaleMetadata, double pixelSizeScalingFactor )
{
   DeleteAstrometryMetadataAndPreviews( window, deleteCenterMetadata, deleteScaleMetadata, pixelSizeScalingFactor );
   window.RemoveMaskReferences();
   window.RemoveMask();
}

// ----------------------------------------------------------------------------

void DeleteAstrometryMetadataAndPreviews( ImageWindow& window,
                                          bool deleteCenterMetadata, bool deleteScaleMetadata, double pixelSizeScalingFactor )
{
   window.ClearAstrometricSolution();

   FITSKeywordArray keywords = window.Keywords();
   AstrometricMetadata::RemoveKeywords( keywords, deleteCenterMetadata, deleteScaleMetadata );
   if ( !deleteScaleMetadata )
      if ( pixelSizeScalingFactor != 1 )
         AstrometricMetadata::RescalePixelSizeKeywords( keywords, pixelSizeScalingFactor );
   window.SetKeywords( keywords );

   AstrometricMetadata::RemoveProperties( window, deleteCenterMetadata, deleteScaleMetadata );
   if ( !deleteScaleMetadata )
      if ( pixelSizeScalingFactor != 1 )
         AstrometricMetadata::RescalePixelSizeProperties( window, pixelSizeScalingFactor );

   window.DeletePreviews();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GeometryModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::DynamicCropProcess;
      new pcl::DynamicCropInterface;
      new pcl::FastRotationProcess;
      new pcl::FastRotationInterface;
      new pcl::ChannelMatchProcess;
      new pcl::ChannelMatchInterface;
      new pcl::ResampleProcess;
      new pcl::ResampleInterface;
      new pcl::IntegerResampleProcess;
      new pcl::IntegerResampleInterface;
      new pcl::RotationProcess;
      new pcl::RotationInterface;
      new pcl::CropProcess;
      new pcl::CropInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GeometryModule.cpp - Released 2025-04-07T08:53:55Z
