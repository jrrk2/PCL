//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CropParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CRLeftMargin*     TheCRLeftMarginParameter = nullptr;
CRTopMargin*      TheCRTopMarginParameter = nullptr;
CRRightMargin*    TheCRRightMarginParameter = nullptr;
CRBottomMargin*   TheCRBottomMarginParameter = nullptr;
CRMode*           TheCRModeParameter = nullptr;
XResolution*      TheCRXResolutionParameter = nullptr;
YResolution*      TheCRYResolutionParameter = nullptr;
MetricResolution* TheCRMetricResolutionParameter = nullptr;
ForceResolution*  TheCRForceResolutionParameter = nullptr;
FillRed*          TheCRFillRedParameter = nullptr;
FillGreen*        TheCRFillGreenParameter = nullptr;
FillBlue*         TheCRFillBlueParameter = nullptr;
FillAlpha*        TheCRFillAlphaParameter = nullptr;
NoGUIMessages*    TheCRNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

CRLeftMargin::CRLeftMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRLeftMarginParameter = this;
}

IsoString CRLeftMargin::Id() const
{
   return "leftMargin";
}

int CRLeftMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRTopMargin::CRTopMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRTopMarginParameter = this;
}

IsoString CRTopMargin::Id() const
{
   return "topMargin";
}

int CRTopMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRRightMargin::CRRightMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRRightMarginParameter = this;
}

IsoString CRRightMargin::Id() const
{
   return "rightMargin";
}

int CRRightMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRBottomMargin::CRBottomMargin( MetaProcess* P ) : MetaDouble( P )
{
   TheCRBottomMarginParameter = this;
}

IsoString CRBottomMargin::Id() const
{
   return "bottomMargin";
}

int CRBottomMargin::Precision() const
{
   return -8;
}

// ----------------------------------------------------------------------------

CRMode::CRMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheCRModeParameter = this;
}

IsoString CRMode::Id() const
{
   return "mode";
}

size_type CRMode::NumberOfElements() const
{
   return NumberOfCRModes;
}

IsoString CRMode::ElementId( size_type i ) const
{
   switch ( i )
   {
   default:
   case RelativeMargins:     return "RelativeMargins";
   case AbsolutePixels:      return "AbsolutePixels";
   case AbsoluteCentimeters: return "AbsoluteCentimeters";
   case AbsoluteInches:      return "AbsoluteInches";
   }
}

int CRMode::ElementValue( size_type i ) const
{
   return int( i );
}

size_type CRMode::DefaultValueIndex() const
{
   return Default;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropParameters.cpp - Released 2025-04-07T08:53:55Z
