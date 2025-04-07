//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceDownloader.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "NetworkServiceDownloader.h"
#include "NetworkServiceProcess.h"

#include <pcl/MetaModule.h>
#include <pcl/NetworkTransfer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void NetworkServiceDownloader::Attach( NetworkTransfer& transfer )
{
   data.Clear();
   transfer.OnDownloadDataAvailable( (NetworkTransfer::download_event_handler)&NetworkServiceDownloader::e_DataAvailable, *this );
}

// ----------------------------------------------------------------------------

bool NetworkServiceDownloader::e_DataAvailable( NetworkTransfer& sender, const void* buffer, fsize_type size )
{
   Module->ProcessEvents();
   if ( TheNetworkServiceProcess->StopRequested() )
      return false;

   const uint8* chunk = reinterpret_cast<const uint8*>( buffer );
   data.Append( chunk, chunk+size );
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NetworkServiceDownloader.cpp - Released 2025-04-07T08:53:56Z
