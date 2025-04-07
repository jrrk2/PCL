//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// LarsonSekaninaInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LarsonSekaninaInstance_h
#define __LarsonSekaninaInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

class LarsonSekaninaInstance : public ProcessImplementation
{
public:

   LarsonSekaninaInstance( const MetaProcess* );
   LarsonSekaninaInstance( const LarsonSekaninaInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   pcl_enum interpolation;
   float    radiusDiff;
   float    angleDiff;
   DPoint   center;
   float    amount;
   float    threshold;
   float    deringing;
   pcl_bool useLuminance;
   pcl_bool highPass;

   float    rangeLow;
   float    rangeHigh;
   pcl_bool disableExtension;

   friend class LarsonSekaninaProcess;
   friend class LarsonSekaninaInterface;

};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LarsonSekaninaInstance_h

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaInstance.h - Released 2025-04-07T08:53:55Z
