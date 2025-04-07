//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawModule.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RawModule_h
#define __RawModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class RawModule : public MetaModule
{
public:

   RawModule() = default;

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

#endif   // __RawModule_h

// ----------------------------------------------------------------------------
// EOF RawModule.h - Released 2025-04-07T08:53:45Z
