//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// MergeCFAProcess.cpp - Released 2025-04-07T08:53:56Z
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

#include "MergeCFAInterface.h"
#include "MergeCFAParameters.h"
#include "MergeCFAProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MergeCFAProcess* TheMergeCFAProcess = nullptr;

// ----------------------------------------------------------------------------

MergeCFAProcess::MergeCFAProcess()
{
   TheMergeCFAProcess = this;

   // Instantiate process parameters
   new MergeCFASourceImage0( this );
   new MergeCFASourceImage1( this );
   new MergeCFASourceImage2( this );
   new MergeCFASourceImage3( this );
   new MergeCFAOutputViewId( this );
}

// ----------------------------------------------------------------------------

IsoString MergeCFAProcess::Id() const
{
   return "MergeCFA";
}

// ----------------------------------------------------------------------------

IsoString MergeCFAProcess::Category() const
{
   return "ColorSpaces,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 MergeCFAProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String MergeCFAProcess::Description() const
{
   return "<html>"
          "<p>Combines one CFA Bayer ([R][G][G][B]) frame from subimages (names [0][1][2][3]).</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String MergeCFAProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/MergeCFA.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* MergeCFAProcess::DefaultInterface() const
{
   return TheMergeCFAInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* MergeCFAProcess::Create() const
{
   return new MergeCFAInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* MergeCFAProcess::Clone( const ProcessImplementation& p ) const
{
   const MergeCFAInstance* instPtr = dynamic_cast<const MergeCFAInstance*>( &p );
   return (instPtr != nullptr) ? new MergeCFAInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF MergeCFAProcess.cpp - Released 2025-04-07T08:53:56Z
