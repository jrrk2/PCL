//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/UIScaling.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/UIScaling.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PCL_DATA const double PCL_UIScalingFactor_LUT[PCL_UIScaling_LUT_Length] =
                              { 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0 };
PCL_DATA const char*  PCL_UIScalingSubdir_LUT[PCL_UIScaling_LUT_Length] =
                              { "", "1.5", "2.0", "2.5", "3.0", "3.5", "4.0" };

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/UIScaling.cpp - Released 2025-04-07T08:53:32Z
