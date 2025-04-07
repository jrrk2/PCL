//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// SampleFormatConversionParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SampleFormatConversionParameters_h
#define __SampleFormatConversionParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SampleFormatConversion : public MetaEnumeration
{
public:

   enum { To8Bit,       // unsigned 8-bit integer
          To16Bit,      // unsigned 16-bit integer
          To32Bit,      // unsigned 32-bit integer
          ToFloat,      // IEEE 32-bit floating point (simple precision)
          ToDouble };   // IEEE 64-bit floating point (double precision)

   SampleFormatConversion( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern SampleFormatConversion* TheSampleFormatConversionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SampleFormatConversionParameters_h

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionParameters.h - Released 2025-04-07T08:53:56Z
