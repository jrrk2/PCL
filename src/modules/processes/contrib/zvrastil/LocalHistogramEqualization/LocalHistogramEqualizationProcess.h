//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LocalHistogramEqualizationProcess_h
#define __LocalHistogramEqualizationProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class LocalHistogramEqualizationProcess : public MetaProcess
{
public:

   LocalHistogramEqualizationProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool CanProcessCommandLines() const override;
   int ProcessCommandLine( const StringList& ) const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern LocalHistogramEqualizationProcess* TheLocalHistogramEqualizationProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __LocalHistogramEqualizationProcess_h

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationProcess.h - Released 2025-04-07T08:53:56Z
