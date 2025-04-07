//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MorphologicalTransformationProcess_h
#define __MorphologicalTransformationProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class MorphologicalTransformationProcess : public MetaProcess
{
public:

   MorphologicalTransformationProcess();

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

PCL_BEGIN_LOCAL
extern MorphologicalTransformationProcess* TheMorphologicalTransformationProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MorphologicalTransformationProcess_h

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationProcess.h - Released 2025-04-07T08:53:56Z
