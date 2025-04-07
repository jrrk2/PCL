//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ArcsinhStretchs_h
#define __ArcsinhStretchs_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class ArcsinhStretch : public MetaFloat
{
public:

   ArcsinhStretch( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ArcsinhStretch* TheArcsinhStretchParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchBlackPoint : public MetaFloat
{
public:

   ArcsinhStretchBlackPoint(MetaProcess*);

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern ArcsinhStretchBlackPoint* TheArcsinhStretchBlackPointParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchProtectHighlights : public MetaBoolean
{
public:

   ArcsinhStretchProtectHighlights(MetaProcess*);

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ArcsinhStretchProtectHighlights* TheArcsinhStretchProtectHighlightsParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchUseRgbws : public MetaBoolean
{
public:

   ArcsinhStretchUseRgbws(MetaProcess*);

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ArcsinhStretchUseRgbws* TheArcsinhStretchUseRgbwsParameter;

// ----------------------------------------------------------------------------

class ArcsinhStretchPreviewClipped : public MetaBoolean
{
public:

   ArcsinhStretchPreviewClipped(MetaProcess*);

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern ArcsinhStretchPreviewClipped* TheArcsinhStretchPreviewClippedParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ArcsinhStretchs_h

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchParameters.h - Released 2025-04-07T08:53:56Z
