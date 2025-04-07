//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDICCDFrameProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "INDICCDFrameProcess.h"
#include "INDICCDFrameInstance.h"
#include "INDICCDFrameInterface.h"
#include "INDICCDFrameParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDICCDFrameProcess* TheINDICCDFrameProcess = nullptr;

// ----------------------------------------------------------------------------

INDICCDFrameProcess::INDICCDFrameProcess()
{
   TheINDICCDFrameProcess = this;

   new ICFDeviceName( this );
   new ICFUploadMode( this );
   new ICFServerUploadDirectory( this );
   new ICFServerFileNameTemplate( this );
   new ICFFrameType( this );
   new ICFBinningX( this );
   new ICFBinningY( this );
   new ICFFilterSlot( this );
   new ICFExposureTime( this );
   new ICFExposureDelay( this );
   new ICFExposureCount( this );
   new ICFOpenClientImages( this );
   new ICFNewImageIdTemplate( this );
   new ICFReuseImageWindow( this );
   new ICFAutoStretch( this );
   new ICFLinkedAutoStretch( this );
   new ICFSaveClientImages( this );
   new ICFOverwriteClientImages( this );
   new ICFClientDownloadDirectory( this );
   new ICFClientFileNameTemplate( this );
   new ICFClientOutputFormatHints( this );
   new ICFObjectName( this );
   new ICFTelescopeSelection( this );
   new ICFRequireSelectedTelescope( this );
   new ICFTelescopeDeviceName( this );
   new ICFExternalFilterWheelDeviceName( this );
   new ICFEnableAlignmentCorrection( this );
   new ICFAlignmentFile( this );
   new ICFClientFrames( this );
   new ICFClientViewId( TheICFClientFramesParameter );
   new ICFClientFilePath( TheICFClientFramesParameter );
   new ICFServerFrames( this );
   new ICFServerFrame( TheICFServerFramesParameter );
   new ICFTelescopeFocalLength( this );
   new ICFApplyPlateSolver( this );
   new ICFCenterTarget( this );
   new ICFServerURL( this );
   new ICFSolverCatalogName( this );
   new ICFSolverAutoCatalog( this );
   new ICFSolverLimitMagnitude( this );
   new ICFSolverAutoLimitMagnitude( this );
   new ICFSolverStarSensitivity( this );
   new ICFSolverNoiseLayers( this );
   new ICFSolverAlignmentDevice( this );
   new ICFSolverDistortionCorrection( this );
   new ICFSolverSplineSmoothing( this );
   new ICFSolverProjection( this );
   new ICFCCDMode( this );
}

// ----------------------------------------------------------------------------

IsoString INDICCDFrameProcess::Id() const
{
   return "IndigoCCDFrame";
}

// ----------------------------------------------------------------------------

IsoString INDICCDFrameProcess::Category() const
{
   return "Instrumentation";
}

// ----------------------------------------------------------------------------

uint32 INDICCDFrameProcess::Version() const
{
   return 0x200;
}

// ----------------------------------------------------------------------------

String INDICCDFrameProcess::Description() const
{
   return "<html>"
          "<p>Acquire frames from Indigo CCD devices.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String INDICCDFrameProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/IndigoCCDFrame.svg";
}

// ----------------------------------------------------------------------------

bool INDICCDFrameProcess::PrefersGlobalExecution() const
{
   return true;
}
// ----------------------------------------------------------------------------

ProcessInterface* INDICCDFrameProcess::DefaultInterface() const
{
   return TheINDICCDFrameInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDICCDFrameProcess::Create() const
{
   return new INDICCDFrameInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDICCDFrameProcess::Clone( const ProcessImplementation& p ) const
{
   const INDICCDFrameInstance* r = dynamic_cast<const INDICCDFrameInstance*>( &p );
   return (r != nullptr) ? new INDICCDFrameInstance( *r ) : nullptr;
}

// ----------------------------------------------------------------------------

bool INDICCDFrameProcess::CanProcessCommandLines() const
{
   // ### TODO
   return false;
}

// ----------------------------------------------------------------------------

#if 0
static void ShowHelp()
{
   Console().Write(
"<raw>"
"Nothing to show."
"</raw>" );
}
#endif

int INDICCDFrameProcess::ProcessCommandLine( const StringList& argv ) const
{
   // ### TODO
   return 0;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameProcess.cpp - Released 2025-04-07T08:53:56Z
