//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard APASS Process Module Version 1.1.5
// ----------------------------------------------------------------------------
// APASSModule.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard APASS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __APASSModule_h
#define __APASSModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class APASSModule : public MetaModule
{
public:

   APASSModule();

   const char* Version() const override;
   IsoString Name() const override;
   String Description() const override;
   String Company() const override;
   String Author() const override;
   String Copyright() const override;
   String TradeMarks() const override;
   String OriginalFileName() const override;
   void GetReleaseDate( int& year, int& month, int& day ) const override;
   void OnUnload() override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __APASSModule_h

// ----------------------------------------------------------------------------
// EOF APASSModule.h - Released 2025-04-07T08:53:55Z
