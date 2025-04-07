//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// RotationInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RotationInstance_h
#define __RotationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Point.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PixelInterpolation;

class RotationInstance : public ProcessImplementation
{
public:

   RotationInstance( const MetaProcess* );
   RotationInstance( const RotationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

   void GetNewSizes( int& w, int& h ) const;

private:

   double   p_angle;             // rotation angle, radians
   pcl_bool p_optimizeFast;      // optimize fast rotations: +/-90d, 180d
   pcl_enum p_interpolation;     // interpolation algorithm
   float    p_clampingThreshold; // deringing clamping for bicubic spline and Lanczos
   float    p_smoothness;        // smoothness for cubic filter interpolations
   pcl_bool p_gammaCorrection;   // interpolate with gamma correction
   DVector  p_fillColor;         // filling values for unused areas (R/K,G,B,A)
   pcl_bool p_noGUIMessages;     // only show warning messages on the console

   friend class RotationEngine;
   friend class RotationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RotationInstance_h

// ----------------------------------------------------------------------------
// EOF RotationInstance.h - Released 2025-04-07T08:53:55Z
