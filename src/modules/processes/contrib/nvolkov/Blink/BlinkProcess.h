//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// BlinkProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BlinkProcess_h
#define __BlinkProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BlinkProcess : public MetaProcess
{
public:

   BlinkProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool IsAssignable() const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BlinkProcess* TheBlinkProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BlinkProcess_h

// ----------------------------------------------------------------------------
// EOF BlinkProcess.h - Released 2025-04-07T08:53:56Z
