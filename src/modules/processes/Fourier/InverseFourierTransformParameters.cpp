//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// InverseFourierTransformParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "InverseFourierTransformParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IFTIdOfFirstComponent*  TheIFTIdOfFirstComponentParameter = nullptr;
IFTIdOfSecondComponent* TheIFTIdOfSecondComponentParameter = nullptr;
IFTOnOutOfRangeResult*  TheIFTOnOutOfRangeResultParameter = nullptr;

// ----------------------------------------------------------------------------

IFTIdOfFirstComponent::IFTIdOfFirstComponent( MetaProcess* p ) : MetaString( p )
{
   TheIFTIdOfFirstComponentParameter = this;
}

IsoString IFTIdOfFirstComponent::Id() const
{
   return "idOfFirstComponent";
}

size_type IFTIdOfFirstComponent::MinLength() const
{
   return 0; // can be void
}

String IFTIdOfFirstComponent::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

IFTIdOfSecondComponent::IFTIdOfSecondComponent( MetaProcess* p ) : MetaString( p )
{
   TheIFTIdOfSecondComponentParameter = this;
}

IsoString IFTIdOfSecondComponent::Id() const
{
   return "idOfSecondComponent";
}

size_type IFTIdOfSecondComponent::MinLength() const
{
   return 0; // can be void
}

String IFTIdOfSecondComponent::AllowedCharacters() const
{
   return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
}

// ----------------------------------------------------------------------------

IFTOnOutOfRangeResult::IFTOnOutOfRangeResult( MetaProcess* P ) : MetaEnumeration( P )
{
   TheIFTOnOutOfRangeResultParameter = this;
}

IsoString IFTOnOutOfRangeResult::Id() const
{
   return "onOutOfRangeResult";
}

size_type IFTOnOutOfRangeResult::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString IFTOnOutOfRangeResult::ElementId( size_type i ) const
{
   switch ( i )
   {
   case DontCare: return "DontCare";
   default:
   case Truncate: return "Truncate";
   case Rescale:  return "Rescale";
   }
}

int IFTOnOutOfRangeResult::ElementValue( size_type i ) const
{
   return int( i );
}

size_type IFTOnOutOfRangeResult::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformParameters.cpp - Released 2025-04-07T08:53:55Z
