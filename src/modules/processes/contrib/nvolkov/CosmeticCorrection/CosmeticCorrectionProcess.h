//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.1
// ----------------------------------------------------------------------------
// CosmeticCorrectionProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CosmeticCorrectionProcess_h
#define __CosmeticCorrectionProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{
// ----------------------------------------------------------------------------

class CosmeticCorrectionProcess : public MetaProcess
{
public:

   CosmeticCorrectionProcess();

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
extern CosmeticCorrectionProcess* TheCosmeticCorrectionProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __CosmeticCorrectionProcess_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionProcess.h - Released 2025-04-07T08:53:56Z
