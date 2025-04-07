//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// RGBWorkingSpaceProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RGBWorkingSpaceProcess.h"
#include "RGBWorkingSpaceParameters.h"
#include "RGBWorkingSpaceInstance.h"
#include "RGBWorkingSpaceInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RGBWorkingSpaceProcess*  TheRGBWorkingSpaceProcess = nullptr;

// ----------------------------------------------------------------------------

RGBWorkingSpaceProcess::RGBWorkingSpaceProcess()
{
   TheRGBWorkingSpaceProcess = this;

   new RGBWSChannelTable( this );
   new RGBWSLuminanceCoefficient( TheRGBWSChannelTableParameter );
   new RGBWSChromaticityX( TheRGBWSChannelTableParameter );
   new RGBWSChromaticityY( TheRGBWSChannelTableParameter );
   new RGBWSGamma( this );
   new RGBWSsRGBG( this );
   new RGBWSApplyGlobal( this );
}

// ----------------------------------------------------------------------------

IsoString RGBWorkingSpaceProcess::Id() const
{
   return "RGBWorkingSpace";
}

// ----------------------------------------------------------------------------

IsoString RGBWorkingSpaceProcess::Categories() const
{
   return "ColorSpaces";
}

// ----------------------------------------------------------------------------

String RGBWorkingSpaceProcess::Description() const
{
   return

   "<html>"
   "<p>Separating a color image into its luminance and chrominance components is a fundamental "
   "task of image processing. To calculate luminance and chrominance values, PixInsight uses a "
   "colorimetrically defined <i>RGB working space</i> (RGBWS). Such a RGBWS is defined by the "
   "following parameters:</p>"

   "<ul>"
   "<li><b>Luminance coefficients.</b> These are the relative weights of red, green and blue used "
   "to calculate the luminance of a pixel.<br/></li>"

   "<li><b>Chromaticity coordinates.</b> The x, y coordinates of the red, green and blue primaries. "
   "These colors are the fundamental colorants of the RGB color space.<br/></li>"

   "<li><b>Gamma.</b> A function used to linearize RGB components when converting them into a linear "
   "transformation space such as CIE XYZ. With the exception of the sRGB space, the gamma function is "
   "just the raise function (exponentiation), and the value of gamma is the exponent applied.<br/></li>"
   "</ul>"

   "<p>RGB Working Spaces in PixInsight have nothing to do with color management or ICC profiles. Color "
   "management comprises color spaces and transformations used to achieve consistent color through "
   "different imaging devices. A RGBWS is used strictly for pure image processing tasks. This differs "
   "from what is customary in most imaging applications.</p>"

   "<p>Each image window can use its own, <i>local RGBWS</i>. For images that don't have their own RGBWS "
   "(as happens by default), there exists a <i>global RGBWS</i>. By default, the global RGBWS is the "
   "sRGB space, mainly for compatibility with other applications. Note that sRGB is usually far from "
   "optimal for deep-sky astronomical images.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String RGBWorkingSpaceProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/RGBWorkingSpace.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* RGBWorkingSpaceProcess::DefaultInterface() const
{
   return TheRGBWorkingSpaceInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* RGBWorkingSpaceProcess::Create() const
{
   return new RGBWorkingSpaceInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* RGBWorkingSpaceProcess::Clone( const ProcessImplementation& p ) const
{
   const RGBWorkingSpaceInstance* instPtr = dynamic_cast<const RGBWorkingSpaceInstance*>( &p );
   return (instPtr != nullptr) ? new RGBWorkingSpaceInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceProcess.cpp - Released 2025-04-07T08:53:55Z
