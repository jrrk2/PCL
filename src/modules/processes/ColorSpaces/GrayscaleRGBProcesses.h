//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// GrayscaleRGBProcesses.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __GrayscaleRGBProcesses_h
#define __GrayscaleRGBProcesses_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvertToGrayscaleProcess : public MetaProcess
{
public:

   ConvertToGrayscaleProcess();

   IsoString Id() const override;
   IsoString Categories() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool IsAssignable() const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ConvertToGrayscaleProcess* TheConvertToGrayscaleProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

class ConvertToRGBColorProcess : public MetaProcess
{
public:

   ConvertToRGBColorProcess();

   IsoString Id() const override;
   IsoString Categories() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool IsAssignable() const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ConvertToRGBColorProcess* TheConvertToRGBColorProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GrayscaleRGBProcesses_h

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBProcesses.h - Released 2025-04-07T08:53:55Z
