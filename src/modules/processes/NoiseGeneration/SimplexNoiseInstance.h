//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SimplexNoiseInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SimplexNoiseInstance_h
#define __SimplexNoiseInstance_h

#include <pcl/ProcessImplementation.h>

#include "SimplexNoiseParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SimplexNoiseInstance : public ProcessImplementation
{
public:

   SimplexNoiseInstance( const MetaProcess* );
   SimplexNoiseInstance( const SimplexNoiseInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   float    p_amount;
   int32    p_scale;
   int32    p_offsetX;
   int32    p_offsetY;
   pcl_enum p_operator;

   friend class SimplexNoiseEngine;
   friend class SimplexNoiseInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SimplexNoiseInstance_h

// ----------------------------------------------------------------------------
// EOF SimplexNoiseInstance.h - Released 2025-04-07T08:53:56Z
