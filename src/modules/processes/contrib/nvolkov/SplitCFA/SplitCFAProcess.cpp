//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SplitCFAInstance.h"
#include "SplitCFAInterface.h"
#include "SplitCFAParameters.h"
#include "SplitCFAProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFAProcess* TheSplitCFAProcess = nullptr;

// ----------------------------------------------------------------------------

SplitCFAProcess::SplitCFAProcess()
{
   TheSplitCFAProcess = this;

   new SplitCFATargetFrames( this );
   new SplitCFATargetFrameEnabled( TheTargetFramesParameter );
   new SplitCFATargetFramePath( TheTargetFramesParameter );
   new SplitCFATargetFrameFolder( TheTargetFramesParameter );
   new SplitCFAOutputTree( this );
   new SplitCFAOutputSubDirCFA( this );
   new SplitCFAOutputDir( this );
   new SplitCFAPrefix( this );
   new SplitCFAPostfix( this );
   new SplitCFAOverwrite( this );
   new SplitCFAOutputViewId0( this );
   new SplitCFAOutputViewId1( this );
   new SplitCFAOutputViewId2( this );
   new SplitCFAOutputViewId3( this );
}

// ----------------------------------------------------------------------------

IsoString SplitCFAProcess::Id() const
{
   return "SplitCFA";
}

// ----------------------------------------------------------------------------

IsoString SplitCFAProcess::Category() const
{
   return "ColorSpaces,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 SplitCFAProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SplitCFAProcess::Description() const
{
   return "<html>"
          "<p>Extracts 4 ([R][G][G][B]) subimages from a Bayer CFA to 4 files (names [0][1][2][3]).</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String SplitCFAProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SplitCFA.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SplitCFAProcess::DefaultInterface() const
{
   return TheSplitCFAInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* SplitCFAProcess::Create() const
{
   return new SplitCFAInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SplitCFAProcess::Clone( const ProcessImplementation& p ) const
{
   const SplitCFAInstance* instPtr = dynamic_cast<const SplitCFAInstance*>( &p );
   return (instPtr != nullptr) ? new SplitCFAInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF SplitCFAProcess.cpp - Released 2025-04-07T08:53:56Z
