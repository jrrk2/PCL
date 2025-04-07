//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AutoHistogramInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AutoHistogramInstance_h
#define __AutoHistogramInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

class AutoHistogramInstance : public ProcessImplementation
{
public:

   AutoHistogramInstance( const MetaProcess* );
   AutoHistogramInstance( const AutoHistogramInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   // Channel indexes: 0=R/K, 1=G, 2=B

   pcl_bool p_clip;              // perform histogram clipping ?
   pcl_bool p_clipTogether;      // GUI only: clip joint RGB channels ?
   float    p_clipLow[ 3 ];      // clipped pixels, shadows, percentage of total
   float    p_clipHigh[ 3 ];     // clipped pixels, highlights, percentage of total

   pcl_bool p_stretch;           // perform nonlinear stretch ?
   pcl_bool p_stretchTogether;   // GUI only: stretch joint RGB channels ?
   pcl_enum p_stretchMethod;     // nonlinear stretch method
   double   p_targetMedian[ 3 ]; // target median values in [0,1]

   friend class AutoHistogramEngine;
   friend class AutoHistogramProcess;
   friend class AutoHistogramInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AutoHistogramInstance_h

// ----------------------------------------------------------------------------
// EOF AutoHistogramInstance.h - Released 2025-04-07T08:53:56Z
