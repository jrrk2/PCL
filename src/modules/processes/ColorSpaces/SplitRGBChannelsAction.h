//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// SplitRGBChannelsAction.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SplitRGBChannelsAction_h
#define __SplitRGBChannelsAction_h

#include <pcl/Action.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SplitRGBChannelsAction : public Action
{
public:

   SplitRGBChannelsAction();

   void Execute() override;
   bool IsEnabled( ActionInfo info ) const override;
};

// ----------------------------------------------------------------------------

class ExtractRedChannelAction : public Action
{
public:

   ExtractRedChannelAction();

   void Execute() override;
   bool IsEnabled( ActionInfo info ) const override;
};

// ----------------------------------------------------------------------------

class ExtractGreenChannelAction : public Action
{
public:

   ExtractGreenChannelAction();

   void Execute() override;
   bool IsEnabled( ActionInfo info ) const override;
};

// ----------------------------------------------------------------------------

class ExtractBlueChannelAction : public Action
{
public:

   ExtractBlueChannelAction();

   void Execute() override;
   bool IsEnabled( ActionInfo info ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SplitRGBChannelsAction_h

// ----------------------------------------------------------------------------
// EOF SplitRGBChannelsAction.h - Released 2025-04-07T08:53:55Z
