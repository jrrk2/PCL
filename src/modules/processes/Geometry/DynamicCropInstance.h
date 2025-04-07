//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __DynamicCropInstance_h
#define __DynamicCropInstance_h

#include <pcl/Point.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/Rectangle.h>

#include "DynamicCropParameters.h"

namespace pcl
{

class PixelInterpolation;

// ----------------------------------------------------------------------------

class DynamicCropInstance : public ProcessImplementation
{
public:

   DynamicCropInstance( const MetaProcess* );
   DynamicCropInstance( const DynamicCropInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   int32    p_refWidth, p_refHeight; // target image dimensions (px)
   double   p_outWidth, p_outHeight; // output image dimensions (px)
   DPoint   p_center;                // center of cropping rectangle
   double   p_width, p_height;       // normalized dimensions of cropping area ### DEPRECATED
   double   p_angle;                 // rotation angle, radians
   double   p_scaleX, p_scaleY;      // scaling factors
   pcl_bool p_optimizeFast;          // optimize fast rotations: +/-90d, 180d
   pcl_enum p_interpolation;         // interpolation algorithm
   float    p_clampingThreshold;     // deringing clamping for bicubic spline and Lanczos
   float    p_smoothness;            // smoothness for cubic filter interpolations
   pcl_bool p_gammaCorrection;       // interpolate with gamma correction
   DPoint   p_resolution;            // horizontal and vertical resolution in pixels
   pcl_bool p_metric;                // metric resolution?
   pcl_bool p_forceResolution;       // set resolution of target image window?
   DVector  p_fillColor;             // filling values for unused areas (R/K,G,B,A)
   pcl_bool p_noGUIMessages;         // only show warning messages on the console

   RGBColorSystem m_rgbws;

   void EnsureCompatibleWithV1Instance();

   friend class DynamicCropEngine;
   friend class DynamicCropInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicCropInstance_h

// ----------------------------------------------------------------------------
// EOF DynamicCropInstance.h - Released 2025-04-07T08:53:55Z
