//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// GrayscaleRGBProcesses.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "GrayscaleRGBProcesses.h"
#include "GrayscaleRGBInstances.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConvertToGrayscaleProcess* TheConvertToGrayscaleProcess = nullptr;

// ----------------------------------------------------------------------------

ConvertToGrayscaleProcess::ConvertToGrayscaleProcess()
{
   TheConvertToGrayscaleProcess = this;
}

// ----------------------------------------------------------------------------

IsoString ConvertToGrayscaleProcess::Id() const
{
   return "ConvertToGrayscale";
}

// ----------------------------------------------------------------------------

IsoString ConvertToGrayscaleProcess::Categories() const
{
   return "ColorSpaces, ColorSpaceConversion";
}

// ----------------------------------------------------------------------------

String ConvertToGrayscaleProcess::Description() const
{
   return

   "<html>"
   "<p>ConvertToGrayscale transforms pixels from the RGB color space to the monochrome "
   "grayscale space. Grayscale images have a single nominal channel instead of the "
   "three nominal channels defining RGB images, and are visually rendered by 256 "
   "tones of gray in PixInsight.</p>"

   "<p>Grayscale pixel values are calculated as the L* component of CIE L*a*b* from RGB "
   "source pixels. The transformation is carried out in the RGB Working Space (RGBWS) "
   "of the target image. Existing alpha channels are not modified by ConvertToGrayscale.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String ConvertToGrayscaleProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ConvertToGrayscale.svg";
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToGrayscaleProcess::Create() const
{
   return new ConvertToGrayscaleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToGrayscaleProcess::Clone( const ProcessImplementation& p ) const
{
   const ConvertToGrayscaleInstance* instPtr = dynamic_cast<const ConvertToGrayscaleInstance*>( &p );
   return (instPtr != nullptr) ? new ConvertToGrayscaleInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvertToRGBColorProcess* TheConvertToRGBColorProcess = nullptr;

// ----------------------------------------------------------------------------

ConvertToRGBColorProcess::ConvertToRGBColorProcess()
{
   TheConvertToRGBColorProcess = this;
}

// ----------------------------------------------------------------------------

IsoString ConvertToRGBColorProcess::Id() const
{
   return "ConvertToRGBColor";
}

// ----------------------------------------------------------------------------

IsoString ConvertToRGBColorProcess::Categories() const
{
   return "ColorSpaces, ColorSpaceConversion";
}

// ----------------------------------------------------------------------------

String ConvertToRGBColorProcess::Description() const
{
   return
   "<html>"
   "<p>ConvertToRGBColor transforms pixels from the monochrome gray space to the RGB "
   "color space. RGB color images have three nominal channels corresponding to "
   "their red, green and blue pixel intensities.</p>"

   "<p>ConvertToRGBColor simply copies grayscale source pixel values to each individual "
   "RGB color channel of the resulting image. Existing alpha channels are not "
   "modified by ConvertToRGBColor.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String ConvertToRGBColorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ConvertToRGBColor.svg";
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToRGBColorProcess::Create() const
{
   return new ConvertToRGBColorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvertToRGBColorProcess::Clone( const ProcessImplementation& p ) const
{
   const ConvertToRGBColorInstance* instPtr = dynamic_cast<const ConvertToRGBColorInstance*>( &p );
   return (instPtr != nullptr) ? new ConvertToRGBColorInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBProcesses.cpp - Released 2025-04-07T08:53:55Z
