//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierTransformParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FourierTransformParameters_h
#define __FourierTransformParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FTRadialCoordinates : public MetaBoolean
{
public:

   FTRadialCoordinates( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FTRadialCoordinates* TheFTRadialCoordinatesParameter;

// ----------------------------------------------------------------------------

class FTCentered : public MetaBoolean
{
public:

   FTCentered( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern FTCentered* TheFTCenteredParameter;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FourierTransformParameters_h

// ----------------------------------------------------------------------------
// EOF FourierTransformParameters.h - Released 2025-04-07T08:53:55Z
