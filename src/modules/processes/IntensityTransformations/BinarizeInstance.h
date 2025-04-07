//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BinarizeInstance_h
#define __BinarizeInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

class BinarizeInstance : public ProcessImplementation
{
public:

   BinarizeInstance( const MetaProcess* );
   BinarizeInstance( const BinarizeInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

   void Preview( UInt16Image& ) const;

private:

   // Channel indices: 0=R/K, 1=G, 2=B
   double   p_level[ 3 ]; // binarization thresholds in [0,1]
   pcl_bool p_global;   // true to apply first threshold to all channels

   friend class BinarizeProcess;
   friend class BinarizeInterface;
   friend class BinarizeEngine;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BinarizeInstance_h

// ----------------------------------------------------------------------------
// EOF BinarizeInstance.h - Released 2025-04-07T08:53:56Z
