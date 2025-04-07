//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ArcsinhStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretch*                  TheArcsinhStretchParameter = nullptr;
ArcsinhStretchBlackPoint*        TheArcsinhStretchBlackPointParameter = nullptr;
ArcsinhStretchProtectHighlights* TheArcsinhStretchProtectHighlightsParameter = nullptr;
ArcsinhStretchUseRgbws*          TheArcsinhStretchUseRgbwsParameter = nullptr;
ArcsinhStretchPreviewClipped*    TheArcsinhStretchPreviewClippedParameter = nullptr;

// ----------------------------------------------------------------------------

ArcsinhStretch::ArcsinhStretch( MetaProcess* P ) : MetaFloat( P )
{
   TheArcsinhStretchParameter = this;
}

IsoString ArcsinhStretch::Id() const
{
   return "stretch";
}

int ArcsinhStretch::Precision() const
{
   return 2;
}

double ArcsinhStretch::DefaultValue() const
{
   return 1;
}

double ArcsinhStretch::MinimumValue() const
{
   return 1;
}

double ArcsinhStretch::MaximumValue() const
{
   return 1000;
}

// ----------------------------------------------------------------------------

ArcsinhStretchBlackPoint::ArcsinhStretchBlackPoint( MetaProcess* P ) : MetaFloat( P )
{
   TheArcsinhStretchBlackPointParameter = this;
}

IsoString ArcsinhStretchBlackPoint::Id() const
{
   return "blackPoint";
}

int ArcsinhStretchBlackPoint::Precision() const
{
   return 6;
}

double ArcsinhStretchBlackPoint::DefaultValue() const
{
   return 0;
}

double ArcsinhStretchBlackPoint::MinimumValue() const
{
   return 0;
}

double ArcsinhStretchBlackPoint::MaximumValue() const
{
   return 0.2;
}

// ----------------------------------------------------------------------------

ArcsinhStretchProtectHighlights::ArcsinhStretchProtectHighlights( MetaProcess* P ) : MetaBoolean( P )
{
   TheArcsinhStretchProtectHighlightsParameter = this;
}

IsoString ArcsinhStretchProtectHighlights::Id() const
{
   return "protectHighlights";
}

bool ArcsinhStretchProtectHighlights::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ArcsinhStretchUseRgbws::ArcsinhStretchUseRgbws( MetaProcess* P ) : MetaBoolean( P )
{
   TheArcsinhStretchUseRgbwsParameter = this;
}

IsoString ArcsinhStretchUseRgbws::Id() const
{
   return "useRGBWS";
}

bool ArcsinhStretchUseRgbws::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ArcsinhStretchPreviewClipped::ArcsinhStretchPreviewClipped( MetaProcess* P ) : MetaBoolean( P )
{
   TheArcsinhStretchPreviewClippedParameter = this;
}

IsoString ArcsinhStretchPreviewClipped::Id() const
{
   return "previewClipped";
}

bool ArcsinhStretchPreviewClipped::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchParameters.cpp - Released 2025-04-07T08:53:56Z
