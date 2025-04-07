//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierTransformParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FourierTransformParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FTRadialCoordinates* TheFTRadialCoordinatesParameter = nullptr;
FTCentered*          TheFTCenteredParameter = nullptr;

// ----------------------------------------------------------------------------

FTRadialCoordinates::FTRadialCoordinates( MetaProcess* P ) : MetaBoolean( P )
{
   TheFTRadialCoordinatesParameter = this;
}

IsoString FTRadialCoordinates::Id() const
{
   return "radialCoordinates";
}

bool FTRadialCoordinates::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

FTCentered::FTCentered( MetaProcess* P ) : MetaBoolean( P )
{
   TheFTCenteredParameter = this;
}

IsoString FTCentered::Id() const
{
   return "centered";
}

bool FTCentered::DefaultValue() const
{
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FourierTransformParameters.cpp - Released 2025-04-07T08:53:55Z
