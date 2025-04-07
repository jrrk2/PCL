//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelExtractionProcess.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelExtractionProcess_h
#define __ChannelExtractionProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelExtractionProcess : public MetaProcess
{
public:

   ChannelExtractionProcess();

   IsoString Id() const override;
   IsoString Categories() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool NeedsValidation() const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ChannelExtractionProcess* TheChannelExtractionProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelExtractionProcess_h

// ----------------------------------------------------------------------------
// EOF ChannelExtractionProcess.h - Released 2025-04-07T08:53:55Z
