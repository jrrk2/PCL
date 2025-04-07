//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// UnsharpMaskInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __UnsharpMaskInstance_h
#define __UnsharpMaskInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

class UnsharpMaskInstance : public ProcessImplementation
{
public:

   UnsharpMaskInstance( const MetaProcess* );
   UnsharpMaskInstance( const UnsharpMaskInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

   void Preview( UInt16Image& img ) const;

private:

   // USM Filter
   float    sigma;
   float    amount;
   pcl_bool useLuminance;        // apply to luminance for color images
   pcl_bool linear;              // apply to CIE Y instead of CIE L*

   // Deringing
   pcl_bool deringing;           // enable deringing
   float    deringingDark;       // deringing amount, dark rings
   float    deringingBright;     // deringing amount, bright rings
   pcl_bool outputDeringingMaps; // generate deringing map images

   // Dynamic Range Extension
   float    rangeLow;            // dynamic range extension, low range
   float    rangeHigh;           // dynamic range extension, high range

   friend class UnsharpMaskEngine;
   friend class UnsharpMaskProcess;
   friend class UnsharpMaskInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __UnsharpMaskInstance_h

// ----------------------------------------------------------------------------
// EOF UnsharpMaskInstance.h - Released 2025-04-07T08:53:55Z
