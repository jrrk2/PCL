//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// RotationParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RotationParameters_h
#define __RotationParameters_h

#include "CommonParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

extern RotationAngle* TheRTRotationAngleParameter;
extern OptimizeFastRotations* TheRTOptimizeFastRotationsParameter;

extern InterpolationAlgorithm* TheRTInterpolationAlgorithmParameter;
extern ClampingThreshold* TheRTClampingThresholdParameter;
extern Smoothness* TheRTSmoothnessParameter;
extern GammaCorrection* TheRTGammaCorrectionParameter;

extern FillRed* TheRTFillRedParameter;
extern FillGreen* TheRTFillGreenParameter;
extern FillBlue* TheRTFillBlueParameter;
extern FillAlpha* TheRTFillAlphaParameter;

extern NoGUIMessages* TheRTNoGUIMessagesParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __RotationParameters_h

// ----------------------------------------------------------------------------
// EOF RotationParameters.h - Released 2025-04-07T08:53:56Z
