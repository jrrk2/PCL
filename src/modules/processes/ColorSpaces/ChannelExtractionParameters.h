//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelExtractionParameters.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelExtractionParameters_h
#define __ChannelExtractionParameters_h

#include "ChannelParameters.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

extern ColorSpaceId*                 TheColorSpaceIdExtractionParameter;
extern ChannelTable*                 TheChannelTableExtractionParameter;
extern ChannelEnabled*               TheChannelEnabledExtractionParameter;
extern ChannelId*                    TheChannelIdExtractionParameter;
extern ChannelSampleFormat*          TheChannelSampleFormatExtractionParameter;
extern CSInheritAstrometricSolution* TheCEInheritAstrometricSolutionParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // pcl

#endif   // __ChannelExtractionParameters_h

// ----------------------------------------------------------------------------
// EOF ChannelExtractionParameters.h - Released 2025-04-07T08:53:55Z
