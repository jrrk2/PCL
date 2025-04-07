//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 1.0.5
// ----------------------------------------------------------------------------
// RestorationFilterProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RestorationFilterInstance.h"
#include "RestorationFilterInterface.h"
#include "RestorationFilterParameters.h"
#include "RestorationFilterProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RestorationFilterProcess* TheRestorationFilterProcess = nullptr;

// ----------------------------------------------------------------------------

RestorationFilterProcess::RestorationFilterProcess()
{
   TheRestorationFilterProcess = this;

   new RFWienerK( this );
   new RFLeastSquaresGamma( this );
   new RFAlgorithm( this );
   new RFPSFMode( this );
   new RFPSFParametricSigma( this );
   new RFPSFParametricShape( this );
   new RFPSFParametricAspectRatio( this );
   new RFPSFParametricRotationAngle( this );
   new RFPSFMotionLength( this );
   new RFPSFMotionRotationAngle( this );
   new RFPSFViewId( this );
   new RFAmount( this );
   new RFDeringing( this );
   new RFDeringingDark( this );
   new RFDeringingBright( this );
   new RFOutputDeringingMaps( this );
   new RFToLuminance( this );
   new RFLinear( this );
   new RFRangeLow( this );
   new RFRangeHigh( this );
}

// ----------------------------------------------------------------------------

IsoString RestorationFilterProcess::Id() const
{
   return "RestorationFilter";
}

// ----------------------------------------------------------------------------

IsoString RestorationFilterProcess::Category() const
{
   return "Deconvolution";
}

// ----------------------------------------------------------------------------

uint32 RestorationFilterProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String RestorationFilterProcess::Description() const
{
   return

   "<html>"
   "<p>RestorationFilter implements some non-iterative image deconvolution algorithms in the frequency domain.</p>"

   "<p>The two algorithms currently available are Wiener filtering (or Minimum Mean Square Error filtering) and "
   "Constrained Least Squares filtering.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String RestorationFilterProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/RestorationFilter.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* RestorationFilterProcess::DefaultInterface() const
{
   return TheRestorationFilterInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* RestorationFilterProcess::Create() const
{
   return new RestorationFilterInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* RestorationFilterProcess::Clone( const ProcessImplementation& p ) const
{
   const RestorationFilterInstance* instPtr = dynamic_cast<const RestorationFilterInstance*>( &p );
   return (instPtr != nullptr) ? new RestorationFilterInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RestorationFilterProcess.cpp - Released 2025-04-07T08:53:56Z
