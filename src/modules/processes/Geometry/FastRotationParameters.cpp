//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FastRotationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FRMode*        TheFRModeParameter = nullptr;
NoGUIMessages* TheFRNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

FRMode::FRMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheFRModeParameter = this;
}

IsoString FRMode::Id() const
{
   return "mode";
}

size_type FRMode::NumberOfElements() const
{
   return NumberOfModes;
}

IsoString FRMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Rotate180:        return "Rotate180";
   case Rotate90CW:       return "Rotate90CW";
   case Rotate90CCW:      return "Rotate90CCW";
   case HorizontalMirror: return "HorizontalMirror";
   case VerticalMirror:   return "VerticalMirror";
   }
}

int FRMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type FRMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationParameters.cpp - Released 2025-04-07T08:53:55Z
