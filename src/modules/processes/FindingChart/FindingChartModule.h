//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartModule.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FindingChartModule_h
#define __FindingChartModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FindingChartModule : public MetaModule
{
public:

   FindingChartModule();

   const char* Version() const override;
   IsoString Name() const override;
   String Description() const override;
   String Company() const override;
   String Author() const override;
   String Copyright() const override;
   String TradeMarks() const override;
   String OriginalFileName() const override;
   void GetReleaseDate( int& year, int& month, int& day ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FindingChartModule_h

// ----------------------------------------------------------------------------
// EOF FindingChartModule.h - Released 2025-04-07T08:53:55Z
