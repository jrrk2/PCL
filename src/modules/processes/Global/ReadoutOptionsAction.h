//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ReadoutOptionsAction.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ReadoutOptionsActions_h
#define __ReadoutOptionsActions_h

#include <pcl/Action.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ReadoutOptionsAction : public Action
{
public:

   ReadoutOptionsAction();

   void Execute() override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ReadoutOptionsActions_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsAction.h - Released 2025-04-07T08:53:56Z
