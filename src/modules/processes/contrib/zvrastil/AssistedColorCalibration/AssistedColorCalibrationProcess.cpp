//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AssistedColorCalibrationProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AssistedColorCalibrationProcess.h"
#include "AssistedColorCalibrationInstance.h"
#include "AssistedColorCalibrationInterface.h"
#include "AssistedColorCalibrationParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AssistedColorCalibrationProcess* TheAssistedColorCalibrationProcess = nullptr;

// ----------------------------------------------------------------------------

AssistedColorCalibrationProcess::AssistedColorCalibrationProcess()
{
   TheAssistedColorCalibrationProcess = this;

   new ACCRedCorrectionFactor( this );
   new ACCGreenCorrectionFactor( this );
   new ACCBlueCorrectionFactor( this );
   new ACCBackgroundReference( this );
   new ACCHistogramShadows( this );
   new ACCHistogramHighlights( this );
   new ACCHistogramMidtones( this );
   new ACCSaturationBoost( this );
}

// ----------------------------------------------------------------------------

IsoString AssistedColorCalibrationProcess::Id() const
{
   return "AssistedColorCalibration";
}

// ----------------------------------------------------------------------------

IsoString AssistedColorCalibrationProcess::Category() const
{
   return "ColorCalibration";
}

// ----------------------------------------------------------------------------

uint32 AssistedColorCalibrationProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationProcess::Description() const
{
   return
   "<html>"
   "<p>AssistedColorCalibration process is used to do <b>manual</b> color calibration (white balance) on the image."
   "The color calibration should be done before further processing, even before the background (sky glow, light pollution) is subtracted. "
   "This makes manual color calibration difficult, because it is not easy to evaluate the effect on not-yet-processed image.</p>"
   "<p>This process helps you to find out (or just check) the correct white balance factors by computing an approximation of final processed image. "
   "It does it by applying selected white balance coefficents, subtracting provided background reference (also calibrated), applying histogram "
   "transform and color saturation enhancement. Although this is definitely very simple processing, it should give you an image reasonably close "
   "to final result - at least good enough to evaluate current white balance coefficents.</p>"
   "<p>Please note that these further steps (background subtraction, histogram transform, saturation enhancement) are only applied to previews. Only white "
   "balance cofficients are applied to the regular image. This process is only designed to do the white balance - not to substitute the post-processing.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String AssistedColorCalibrationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/AssistedColorCalibration.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* AssistedColorCalibrationProcess::DefaultInterface() const
{
   return TheAssistedColorCalibrationInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* AssistedColorCalibrationProcess::Create() const
{
   return new AssistedColorCalibrationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AssistedColorCalibrationProcess::Clone( const ProcessImplementation& p ) const
{
   const AssistedColorCalibrationInstance* instPtr = dynamic_cast<const AssistedColorCalibrationInstance*>( &p );
   return (instPtr != nullptr) ? new AssistedColorCalibrationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool AssistedColorCalibrationProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationProcess.cpp - Released 2025-04-07T08:53:56Z
