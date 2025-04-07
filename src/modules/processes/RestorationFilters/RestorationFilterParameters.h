//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 1.0.5
// ----------------------------------------------------------------------------
// RestorationFilterParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RestorationFilterParameters_h
#define __RestorationFilterParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class RFWienerK : public MetaDouble
{
public:

   RFWienerK( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFWienerK* TheRFWienerKParameter;

// ----------------------------------------------------------------------------

class RFLeastSquaresGamma : public MetaDouble
{
public:

   RFLeastSquaresGamma( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFLeastSquaresGamma* TheRFLeastSquaresGammaParameter;

// ----------------------------------------------------------------------------

class RFAlgorithm : public MetaEnumeration
{
public:

   enum { Wiener,
          ConstrainedLeastSquares,
          NumberOfAlgorithms,
          Default = ConstrainedLeastSquares };

   RFAlgorithm( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
};

extern RFAlgorithm* TheRFAlgorithmParameter;

// ----------------------------------------------------------------------------

class RFPSFMode : public MetaEnumeration
{
public:

   enum { Parametric,
          MotionBlur,
          External,
          NumberOfModes,
          Default = Parametric };

   RFPSFMode( MetaProcess* );

   IsoString Id() const override;
   size_type NumberOfElements() const override;
   IsoString ElementId( size_type ) const override;
   int ElementValue( size_type ) const override;
   size_type DefaultValueIndex() const override;
   IsoString ElementAliases() const override;
};

extern RFPSFMode* TheRFPSFModeParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricSigma : public MetaFloat
{
public:

   RFPSFParametricSigma( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricSigma* TheRFPSFParametricSigmaParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricShape : public MetaFloat
{
public:

   RFPSFParametricShape( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricShape* TheRFPSFParametricShapeParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricAspectRatio : public MetaFloat
{
public:

   RFPSFParametricAspectRatio( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricAspectRatio* TheRFPSFParametricAspectRatioParameter;

// ----------------------------------------------------------------------------

class RFPSFParametricRotationAngle : public MetaFloat
{
public:

   RFPSFParametricRotationAngle( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFParametricRotationAngle* TheRFPSFParametricRotationAngleParameter;

// ----------------------------------------------------------------------------

class RFPSFMotionLength : public MetaFloat
{
public:

   RFPSFMotionLength( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFMotionLength* TheRFPSFMotionLengthParameter;

// ----------------------------------------------------------------------------

class RFPSFMotionRotationAngle : public MetaFloat
{
public:

   RFPSFMotionRotationAngle( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFPSFMotionRotationAngle* TheRFPSFMotionRotationAngleParameter;

// ----------------------------------------------------------------------------

class RFPSFViewId : public MetaString
{
public:

   RFPSFViewId( MetaProcess* );

   IsoString Id() const override;
};

extern RFPSFViewId* TheRFPSFViewIdParameter;

// ----------------------------------------------------------------------------

class RFAmount : public MetaFloat
{
public:

   RFAmount( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFAmount* TheRFAmountParameter;

// ----------------------------------------------------------------------------

class RFToLuminance : public MetaBoolean
{
public:

   RFToLuminance( MetaProcess* );

   IsoString Id() const override;
   IsoString Aliases() const override;
   bool DefaultValue() const override;
};

extern RFToLuminance* TheRFToLuminanceParameter;

// ----------------------------------------------------------------------------

class RFLinear : public MetaBoolean
{
public:

   RFLinear( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RFLinear* TheRFLinearParameter;

// ----------------------------------------------------------------------------

class RFDeringing : public MetaBoolean
{
public:

   RFDeringing( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RFDeringing* TheRFDeringingParameter;

// ----------------------------------------------------------------------------

class RFDeringingDark : public MetaFloat
{
public:

   RFDeringingDark( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RFDeringingDark* TheRFDeringingDarkParameter;

// ----------------------------------------------------------------------------

class RFDeringingBright : public MetaFloat
{
public:

   RFDeringingBright( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern RFDeringingBright* TheRFDeringingBrightParameter;

// ----------------------------------------------------------------------------

class RFOutputDeringingMaps : public MetaBoolean
{
public:

   RFOutputDeringingMaps( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern RFOutputDeringingMaps* TheRFOutputDeringingMapsParameter;

// ----------------------------------------------------------------------------

class RFRangeLow : public MetaFloat
{
public:

   RFRangeLow( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFRangeLow* TheRFRangeLowParameter;

// ----------------------------------------------------------------------------

class RFRangeHigh : public MetaFloat
{
public:

   RFRangeHigh( MetaProcess* );

   IsoString Id() const override;
   int Precision() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   double DefaultValue() const override;
};

extern RFRangeHigh* TheRFRangeHighParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __RestorationFilterParameters_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterParameters.h - Released 2025-04-07T08:53:56Z
