//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIDeviceControllerProcess_h
#define __INDIDeviceControllerProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIDeviceControllerProcess : public MetaProcess
{
public:

   INDIDeviceControllerProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   bool PrefersGlobalExecution() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool CanProcessCommandLines() const override;
   int ProcessCommandLine( const StringList& ) const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIDeviceControllerProcess* TheINDIDeviceControllerProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIDeviceControllerProcess_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerProcess.h - Released 2025-04-07T08:53:56Z
