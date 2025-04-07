//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __IntegerResampleInstance_h
#define __IntegerResampleInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h>
#include <pcl/Point.h>
#include <pcl/IntegerResample.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class IntegerResampleInstance : public ProcessImplementation
{
public:

   IntegerResampleInstance( const MetaProcess* );
   IntegerResampleInstance( const IntegerResampleInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

   void GetNewSizes( int& w, int& h ) const;

private:

   int32    p_zoomFactor;
   pcl_enum p_downsampleMode;
   DPoint   p_resolution;      // Horizontal and vertical resolution in pixels
   pcl_bool p_metric;          // Metric resolution?
   pcl_bool p_forceResolution; // Set resolution of target image window
   pcl_bool p_gammaCorrection; // interpolate with gamma correction
   pcl_bool p_noGUIMessages;   // only show warning messages on the console

   friend class IntegerResampleEngine;
   friend class IntegerResampleInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __IntegerResampleInstance_h

// ----------------------------------------------------------------------------
// EOF IntegerResampleInstance.h - Released 2025-04-07T08:53:55Z
