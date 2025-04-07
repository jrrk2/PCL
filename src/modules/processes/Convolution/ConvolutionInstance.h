//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ConvolutionInstance_h
#define __ConvolutionInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum

#include "FilterLibrary.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvolutionInstance : public ProcessImplementation
{
public:

   ConvolutionInstance( const MetaProcess* );
   ConvolutionInstance( const ConvolutionInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   bool CreateFilterImage( Image& ) const;

private:

   pcl_enum mode;             // parametric, library or external image
   float    sigma;            // standard deviation of parametric filter
   float    shape;            // shape (kurtosys) of parametric filter
   float    aspectRatio;      // axis ratio of parametric filter
   float    rotationAngle;    // rotation angle of parametric filter, degrees
   String   sourceCode;       // filter source code (library mode only)
   pcl_bool rescaleHighPass;  // rescale after convolution with a high pass filter
   String   viewId;           // id of external filter view (external image only)

   // Special routine for real-time previews
   void Convolve( Image&, int zoomLevel ) const;

   friend class ConvolutionEngine;
   friend class ConvolutionProcess;
   friend class ConvolutionInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ConvolutionInstance_h

// ----------------------------------------------------------------------------
// EOF ConvolutionInstance.h - Released 2025-04-07T08:53:55Z
