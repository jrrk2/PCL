//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// NoiseGeneratorParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NoiseGeneratorParameters_h
#define __NoiseGeneratorParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class NGNoiseAmount : public MetaFloat
{
public:

   NGNoiseAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern NGNoiseAmount* TheNGNoiseAmountParameter;

// ----------------------------------------------------------------------------

class NGNoiseDistribution : public MetaEnumeration
{
public:

   enum { Uniform,      // Uniform distribution
          Normal,       // Normal or Gaussian distribution
          Poisson,      // Poisson distribution
          Impulsional,  // Impulsional noise (salt & pepper)

          NumberOfNGNoiseDistributions,
          Default = Uniform };

   NGNoiseDistribution( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern NGNoiseDistribution* TheNGNoiseDistributionParameter;

// ----------------------------------------------------------------------------

class NGImpulsionalNoiseProbability : public MetaFloat
{
public:

   NGImpulsionalNoiseProbability( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern NGImpulsionalNoiseProbability* TheNGImpulsionalNoiseProbabilityParameter;

// ----------------------------------------------------------------------------

// ### DEPRECATED
class NGPreserveBrightness : public MetaEnumeration
{
public:

   enum { None,            // Don't try to preserve brightness
          PreserveMedian,  // Preserve median values
          PreserveMean,    // Preserve mean values

          NumberOfNGPreserveBrightnessModes,
          Default = PreserveMedian };

   NGPreserveBrightness( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern NGPreserveBrightness* TheNGPreserveBrightnessParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __NoiseGeneratorParameters_h

// ----------------------------------------------------------------------------
// EOF NoiseGeneratorParameters.h - Released 2025-04-07T08:53:56Z
