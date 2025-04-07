//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CropParameters_h
#define __CropParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class CRLeftMargin : public MetaDouble
{
public:

   CRLeftMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRLeftMargin* TheCRLeftMarginParameter;

// ----------------------------------------------------------------------------

class CRTopMargin : public MetaDouble
{
public:

   CRTopMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRTopMargin* TheCRTopMarginParameter;

// ----------------------------------------------------------------------------

class CRRightMargin : public MetaDouble
{
public:

   CRRightMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRRightMargin* TheCRRightMarginParameter;

// ----------------------------------------------------------------------------

class CRBottomMargin : public MetaDouble
{
public:

   CRBottomMargin( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
};

extern CRBottomMargin* TheCRBottomMarginParameter;

// ----------------------------------------------------------------------------

class CRMode : public MetaEnumeration
{
public:

   enum { RelativeMargins,       // margins relative to current image dimensions
          AbsolutePixels,        // absolute margins in pixels
          AbsoluteCentimeters,   // absolute margins in centimeters
          AbsoluteInches,        // absolute margins in inches
          NumberOfCRModes,
          Default = AbsolutePixels };

   CRMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern CRMode* TheCRModeParameter;

// ----------------------------------------------------------------------------

extern XResolution* TheCRXResolutionParameter;
extern YResolution* TheCRYResolutionParameter;
extern MetricResolution* TheCRMetricResolutionParameter;
extern ForceResolution* TheCRForceResolutionParameter;

extern FillRed* TheCRFillRedParameter;
extern FillGreen* TheCRFillGreenParameter;
extern FillBlue* TheCRFillBlueParameter;
extern FillAlpha* TheCRFillAlphaParameter;

extern NoGUIMessages* TheCRNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __CropParameters_h

// ----------------------------------------------------------------------------
// EOF CropParameters.h - Released 2025-04-07T08:53:55Z
