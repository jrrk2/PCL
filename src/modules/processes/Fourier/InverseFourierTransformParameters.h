//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// InverseFourierTransformParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __InverseFourierTransformParameters_h
#define __InverseFourierTransformParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class IFTIdOfFirstComponent : public MetaString
{
public:

   IFTIdOfFirstComponent( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern IFTIdOfFirstComponent* TheIFTIdOfFirstComponentParameter;

// ----------------------------------------------------------------------------

class IFTIdOfSecondComponent : public MetaString
{
public:

   IFTIdOfSecondComponent( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String AllowedCharacters() const override;
};

extern IFTIdOfSecondComponent* TheIFTIdOfSecondComponentParameter;

// ----------------------------------------------------------------------------

class IFTOnOutOfRangeResult : public MetaEnumeration
{
public:

   enum { DontCare,
          Truncate,
          Rescale,
          NumberOfModes,
          Default = Truncate };

   IFTOnOutOfRangeResult( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern IFTOnOutOfRangeResult* TheIFTOnOutOfRangeResultParameter;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __InverseFourierTransformParameters_h

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformParameters.h - Released 2025-04-07T08:53:55Z
