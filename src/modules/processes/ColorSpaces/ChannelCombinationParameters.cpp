//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelCombinationParameters.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelCombinationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSpaceId*                 TheColorSpaceIdCombinationParameter = nullptr;
ChannelTable*                 TheChannelTableCombinationParameter = nullptr;
ChannelEnabled*               TheChannelEnabledCombinationParameter = nullptr;
ChannelId*                    TheChannelIdCombinationParameter = nullptr;
CSInheritAstrometricSolution* TheCCInheritAstrometricSolutionParameter = nullptr;

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelCombinationParameters.cpp - Released 2025-04-07T08:53:55Z
