//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SpinStatus.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/SpinStatus.h>

#include <pcl/api/APIInterface.h>

#define THREAD_ABORT ((*API->Thread->GetThreadStatus)( m_thread ) & 0x80000000) != 0

namespace pcl
{

// ----------------------------------------------------------------------------

static const char* cspin[ 4 ] = { "&mdash;", "\\", "|", "/" };

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

int SpinStatus::Initialized( const StatusMonitor& monitor ) const
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

      m_console.Write( "<end><cbr>" + monitor.Info() + ": " + cspin[0] );
      m_spinCount = 0;
   }

   return 0;
}

// ----------------------------------------------------------------------------

int SpinStatus::Updated( const StatusMonitor& monitor ) const
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
         m_console.Write( "<end>\b*" );
         m_console.Abort();
         return 1;
      }

      m_console.Write( "<end>\b" + String( cspin[m_spinCount++%4] ) );
   }

   return 0;
}

// ----------------------------------------------------------------------------

int SpinStatus::Completed( const StatusMonitor& monitor ) const
{
   (void)Updated( monitor );
   m_console.WriteLn( m_thread ? "<end>done" : "<end>\bdone" );
   return 0;
}

// ----------------------------------------------------------------------------

void SpinStatus::InfoUpdated( const StatusMonitor& monitor ) const
{
   if ( m_thread != 0 )
   {
      m_console.WriteLn( "<end>*" );
      m_console.Write( "<end><cbr>" + monitor.Info() + ": " );
   }
   else
   {
      if ( !m_console.AbortRequested() )
         m_console.Write( "<end><bol><clreol>" + monitor.Info() + ": " + cspin[m_spinCount%4] );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SpinStatus.cpp - Released 2025-04-07T08:53:32Z
