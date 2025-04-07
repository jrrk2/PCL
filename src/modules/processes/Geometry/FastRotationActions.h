//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationActions.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FastRotationActions_h
#define __FastRotationActions_h

#include <pcl/Action.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class Rotate180Action : public Action
{
public:

   Rotate180Action();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

class Rotate90CWAction : public Action
{
public:

   Rotate90CWAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

class Rotate90CCWAction : public Action
{
public:

   Rotate90CCWAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

class HorizontalMirrorAction : public Action
{
public:

   HorizontalMirrorAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

class VerticalMirrorAction : public Action
{
public:

   VerticalMirrorAction();

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FastRotationActions_h

// ----------------------------------------------------------------------------
// EOF FastRotationActions.h - Released 2025-04-07T08:53:55Z
