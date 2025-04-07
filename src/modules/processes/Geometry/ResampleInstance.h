//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ResampleInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ResampleInstance_h
#define __ResampleInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Point.h>
#include <pcl/MetaParameter.h>
#include <pcl/Resample.h>

namespace pcl
{

class PixelInterpolation;

// ----------------------------------------------------------------------------

class ResampleInstance : public ProcessImplementation
{
public:

   ResampleInstance( const MetaProcess* );
   ResampleInstance( const ResampleInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

   void GetNewSizes( int& w, int& h ) const;

private:

   DPoint   p_size;              // resizing fators or absolute dimensions
   pcl_enum p_mode;              // resizing mode
   pcl_enum p_absMode;           // absolute resizing mode
   DPoint   p_resolution;        // horizontal and vertical resolution in pixels
   pcl_bool p_metric;            // metric resolution?
   pcl_bool p_forceResolution;   // set resolution of target image window?
   pcl_enum p_interpolation;     // interpolation algorithm
   float    p_clampingThreshold; // deringing clamping for bicubic spline and Lanczos
   float    p_smoothness;        // smoothness for cubic filter interpolations
   pcl_bool p_gammaCorrection;   // interpolate with gamma correction
   pcl_bool p_noGUIMessages;     // only show warning messages on the console

   friend class ResampleEngine;
   friend class ResampleInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ResampleInstance_h

// ----------------------------------------------------------------------------
// EOF ResampleInstance.h - Released 2025-04-07T08:53:56Z
