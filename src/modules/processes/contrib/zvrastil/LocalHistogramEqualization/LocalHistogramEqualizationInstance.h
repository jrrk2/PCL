//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LocalHistogramEqualizationInstance_h
#define __LocalHistogramEqualizationInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class LocalHistogramEqualizationInstance : public ProcessImplementation
{
public:

   LocalHistogramEqualizationInstance( const MetaProcess* );
   LocalHistogramEqualizationInstance( const LocalHistogramEqualizationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

   int GetRadius() const
   {
      return radius;
   }

   int GetHistogramSize() const;

   double GetLimit() const
   {
      return slopeLimit;
   }

   double GetAmount() const
   {
      return amount;
   }

   bool IsCircular() const
   {
      return circularKernel;
   }

   void Preview( UInt16Image& ) const;

private:

   int32    radius;
   pcl_enum histogramBins;
   double   slopeLimit;
   double   amount;
   pcl_bool circularKernel;

   friend class LocalHistogramEqualizationProcess;
   friend class LocalHistogramEqualizationInterface;
   friend class LocalHistogramEqualizationEngine;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __LocalHistogramEqualizationInstance_h

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationInstance.h - Released 2025-04-07T08:53:56Z
