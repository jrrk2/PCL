//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// NewImageProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NewImageProcess_h
#define __NewImageProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class NewImageProcess : public MetaProcess
{
public:

   NewImageProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   String Description() const override;
   uint32 Version() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool NeedsValidation() const override;
   bool PrefersGlobalExecution() const override;
   bool CanProcessCommandLines() const override;
   int ProcessCommandLine( const StringList& argv ) const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NewImageProcess* TheNewImageProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NewImageProcess_h

// ----------------------------------------------------------------------------
// EOF NewImageProcess.h - Released 2025-04-07T08:53:56Z
