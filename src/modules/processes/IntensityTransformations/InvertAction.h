//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// InvertAction.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __InvertAction_h
#define __InvertAction_h

#include <pcl/Action.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class InvertAction : public Action
{
public:

   InvertAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __InvertAction_h

// ----------------------------------------------------------------------------
// EOF InvertAction.h - Released 2025-04-07T08:53:56Z
