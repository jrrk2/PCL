//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// StatisticsProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "StatisticsProcess.h"
#include "StatisticsInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

StatisticsProcess* TheStatisticsProcess = nullptr;

// ----------------------------------------------------------------------------

StatisticsProcess::StatisticsProcess()
{
   TheStatisticsProcess = this;
}

// ----------------------------------------------------------------------------

IsoString StatisticsProcess::Id() const
{
   return "Statistics";
}

// ----------------------------------------------------------------------------

IsoString StatisticsProcess::Category() const
{
   return "Image,ImageInspection";
}

// ----------------------------------------------------------------------------

uint32 StatisticsProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String StatisticsProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String StatisticsProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Statistics.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* StatisticsProcess::DefaultInterface() const
{
   return TheStatisticsInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StatisticsProcess::Create() const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

ProcessImplementation* StatisticsProcess::Clone( const ProcessImplementation& ) const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

bool StatisticsProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StatisticsProcess.cpp - Released 2025-04-07T08:53:56Z
