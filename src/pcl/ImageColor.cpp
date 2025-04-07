//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageColor.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ImageColor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * The default RGBWS for local images is the sRGB space.
 *
 * Note that we cannot depend on RGBColorSystem::sRGB here, since that RGB
 * space could be uninitialized at this point.
 */
RGBColorSystem ImageColor::s_defaultRGBWS =
               RGBColorSystem( 2.2F, true,
                               RGBColorSystem::sRGB_x, RGBColorSystem::sRGB_y,
                               RGBColorSystem::sRGB_Y );

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageColor.cpp - Released 2025-04-07T08:53:32Z
