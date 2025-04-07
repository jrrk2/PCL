//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.9
// ----------------------------------------------------------------------------
// CometAlignmentProcess.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CometAlignmentProcess_h
#define __CometAlignmentProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CometAlignmentProcess : public MetaProcess
{
public:

   CometAlignmentProcess();

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
extern CometAlignmentProcess* TheCometAlignmentProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __CometAlignmentProcess_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentProcess.h - Released 2025-04-07T08:53:56Z
