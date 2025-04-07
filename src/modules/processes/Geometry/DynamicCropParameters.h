//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __DynamicCropParameters_h
#define __DynamicCropParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class DCRefWidth : public MetaInt32
{
public:

   DCRefWidth( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DCRefWidth* TheDCRefWidthParameter;

// ----------------------------------------------------------------------------

class DCRefHeight : public MetaInt32
{
public:

   DCRefHeight( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern DCRefHeight* TheDCRefHeightParameter;

// ----------------------------------------------------------------------------

class DCOutWidth : public MetaDouble
{
public:

   DCOutWidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCOutWidth* TheDCOutWidthParameter;

// ----------------------------------------------------------------------------

class DCOutHeight : public MetaDouble
{
public:

   DCOutHeight( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCOutHeight* TheDCOutHeightParameter;

// ----------------------------------------------------------------------------

class DCCenterX : public MetaDouble
{
public:

   DCCenterX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCCenterX* TheDCCenterXParameter;

// ----------------------------------------------------------------------------

class DCCenterY : public MetaDouble
{
public:

   DCCenterY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCCenterY* TheDCCenterYParameter;

// ----------------------------------------------------------------------------

class DCWidth : public MetaDouble
{
public:

   DCWidth( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCWidth* TheDCWidthParameter;

// ----------------------------------------------------------------------------

class DCHeight : public MetaDouble
{
public:

   DCHeight( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCHeight* TheDCHeightParameter;

// ----------------------------------------------------------------------------

extern RotationAngle* TheDCRotationAngleParameter;

// ----------------------------------------------------------------------------

class DCScaleX : public MetaDouble
{
public:

   DCScaleX( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCScaleX* TheDCScaleXParameter;

// ----------------------------------------------------------------------------

class DCScaleY : public MetaDouble
{
public:

   DCScaleY( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
};

extern DCScaleY* TheDCScaleYParameter;

// ----------------------------------------------------------------------------

extern OptimizeFastRotations* TheDCOptimizeFastRotationsParameter;

extern InterpolationAlgorithm* TheDCInterpolationAlgorithmParameter;
extern ClampingThreshold* TheDCClampingThresholdParameter;
extern Smoothness* TheDCSmoothnessParameter;
extern GammaCorrection* TheDCGammaCorrectionParameter;

extern XResolution* TheDCXResolutionParameter;
extern YResolution* TheDCYResolutionParameter;
extern MetricResolution* TheDCMetricResolutionParameter;
extern ForceResolution* TheDCForceResolutionParameter;

extern FillRed* TheDCFillRedParameter;
extern FillGreen* TheDCFillGreenParameter;
extern FillBlue* TheDCFillBlueParameter;
extern FillAlpha* TheDCFillAlphaParameter;

extern NoGUIMessages* TheDCNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __DynamicCropParameters_h

// ----------------------------------------------------------------------------
// EOF DynamicCropParameters.h - Released 2025-04-07T08:53:55Z
