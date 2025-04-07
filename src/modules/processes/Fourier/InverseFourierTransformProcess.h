//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// InverseFourierTransformProcess.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __InverseFourierTransformProcess_h
#define __InverseFourierTransformProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class InverseFourierTransformProcess : public MetaProcess
{
public:

   InverseFourierTransformProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
};

// ----------------------------------------------------------------------------

extern InverseFourierTransformProcess* TheInverseFourierTransformProcess;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __InverseFourierTransformProcess_h

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformProcess.h - Released 2025-04-07T08:53:55Z
