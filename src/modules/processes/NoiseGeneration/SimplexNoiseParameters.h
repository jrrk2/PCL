//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SimplexNoiseParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SimplexNoiseParameters_h
#define __SimplexNoiseParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class SNAmount : public MetaFloat
{
public:

   SNAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern SNAmount* TheSNAmountParameter;

// ----------------------------------------------------------------------------

class SNScale : public MetaInt32
{
public:

   SNScale( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern SNScale* TheSNScaleParameter;

// ----------------------------------------------------------------------------

class SNOffsetX : public MetaInt32
{
public:

   SNOffsetX( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern SNOffsetX* TheSNOffsetXParameter;

// ----------------------------------------------------------------------------

class SNOffsetY : public MetaInt32
{
public:

   SNOffsetY( MetaProcess* );

   IsoString Id() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern SNOffsetY* TheSNOffsetYParameter;

// ----------------------------------------------------------------------------

class SNOperator : public MetaEnumeration
{
public:

   enum { Copy,   // Normal mode, replace target
          Add,    // Add noise to target
          Sub,    // Subtract noise from target
          Mul,    // Multiply target by noise
          Div,    // Divide target by noise
          Pow,    // Target = Target ** noise (exponentiation)
          Dif,    // Target = |Target - noise|
          Screen, // Target = ~(~Target * ~noise)
          Or,     // Target |= noise
          And,    // Target &= noise
          Xor,    // Target ^= noise
          Nor,    // Target |= ~noise
          Nand,   // Target &= ~noise
          Xnor,   // Target ^= ~noise

          NumberOfOperators,
          Default = Copy };

   SNOperator( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
};

extern SNOperator* TheSNOperatorParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __SimplexNoiseParameters_h

// ----------------------------------------------------------------------------
// EOF SimplexNoiseParameters.h - Released 2025-04-07T08:53:56Z
