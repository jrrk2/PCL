//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "DynamicCropProcess.h"
#include "DynamicCropParameters.h"
#include "DynamicCropInstance.h"
#include "DynamicCropInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

DynamicCropProcess* TheDynamicCropProcess = nullptr;

// ----------------------------------------------------------------------------

DynamicCropProcess::DynamicCropProcess()
{
   TheDynamicCropProcess = this;

   new DCRefWidth( this );
   new DCRefHeight( this );
   new DCOutWidth( this );
   new DCOutHeight( this );
   new DCCenterX( this );
   new DCCenterY( this );
   new DCWidth( this );
   new DCHeight( this );
   TheDCRotationAngleParameter = new RotationAngle( this );
   new DCScaleX( this );
   new DCScaleY( this );
   TheDCOptimizeFastRotationsParameter = new OptimizeFastRotations( this );
   TheDCInterpolationAlgorithmParameter = new InterpolationAlgorithm( this );
   TheDCClampingThresholdParameter = new ClampingThreshold( this );
   TheDCSmoothnessParameter = new Smoothness( this );
   TheDCGammaCorrectionParameter = new GammaCorrection( this );
   TheDCXResolutionParameter = new XResolution( this );
   TheDCYResolutionParameter = new YResolution( this );
   TheDCMetricResolutionParameter = new MetricResolution( this );
   TheDCForceResolutionParameter = new ForceResolution( this );
   TheDCFillRedParameter = new FillRed( this );
   TheDCFillGreenParameter = new FillGreen( this );
   TheDCFillBlueParameter = new FillBlue( this );
   TheDCFillAlphaParameter = new FillAlpha( this );
   TheDCNoGUIMessagesParameter = new NoGUIMessages( this );
}

// ----------------------------------------------------------------------------

IsoString DynamicCropProcess::Id() const
{
   return "DynamicCrop";
}

// ----------------------------------------------------------------------------

IsoString DynamicCropProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 DynamicCropProcess::Version() const
{
   return 2;
}

// ----------------------------------------------------------------------------

String DynamicCropProcess::Description() const
{
   return

   "<html>"
   "<p>DynamicCrop is an interactive cropping/rotation/scaling procedure implemented as a "
   "dynamic PixInsight process. As most geometric processes, DynamicCrop can only be "
   "executed on images, not on previews.</p>"

   "<p>To start a DynamicCrop session, double-click the corresponding entry on the "
   "Process Explorer; the DynamicCrop interface window will be shown and activated. "
   "Then click on the target image and drag the mouse cursor to define an initial "
   "cropping rectangle. Once you release the mouse, the DynamicCrop window becomes "
   "active and fully operational.</p>"

   "<p>You can edit DynamicCrop parameters by entering their numeric values literally, "
   "but most of the time you'll be defining a cropping area on the image with the mouse. "
   "To use DynamicCrop interactively, you can perform, among many other actions:</p>"

   "<ul>"
   "<li>Click <i>inside</i> the cropping rectangle and drag to <b>move</b> it.<br/></li>"

   "<li>Click <i>outside</i> the cropping rectangle and drag to <b>rotate</b> it.<br/></li>"

   "<li>Click <i>on one side or corner</i> of the rectangle to change its <b>dimensions</b>.<br/></li>"

   "<li>Click on the <i>center icon</i> (a small circle initially located at the rectangle's center) "
   "and drag to <b>move the center of rotation</b>. You can double-click the center icon to "
   "relocate it at its default central position.<br/></li>"

   "<li>Double-click inside the cropping rectangle to <b>commit</b> the defined cropping/rotation/scaling "
   "operation.<br/></li>"

   "<li>Pressing the Return key or clicking the Execute button on the DynamicCrop window "
   "also commit the currently defined process. The Escape key cancels the DynamicCrop session.<br/></li>"
   "</ul>"

   "<p>Watch the mouse cursor; its shape changes to give you feedback about the actions "
   "available as a function of the current cursor position over the image.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String DynamicCropProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/DynamicCrop.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* DynamicCropProcess::DefaultInterface() const
{
   return TheDynamicCropInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* DynamicCropProcess::Create() const
{
   return new DynamicCropInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* DynamicCropProcess::Clone( const ProcessImplementation& p ) const
{
   const DynamicCropInstance* instPtr = dynamic_cast<const DynamicCropInstance*>( &p );
   return (instPtr != nullptr) ? new DynamicCropInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropProcess.cpp - Released 2025-04-07T08:53:55Z
