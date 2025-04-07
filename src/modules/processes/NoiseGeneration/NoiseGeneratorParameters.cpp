//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// NoiseGeneratorParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "NoiseGeneratorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NGNoiseAmount*                 TheNGNoiseAmountParameter = nullptr;
NGNoiseDistribution*           TheNGNoiseDistributionParameter = nullptr;
NGImpulsionalNoiseProbability* TheNGImpulsionalNoiseProbabilityParameter = nullptr;
NGPreserveBrightness*          TheNGPreserveBrightnessParameter = nullptr; // ### DEPRECATED

// ----------------------------------------------------------------------------

NGNoiseAmount::NGNoiseAmount( MetaProcess* p ) : MetaFloat( p )
{
   TheNGNoiseAmountParameter = this;
}

IsoString NGNoiseAmount::Id() const
{
   return "amount";
}

int NGNoiseAmount::Precision() const
{
   return 8;
}

double NGNoiseAmount::DefaultValue() const
{
   return 0.1;
}

// ----------------------------------------------------------------------------

NGNoiseDistribution::NGNoiseDistribution( MetaProcess* p ) : MetaEnumeration( p )
{
   TheNGNoiseDistributionParameter = this;
}

IsoString NGNoiseDistribution::Id() const
{
   return "distribution";
}

size_type NGNoiseDistribution::NumberOfElements() const
{
   return NumberOfNGNoiseDistributions;
}

IsoString NGNoiseDistribution::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Uniform:     return "Uniform";
   case Normal:      return "Normal";
   case Poisson:     return "Poisson";
   case Impulsional: return "Impulsional";
   }
}

int NGNoiseDistribution::ElementValue( size_type i ) const
{
   return int( i );
}

size_type NGNoiseDistribution::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

NGImpulsionalNoiseProbability::NGImpulsionalNoiseProbability( MetaProcess* p ) : MetaFloat( p )
{
   TheNGImpulsionalNoiseProbabilityParameter = this;
}

IsoString NGImpulsionalNoiseProbability::Id() const
{
   return "impulsionalNoiseProbability";
}

int NGImpulsionalNoiseProbability::Precision() const
{
   return 4;
}

double NGImpulsionalNoiseProbability::DefaultValue() const
{
   return 0.0500;
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
NGPreserveBrightness::NGPreserveBrightness( MetaProcess* p ) : MetaEnumeration( p )
{
   TheNGPreserveBrightnessParameter = this;
}

IsoString NGPreserveBrightness::Id() const
{
   return "preserveBrightness";
}

size_type NGPreserveBrightness::NumberOfElements() const
{
   return NumberOfNGPreserveBrightnessModes;
}

IsoString NGPreserveBrightness::ElementId( size_type i ) const
{
   switch ( i )
   {
   case None:           return "None";
   default:
   case PreserveMedian: return "PreserveMedian";
   case PreserveMean:   return "PreserveMean";
   }
}

int NGPreserveBrightness::ElementValue( size_type i ) const
{
   return int( i );
}

size_type NGPreserveBrightness::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoiseGeneratorParameters.cpp - Released 2025-04-07T08:53:56Z
