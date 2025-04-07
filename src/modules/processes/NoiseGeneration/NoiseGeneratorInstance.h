//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// NoiseGeneratorInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NoiseGeneratorInstance_h
#define __NoiseGeneratorInstance_h

#include <pcl/ProcessImplementation.h>

#include "NoiseGeneratorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class NoiseGeneratorInstance : public ProcessImplementation
{
public:

   NoiseGeneratorInstance( const MetaProcess* );
   NoiseGeneratorInstance( const NoiseGeneratorInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   float    p_amount;
   pcl_enum p_distribution;
   float    p_impulsionalNoiseProbability;
   pcl_enum p_preserveBrightness; // ### deprecated

   friend class NoiseGeneratorEngine;
   friend class NoiseGeneratorInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NoiseGeneratorInstance_h

// ----------------------------------------------------------------------------
// EOF NoiseGeneratorInstance.h - Released 2025-04-07T08:53:56Z
