//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// ICCProfileTransformationParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ICCProfileTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ICCTTargetProfile*                  TheICCTTargetProfileParameter = nullptr;
ICCTToDefaultProfile*               TheICCTToDefaultProfileParameter = nullptr;
ICCTRenderingIntent*                TheICCTRenderingIntentParameter = nullptr;
ICCTUseBlackPointCompensation*      TheICCTUseBlackPointCompensationParameter = nullptr;
ICCTUseFloatingPointTransformation* TheICCTUseFloatingPointTransformationParameter = nullptr;

// ----------------------------------------------------------------------------

ICCTTargetProfile::ICCTTargetProfile( MetaProcess* P ) : MetaString( P )
{
   TheICCTTargetProfileParameter = this;
}

IsoString ICCTTargetProfile::Id() const
{
   return "targetProfile";
}

// ----------------------------------------------------------------------------

ICCTToDefaultProfile::ICCTToDefaultProfile( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCTToDefaultProfileParameter = this;
}

IsoString ICCTToDefaultProfile::Id() const
{
   return "toDefaultProfile";
}

bool ICCTToDefaultProfile::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

ICCTRenderingIntent::ICCTRenderingIntent( MetaProcess* P ) : MetaEnumeration( P )
{
   TheICCTRenderingIntentParameter = this;
}

IsoString ICCTRenderingIntent::Id() const
{
   return "renderingIntent";
}

size_type ICCTRenderingIntent::NumberOfElements() const
{
   return NumberOfRenderingIntents;
}

IsoString ICCTRenderingIntent::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case Perceptual:           return "Perceptual";
   case Saturation:           return "Saturation";
   case RelativeColorimetric: return "RelativeColorimetric";
   case AbsoluteColorimetric: return "AbsoluteColorimetric";
   }
}

int ICCTRenderingIntent::ElementValue( size_type i ) const
{
   return int( i );
}

size_type ICCTRenderingIntent::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

ICCTUseBlackPointCompensation::ICCTUseBlackPointCompensation( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCTUseBlackPointCompensationParameter = this;
}

IsoString ICCTUseBlackPointCompensation::Id() const
{
   return "useBlackPointCompensation";
}

bool ICCTUseBlackPointCompensation::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ICCTUseFloatingPointTransformation::ICCTUseFloatingPointTransformation( MetaProcess* P ) : MetaBoolean( P )
{
   TheICCTUseFloatingPointTransformationParameter = this;
}

IsoString ICCTUseFloatingPointTransformation::Id() const
{
   return "useFloatingPointTransformation";
}

bool ICCTUseFloatingPointTransformation::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationParameters.cpp - Released 2025-04-07T08:53:55Z
