//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AssistedColorCalibrationProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AssistedColorCalibrationProcess_h
#define __AssistedColorCalibrationProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class AssistedColorCalibrationProcess : public MetaProcess
{
public:

   AssistedColorCalibrationProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool CanProcessCommandLines() const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AssistedColorCalibrationProcess* TheAssistedColorCalibrationProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __AssistedColorCalibrationProcess_h

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationProcess.h - Released 2025-04-07T08:53:56Z
