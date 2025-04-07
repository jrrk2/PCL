//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// RotationParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RotationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RotationAngle*          TheRTRotationAngleParameter = nullptr;
OptimizeFastRotations*  TheRTOptimizeFastRotationsParameter = nullptr;
InterpolationAlgorithm* TheRTInterpolationAlgorithmParameter = nullptr;
ClampingThreshold*      TheRTClampingThresholdParameter = nullptr;
Smoothness*             TheRTSmoothnessParameter = nullptr;
GammaCorrection*        TheRTGammaCorrectionParameter = nullptr;
FillRed*                TheRTFillRedParameter = nullptr;
FillGreen*              TheRTFillGreenParameter = nullptr;
FillBlue*               TheRTFillBlueParameter = nullptr;
FillAlpha*              TheRTFillAlphaParameter = nullptr;
NoGUIMessages*          TheRTNoGUIMessagesParameter = nullptr;

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RotationParameters.cpp - Released 2025-04-07T08:53:55Z
