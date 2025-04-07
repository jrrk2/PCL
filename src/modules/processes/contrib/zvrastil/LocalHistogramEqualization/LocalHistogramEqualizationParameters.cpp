//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationParameters.cpp - Released 2025-04-07T08:53:56Z
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

#include "LocalHistogramEqualizationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

LHERadius*         TheLHERadiusParameter = nullptr;
LHEHistogramBins*  TheLHEHistogramBinsParameter = nullptr;
LHESlopeLimit*     TheLHESlopeLimitParameter = nullptr;
LHEAmount*         TheLHEAmountParameter = nullptr;
LHECircularKernel* TheLHECircularKernelParameter = nullptr;

// ----------------------------------------------------------------------------

LHERadius::LHERadius( MetaProcess* P )
   : MetaInt32( P )
{
   TheLHERadiusParameter = this;
}

IsoString LHERadius::Id() const
{
   return "radius";
}

double LHERadius::DefaultValue() const
{
   return 64;
}

double LHERadius::MinimumValue() const
{
   return 16;
}

double LHERadius::MaximumValue() const
{
   return 512;
}

// ----------------------------------------------------------------------------

LHEHistogramBins::LHEHistogramBins( MetaProcess* P )
   : MetaEnumeration( P )
{
   TheLHEHistogramBinsParameter = this;
}

IsoString LHEHistogramBins::Id() const
{
   return "histogramBins";
}

size_type LHEHistogramBins::NumberOfElements() const
{
   return NumberOfItems;
}

IsoString LHEHistogramBins::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Bit8:
      return "Bit8";
   case Bit10:
      return "Bit10";
   case Bit12:
      return "Bit12";
      //case Bit14: return "Bit14";
      //case Bit16: return "Bit16";
   }
}

int LHEHistogramBins::ElementValue( size_type i ) const
{
   return int( i );
}

size_type LHEHistogramBins::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

LHESlopeLimit::LHESlopeLimit( MetaProcess* P )
   : MetaDouble( P )
{
   TheLHESlopeLimitParameter = this;
}

IsoString LHESlopeLimit::Id() const
{
   return "slopeLimit";
}

double LHESlopeLimit::DefaultValue() const
{
   return 2.0;
}

double LHESlopeLimit::MinimumValue() const
{
   return 1.0;
}

double LHESlopeLimit::MaximumValue() const
{
   return 64.0;
}

int LHESlopeLimit::Precision() const
{
   return 1;
}

// ----------------------------------------------------------------------------

LHEAmount::LHEAmount( MetaProcess* P )
   : MetaDouble( P )
{
   TheLHEAmountParameter = this;
}

IsoString LHEAmount::Id() const
{
   return "amount";
}

double LHEAmount::DefaultValue() const
{
   return 1.0;
}

double LHEAmount::MinimumValue() const
{
   return 0.0;
}

double LHEAmount::MaximumValue() const
{
   return 1.0;
}

int LHEAmount::Precision() const
{
   return 3;
}

// ----------------------------------------------------------------------------

LHECircularKernel::LHECircularKernel( MetaProcess* P )
   : MetaBoolean( P )
{
   TheLHECircularKernelParameter = this;
}

IsoString LHECircularKernel::Id() const
{
   return "circularKernel";
}

bool LHECircularKernel::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationParameters.cpp - Released 2025-04-07T08:53:56Z
