//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MuteStatus.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/MuteStatus.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int MuteStatus::Initialized( const StatusMonitor& unused ) const
{
   m_thread = (*API->Thread->GetCurrentThread)();
   return Updated( unused );
}

// ----------------------------------------------------------------------------

int MuteStatus::Updated( const StatusMonitor& ) const
{
   if ( m_thread != 0 )
   {
      // Non-blocking thread status
      uint32 status;
      if ( (*API->Thread->GetThreadStatusEx)( m_thread, &status, 0x00000001 ) != api_false )
         if ( (status & 0x80000000) != 0 )
            return 1;
   }
   else
   {
      if ( m_console.AbortRequested() )
      {
         m_console.Abort();
         return 1;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------

int MuteStatus::Completed( const StatusMonitor& unused ) const
{
   (void)Updated( unused );
   return 0;
}

// ----------------------------------------------------------------------------

void MuteStatus::InfoUpdated( const StatusMonitor& ) const
{
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MuteStatus.cpp - Released 2025-04-07T08:53:32Z
