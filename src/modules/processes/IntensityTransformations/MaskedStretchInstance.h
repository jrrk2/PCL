//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// MaskedStretchInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MaskedStretchInstance_h
#define __MaskedStretchInstance_h

#include <pcl/MetaParameter.h> // for pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class MaskedStretchInstance : public ProcessImplementation
{
public:

   MaskedStretchInstance( const MetaProcess* );
   MaskedStretchInstance( const MaskedStretchInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   double   p_targetBackground;          // target mean background value in [0,1]
   int32    p_numberOfIterations;        // number of iterations, in [0,1000]
   double   p_clippingFraction;          // fraction of clipped dark pixels (0=disabled)
   String   p_backgroundReferenceViewId; // if empty, use $T
   double   p_backgroundLow;             // background sampling range, lower bound
   double   p_backgroundHigh;            // background sampling range, upper bound
   pcl_bool p_useROI;                    // use a region of interest or the whole image
   Rect     p_roi = 0;                   // region of interest
   pcl_enum p_maskType;                  // mask component for color images: V or I

   friend class MaskedStretchEngine;
   friend class MaskedStretchProcess;
   friend class MaskedStretchInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MaskedStretchInstance_h

// ----------------------------------------------------------------------------
// EOF MaskedStretchInstance.h - Released 2025-04-07T08:53:56Z
