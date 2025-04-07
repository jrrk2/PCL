//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoOperation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// NoOperationModule.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoOperation PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NoOperationModule_h
#define __NoOperationModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class NoOperationModule : public MetaModule
{
public:

   NoOperationModule();

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

#endif   // __NoOperationModule_h

// ----------------------------------------------------------------------------
// EOF NoOperationModule.h - Released 2025-04-07T08:53:56Z
