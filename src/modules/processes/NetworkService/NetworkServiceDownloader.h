//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceDownloader.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NetworkServiceDownloader_h
#define __NetworkServiceDownloader_h

#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS NetworkTransfer;

class NetworkServiceDownloader : public Control
{
public:

   ByteArray data;

   void Attach( NetworkTransfer& );

private:

   bool e_DataAvailable( NetworkTransfer& sender, const void* buffer, fsize_type size );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceDownloader_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceDownloader.h - Released 2025-04-07T08:53:56Z
