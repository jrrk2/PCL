//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.9
// ----------------------------------------------------------------------------
// CometAlignmentProcess.cpp - Released 2025-04-07T08:53:56Z
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

#include "CometAlignmentInstance.h"
#include "CometAlignmentInterface.h"
#include "CometAlignmentParameters.h"
#include "CometAlignmentProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CometAlignmentProcess* TheCometAlignmentProcess = nullptr;

// ----------------------------------------------------------------------------

CometAlignmentProcess::CometAlignmentProcess()
{
   TheCometAlignmentProcess = this;

   new CATargetFrames( this );
   new CATargetFramePath( TheCATargetFramesParameter );
   new CATargetFrameEnabled( TheCATargetFramesParameter );
   new CATargetFrameDate( TheCATargetFramesParameter );
   new CATargetFrameJDate( TheCATargetFramesParameter );
   new CATargetFrameX( TheCATargetFramesParameter );
   new CATargetFrameY( TheCATargetFramesParameter );
   new CATargetFrameFixed( TheCATargetFramesParameter );
   new CATargetFrameDrizzlePath( TheCATargetFramesParameter );

   new CAReferenceIndex( this );
   new CAFitPSF( this );
   new CAOperandImageFilePath( this );
   new CAOperandSubtractAligned( this );
   new CAOperandLinearFit( this );
   new CAOperandLinearFitLow( this );
   new CAOperandLinearFitHigh( this );
   new CAOperandNormalize( this );
   new CADrizzleWriteStarAlignedImage( this );
   new CADrizzleWriteCometAlignedImage( this );
   new CAPixelInterpolation( this );
   new CALinearClampingThreshold( this );

   new CAInputHints( this );
   new CAOutputHints( this );

   new CAOutputDirectory( this );
   new CAOutputExtension( this );
   new CAOutputPrefix( this );
   new CAOutputPostfix( this );
   new CAGenerateHistoryProperties( this );
   new CAOverwriteExistingFiles( this );
   new CAGenerateCometPathMap( this );
   new CAOnError( this );

   new CAUseFileThreads( this );
   new CAFileThreadOverload( this );
   new CAMaxFileReadThreads( this );
   new CAMaxFileWriteThreads( this );
}

// ----------------------------------------------------------------------------

IsoString CometAlignmentProcess::Id() const
{
   return "CometAlignment";
}

// ----------------------------------------------------------------------------

IsoString CometAlignmentProcess::Category() const
{
   return "ImageRegistration";
}

// ----------------------------------------------------------------------------

uint32 CometAlignmentProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String CometAlignmentProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String CometAlignmentProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/CometAlignment.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* CometAlignmentProcess::DefaultInterface() const
{
   return TheCometAlignmentInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* CometAlignmentProcess::Create() const
{
   return new CometAlignmentInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* CometAlignmentProcess::Clone( const ProcessImplementation& p ) const
{
   const CometAlignmentInstance* instPtr = dynamic_cast<const CometAlignmentInstance*>( &p );
   return (instPtr != nullptr) ? new CometAlignmentInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF CometAlignmentProcess.cpp - Released 2025-04-07T08:53:56Z
