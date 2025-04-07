//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// STFAutoStretchAction.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __STFAutoStretchAction_h
#define __STFAutoStretchAction_h

#include <pcl/Action.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class STFAutoStretchActionBase : public Action
{
public:

   STFAutoStretchActionBase( bool boost, const String& menuItem, const char* icon );

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;

private:

   bool m_boost;
};

// ----------------------------------------------------------------------------

class STFAutoStretchAction : public STFAutoStretchActionBase
{
public:

   STFAutoStretchAction();
};

// ----------------------------------------------------------------------------

class STFAutoStretchBoostAction : public STFAutoStretchActionBase
{
public:

   STFAutoStretchBoostAction();
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class STFAutoStretchToolBarActionBase : public Action
{
public:

   STFAutoStretchToolBarActionBase( bool boost, const char* icon, const String& toolBar );

   void Execute() override;
   bool IsEnabled( ActionInfo ) const override;

private:

   bool m_boost;

protected:

   static String AdditionalInformation();
};

// ----------------------------------------------------------------------------

class STFAutoStretchToolBarAction : public STFAutoStretchToolBarActionBase
{
public:

   STFAutoStretchToolBarAction();
};

// ----------------------------------------------------------------------------

class STFAutoStretchToolBarBoostAction : public STFAutoStretchToolBarActionBase
{
public:

   STFAutoStretchToolBarBoostAction();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __STFAutoStretchAction_h

// ----------------------------------------------------------------------------
// EOF STFAutoStretchAction.h - Released 2025-04-07T08:53:56Z
