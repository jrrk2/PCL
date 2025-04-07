//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CropInstance_h
#define __CropInstance_h

#include <pcl/Crop.h>
#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PixelInterpolation;

class CropInstance : public ProcessImplementation
{
public:

   CropInstance( const MetaProcess* );
   CropInstance( const CropInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

   void GetNewSizes( int& w, int& h ) const;

private:

   DRect    p_margins;          // Crop margins
   pcl_enum p_mode;             // Crop mode
   DPoint   p_resolution;       // Horizontal and vertical resolution in pixels
   pcl_bool p_metric;           // Metric resolution?
   pcl_bool p_forceResolution;  // Set resolution of target image window
   DVector  p_fillColor;        // Filling values for extended areas (R/K,G,B,A)
   pcl_bool p_noGUIMessages;    // only show warning messages on the console

   friend class CropEngine;
   friend class CropInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CropInstance_h

// ----------------------------------------------------------------------------
// EOF CropInstance.h - Released 2025-04-07T08:53:55Z
