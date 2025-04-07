//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ConvolutionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConMode*            TheConModeParameter = nullptr;
ConSigma*           TheConSigmaParameter = nullptr;
ConShape*           TheConShapeParameter = nullptr;
ConAspectRatio*     TheConAspectRatioParameter = nullptr;
ConRotationAngle*   TheConRotationAngleParameter = nullptr;
ConSourceCode*      TheConSourceCodeParameter = nullptr;
ConRescaleHighPass* TheConRescaleHighPassParameter;
ConViewId*          TheConViewIdParameter = nullptr;

// ----------------------------------------------------------------------------

ConMode::ConMode( MetaProcess* P ) : MetaEnumeration( P )
{
   TheConModeParameter = this;
}

IsoString ConMode::Id() const
{
   return "mode";
}

size_type ConMode::NumberOfElements() const
{
    return NumberOfModes;
}

IsoString ConMode::ElementId( size_type i) const
{
   switch ( i )
   {
   default:
   case Parametric: return "Parametric";
   case Library:    return "Library";
   case Image:      return "Image";
   }
}

int ConMode::ElementValue( size_type i ) const
{
    return int( i );
}

size_type ConMode::DefaultValueIndex() const
{
   return size_type( Default );
}

IsoString ConMode::ElementAliases() const
{
   return "Gaussian=Parametric";
}

// ----------------------------------------------------------------------------

ConSigma::ConSigma( MetaProcess* P ) : MetaFloat( P )
{
   TheConSigmaParameter = this;
}

IsoString ConSigma::Id() const
{
   return "sigma";
}

int ConSigma::Precision() const
{
   return 2;
}

double ConSigma::MinimumValue() const
{
   return 0.1;
}

double ConSigma::MaximumValue() const
{
   return 250;
}

double ConSigma::DefaultValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ConShape::ConShape( MetaProcess* P ) : MetaFloat( P )
{
   TheConShapeParameter = this;
}

IsoString ConShape::Id() const
{
   return "shape";
}

int ConShape::Precision() const
{
   return 2;
}

double ConShape::MinimumValue() const
{
   return 0.25;
}

double ConShape::MaximumValue() const
{
   return 6;
}

double ConShape::DefaultValue() const
{
   return 2;
}

// ----------------------------------------------------------------------------

ConAspectRatio::ConAspectRatio( MetaProcess* P ) : MetaFloat( P )
{
   TheConAspectRatioParameter = this;
}

IsoString ConAspectRatio::Id() const
{
   return "aspectRatio";
}

int ConAspectRatio::Precision() const
{
   return 2;
}

double ConAspectRatio::MinimumValue() const
{
   return 0;
}

double ConAspectRatio::MaximumValue() const
{
   return 1;
}

double ConAspectRatio::DefaultValue() const
{
   return 1;
}

// ----------------------------------------------------------------------------

ConRotationAngle::ConRotationAngle( MetaProcess* P ) : MetaFloat( P )
{
   TheConRotationAngleParameter = this;
}

IsoString ConRotationAngle::Id() const
{
   return "rotationAngle";
}

int ConRotationAngle::Precision() const
{
   return 2;
}

double ConRotationAngle::MinimumValue() const
{
   return 0;
}

double ConRotationAngle::MaximumValue() const
{
   return 180;
}

double ConRotationAngle::DefaultValue() const
{
   return 0;
}

// ----------------------------------------------------------------------------

ConSourceCode::ConSourceCode( MetaProcess* P ) : MetaString( P )
{
   TheConSourceCodeParameter = this;
}

IsoString ConSourceCode::Id() const
{
   return "filterSource";
}

// ----------------------------------------------------------------------------

ConRescaleHighPass::ConRescaleHighPass( MetaProcess* P ) : MetaBoolean( P )
{
   TheConRescaleHighPassParameter = this;
}

IsoString ConRescaleHighPass::Id() const
{
   return "rescaleHighPass";
}

bool ConRescaleHighPass::DefaultValue() const
{
   return false;
}

// ----------------------------------------------------------------------------

ConViewId::ConViewId( MetaProcess* P ) : MetaString( P )
{
   TheConViewIdParameter = this;
}

IsoString ConViewId::Id() const
{
   return "viewId";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ConvolutionParameters.cpp - Released 2025-04-07T08:53:55Z
