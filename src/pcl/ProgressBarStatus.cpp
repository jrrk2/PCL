//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ProgressBarStatus.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/MetaModule.h>
#include <pcl/ProgressBarStatus.h>
#include <pcl/ProgressDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ProgressBarStatus::ProgressBarStatus( const String& title, Control& parent )
   : m_progressDialog( new ProgressDialog( String()/*text*/
                                          , title
                                          , 0/*lowerBound*/, 100/*upperBound*/
                                          , parent ) )
{
}

// ----------------------------------------------------------------------------

ProgressBarStatus::~ProgressBarStatus()
{
}

// ----------------------------------------------------------------------------

int ProgressBarStatus::Initialized( const StatusMonitor& monitor ) const
{
   if ( m_progressDialog->IsCanceled() )
      return 1;
   m_progressDialog->SetRange( 0u, monitor.Total() );
   m_progressDialog->SetText( monitor.Info() );
   m_progressDialog->Open();
   m_progressDialog->Update();
   Module->ProcessEvents();
   return 0;
}

// ----------------------------------------------------------------------------

int ProgressBarStatus::Updated( const StatusMonitor& monitor ) const
{
   if ( m_progressDialog->IsCanceled() )
      return 1;
   m_progressDialog->SetValue( monitor.Count() );
   m_progressDialog->Update();
   Module->ProcessEvents();
   return 0;
}

// ----------------------------------------------------------------------------

int ProgressBarStatus::Completed( const StatusMonitor& monitor ) const
{
   if ( m_progressDialog->IsCanceled() )
      return 1;
   m_progressDialog->SetValue( monitor.Total() );
   m_progressDialog->Update();
   m_progressDialog->Ok();
   Module->ProcessEvents();
   return 0;
}

// ----------------------------------------------------------------------------

void ProgressBarStatus::InfoUpdated( const StatusMonitor& monitor ) const
{
   m_progressDialog->SetText( monitor.Info() );
   m_progressDialog->Update();
   Module->ProcessEvents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProgressBarStatus.cpp - Released 2025-04-07T08:53:32Z
