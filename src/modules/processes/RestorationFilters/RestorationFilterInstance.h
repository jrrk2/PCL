//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 1.0.5
// ----------------------------------------------------------------------------
// RestorationFilterInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RestorationFilterInstance_h
#define __RestorationFilterInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "RestorationFilterParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RestorationFilterInstance : public ProcessImplementation
{
public:

   RestorationFilterInstance( const MetaProcess* );
   RestorationFilterInstance( const RestorationFilterInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   bool CreatePSF( Image& ) const;

private:

   // PSF
   pcl_enum psfMode;                // PSF type: Parametric, motion blur, or external image
   float    psfSigma;               // standard deviation of Parametric PSF
   float    psfShape;               // kurtosys of Parametric-family PSF
   float    psfAspectRatio;         // axis ratio of Parametric PSF
   float    psfRotationAngle;       // rotation angle of Parametric PSF, degrees
   float    psfMotionLength;        // length of motion blur PSF
   float    psfMotionRotationAngle; // angle of motion blur PSF
   String   psfViewId;              // PSF view id (for external PSF only)

   // Noise estimates
   double   K;                      // noise estimation, Wiener
   double   gamma;                  // noise estimation, Constrained Least Squares

   // Filter parameters
   pcl_enum algorithm;
   float    amount;                 // processed:original ratio
   pcl_bool toLuminance;            // apply to luminance for color images
   pcl_bool linear;                 // apply to CIE Y instead of CIE L*

   // Deringing
   pcl_bool deringing;              // enable deringing
   float    deringingDark;          // deringing amount, dark rings
   float    deringingBright;        // deringing amount, bright rings
   pcl_bool outputDeringingMaps;    // generate deringing map images

   // Dynamic Range Extension
   float    rangeLow;               // dynamic range extension, low range
   float    rangeHigh;              // dynamic range extension, high range

   friend class RestorationFilterEngine;
   friend class RestorationFilterInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RestorationFilterInstance_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterInstance.h - Released 2025-04-07T08:53:56Z
