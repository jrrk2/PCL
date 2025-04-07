//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SplitCFAProcess_h
#define __SplitCFAProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class SplitCFAProcess : public MetaProcess
{
public:

   SplitCFAProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
};

PCL_BEGIN_LOCAL
extern SplitCFAProcess* TheSplitCFAProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __SplitCFAProcess_h

// ----------------------------------------------------------------------------
// EOF SplitCFAProcess.h - Released 2025-04-07T08:53:56Z
