//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// GrayscaleRGBActions.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __GrayscaleRGBActions_h
#define __GrayscaleRGBActions_h

#include <pcl/Action.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvertToGrayscaleAction : public Action
{
public:

   ConvertToGrayscaleAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

class ConvertToRGBColorAction : public Action
{
public:

   ConvertToRGBColorAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GrayscaleRGBActions_h

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBActions.h - Released 2025-04-07T08:53:55Z
