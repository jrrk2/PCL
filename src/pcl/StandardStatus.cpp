//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/StandardStatus.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Math.h>
#include <pcl/StandardStatus.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Non-blocking thread status
 */
inline static
bool ThreadAborted( void* thread )
{
   uint32 status;
   return (*API->Thread->GetThreadStatusEx)( thread, &status, 0x00000001 ) != api_false &&
          (status & 0x80000000) != 0;
}

// ----------------------------------------------------------------------------

int StandardStatus::Initialized( const StatusMonitor& monitor ) const
{
   m_thread = (*API->Thread->GetCurrentThread)();
   if ( m_thread != 0 )
   {
      if ( ThreadAborted( m_thread ) )
         return 1;

      m_console.Write( "<end><cbr>" + monitor.Info() + ": " );
   }
   else
   {
      if ( m_console.AbortRequested() )
      {
         m_console.Abort();
         return 1;
      }

      m_console.Write( "<end><cbr>" + monitor.Info() + ":   0%" );
      m_last = -1;
   }

   return 0;
}

// ----------------------------------------------------------------------------

int StandardStatus::Updated( const StatusMonitor& monitor ) const
{
   if ( m_thread != 0 )
   {
      if ( ThreadAborted( m_thread ) )
      {
         m_console.WriteLn( "<end>*" );
         return 1;
      }
   }
   else
   {
      if ( m_console.AbortRequested() )
      {
         m_console.Abort();
         return 1;
      }

      int percent = pcl::Range( pcl::RoundInt( 100*(double( monitor.Count() )/monitor.Total()) ), 0, 100 );
      if ( percent != m_last )
      {
         if ( percent == 100 )
            m_console.Write( "<end>\b\b\b\bdone" );
         else
            m_console.Write( String().Format( "<end>\b\b\b\b%3d%%", percent ) );
         m_last = percent;
      }
   }

   return 0;
}

// ----------------------------------------------------------------------------

int StandardStatus::Completed( const StatusMonitor& monitor ) const
{
   (void)Updated( monitor );
   m_console.WriteLn( m_thread ? "<end>done" : "<end>" );
   return 0;
}

// ----------------------------------------------------------------------------

void StandardStatus::InfoUpdated( const StatusMonitor& monitor ) const
{
   if ( m_thread != 0 )
      m_console.Write( "<end><cbr>" + monitor.Info() + ": " );
   else
   {
      if ( !m_console.AbortRequested() )
         m_console.Write( "<end><bol><clreol>" + monitor.Info() + ":   0%" );
      m_last = -1;
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/StandardStatus.cpp - Released 2025-04-07T08:53:32Z
