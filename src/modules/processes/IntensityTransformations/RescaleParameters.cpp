//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// RescaleParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RescaleParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RescalingMode* TheRescalingModeParameter = nullptr;

// ----------------------------------------------------------------------------

RescalingMode::RescalingMode( MetaProcess* p ) : MetaEnumeration( p )
{
   TheRescalingModeParameter = this;
}

IsoString RescalingMode::Id() const
{
   return "mode";
}

size_type RescalingMode::NumberOfElements() const
{
   return NumberOfRescalingModes;
}

IsoString RescalingMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RGBK:            return "RGBK";
   case RGBK_Individual: return "RGBK_Individual";
   case CIEL:            return "CIEL";
   case CIEY:            return "CIEY";
   }
}

int RescalingMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type RescalingMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleParameters.cpp - Released 2025-04-07T08:53:56Z
