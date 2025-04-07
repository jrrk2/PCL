//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceTask.cpp - Released 2025-04-07T08:53:56Z
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
#include "NetworkServiceLogger.h"
#include "NetworkServiceProcess.h"
#include "NetworkServiceTask.h"

#include <pcl/Exception.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/ReferenceSortedArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static ReferenceSortedArray<NetworkServiceTask> s_tasks;

// ----------------------------------------------------------------------------

class NetworkServiceTaskByType : public NetworkServiceTask
{
public:

   NetworkServiceTaskByType( const String& name )
      : m_name( name )
   {
   }

   String TaskType() const override
   {
      return m_name;
   }

   String TaskVersion() const override
   {
      return String();
   }

   void Perform( const TaskData&, NetworkServiceLogger& ) override
   {
   }

private:

   String m_name;
};

// ----------------------------------------------------------------------------

String NetworkServiceTask::TaskType() const
{
   return "DEFAULT_TASK";
}

// ----------------------------------------------------------------------------

String NetworkServiceTask::TaskVersion() const
{
   return "1.0";
}

// ----------------------------------------------------------------------------

void NetworkServiceTask::Perform( const TaskData& data, NetworkServiceLogger& log )
{
   /*
    * This is the default task, which simply sends back a notification to
    * identify the task for testing purposes.
    *
    * You must reimplement this member function in derived classes to implement
    * actual network services.
    */

   StringList parameters;
   data.parameters.Break( parameters, '\n', true/*trim*/ );
   parameters.Remove( String() );

   log.Line( "Task parameters :" );
   log.Line( String().ToNewLineSeparated( parameters ) );

   log.Section( "Uploading result: " + data.callbackURL );
   {
      String postVars;
      postVars << "serialNumber=" << data.serialNumber
               << "&taskType=" << TaskType()
               << "&workerName=" << (data.workerName.IsEmpty() ? "anonymous" : data.workerName)
               << "&status=OK"
               << "&taskVersion=" << TaskVersion();

      NetworkTransfer transfer;
      transfer.SetURL( data.callbackURL );
      transfer.SetSSL( true/*enableSSL*/, false/*forceSSL*/ );

      NetworkServiceDownloader download;
      download.Attach( transfer );

      if ( !transfer.POST( postVars ) )
         if ( !TheNetworkServiceProcess->StopRequested() )
            throw Error( "POST operation failed: " + transfer.ErrorInformation() );

      if ( !TheNetworkServiceProcess->StopRequested() )
         log.Section( "POST operation successful. Callback result:\n" + IsoString::ToURLDecoded( download.data ).UTF8ToUTF16() );
   }
}

// ----------------------------------------------------------------------------

NetworkServiceTask* NetworkServiceTask::TaskByType( const String& taskName )
{
   ReferenceSortedArray<NetworkServiceTask>::const_iterator i = s_tasks.Search( NetworkServiceTaskByType( taskName ) );
   return (i == s_tasks.End()) ? nullptr : const_cast<NetworkServiceTask*>( (const NetworkServiceTask*)i );
}

// ----------------------------------------------------------------------------

void NetworkServiceTask::RegisterTask( NetworkServiceTask* task )
{
   if ( task != nullptr )
      if ( !s_tasks.Contains( *task ) )
         s_tasks.Add( task );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NetworkServiceTask.cpp - Released 2025-04-07T08:53:56Z
