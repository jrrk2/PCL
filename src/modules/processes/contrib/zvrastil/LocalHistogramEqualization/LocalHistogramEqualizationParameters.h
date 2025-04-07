//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationParameters.h - Released 2025-04-07T08:53:56Z
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

#ifndef __LocalHistogramEqualizationParameters_h
#define __LocalHistogramEqualizationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class LHERadius : public MetaInt32
{
public:

   LHERadius( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern LHERadius* TheLHERadiusParameter;

// ----------------------------------------------------------------------------

class LHEHistogramBins : public MetaEnumeration
{
public:

   enum
   {
      Bit8,
      Bit10,
      Bit12,
      //Bit14,
      //Bit16,
      NumberOfItems,
      Default = Bit8
   };

   LHEHistogramBins( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern LHEHistogramBins* TheLHEHistogramBinsParameter;

// ----------------------------------------------------------------------------

class LHESlopeLimit : public MetaDouble
{
public:

   LHESlopeLimit( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   int Precision() const override;
};

extern LHESlopeLimit* TheLHESlopeLimitParameter;

// ----------------------------------------------------------------------------

class LHEAmount : public MetaDouble
{
public:

   LHEAmount( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   int Precision() const override;
};

extern LHEAmount* TheLHEAmountParameter;

// ----------------------------------------------------------------------------

class LHECircularKernel : public MetaBoolean
{
public:

   LHECircularKernel( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern LHECircularKernel* TheLHECircularKernelParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __LocalHistogramEqualizationParameters_h

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationParameters.h - Released 2025-04-07T08:53:56Z
