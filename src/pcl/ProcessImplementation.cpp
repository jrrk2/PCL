//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ProcessImplementation.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Exception.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/View.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define MANDATORY( funcName ) \
   MandatoryError( meta->Id(), funcName )

static void MandatoryError( const IsoString& procId, const char* funcName )
{
   throw Error( String( procId ) + ": ProcessImplementation::" +
                funcName + "() must be reimplemented in descendant class." );
}

// ----------------------------------------------------------------------------

uint32 ProcessImplementation::Version() const
{
   return (m_serverHandle != 0) ?
      (*API->Process->GetProcessInstanceVersion)( m_serverHandle ) : meta->Version();
}

// ----------------------------------------------------------------------------

void ProcessImplementation::Assign( const ProcessImplementation& )
{
   MANDATORY( "Assign" );
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::CanExecuteOn( const View&, String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool ProcessImplementation::CanExecuteOn( const ImageVariant&, String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool ProcessImplementation::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = String( meta->Id() ) + " instances cannot be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

void ProcessImplementation::Initialize()
{
   MANDATORY( "Initialize" );
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::Validate( String& /*info*/ )
{
   MANDATORY( "Validate" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ExecuteOn( View& )
{
   MANDATORY( "ExecuteOn" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ExecuteOn( ImageVariant&, const IsoString& )
{
   MANDATORY( "ExecuteOn" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ExecuteGlobal()
{
   MANDATORY( "ExecuteGlobal" );
   return false;
}

// ----------------------------------------------------------------------------

void* ProcessImplementation::LockParameter( const MetaParameter*, size_type /*tableRow*/ )
{
   MANDATORY( "LockParameter" );
   return 0;
}

// ----------------------------------------------------------------------------

void ProcessImplementation::UnlockParameter( const MetaParameter*, size_type /*tableRow*/ )
{
   MANDATORY( "UnlockParameter" );
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ValidateParameter( void*, const MetaParameter*, size_type /*tableRow*/ ) const
{
   MANDATORY( "ValidateParameter" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::AllocateParameter( size_type /*size*/, const MetaParameter*, size_type /*tableRow*/ )
{
   MANDATORY( "AllocateParameter" );
   return false;
}

// ----------------------------------------------------------------------------

size_type ProcessImplementation::ParameterLength( const MetaParameter*, size_type /*tableRow*/ ) const
{
   MANDATORY( "ParameterLength" );
   return 0;
}

// ----------------------------------------------------------------------------

void ProcessImplementation::Launch() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::Default, 0 );
}

void ProcessImplementation::LaunchInterface() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::Interface, 0 );
}

void ProcessImplementation::LaunchGlobal() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::Global, 0 );
}

void ProcessImplementation::LaunchOnCurrentView() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::CurrentView, 0 );
}

void ProcessImplementation::LaunchOnCurrentWindow() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::CurrentWindow, 0 );
}

// ----------------------------------------------------------------------------

void ProcessImplementation::LaunchOn( View& v ) const
{
   (*API->Global->LaunchProcessInstanceOnView)( meta, this, v.handle, 0 );
}

void ProcessImplementation::LaunchOn( ImageWindow& w ) const
{
   View mv( w.MainView() );
   LaunchOn( mv );
}

// ----------------------------------------------------------------------------

String ProcessImplementation::ToSource( const IsoString& language, const IsoString& varId, int indent ) const
{
   char16_type* s = (*API->Process->GetProcessInstanceSourceCode)(
                              ModuleHandle(), m_serverHandle, language.c_str(), varId.c_str(), indent );
   if ( s == nullptr )
      throw APIFunctionError( "GetProcessInstanceSourceCode" );
   String source( s );
   Module->Deallocate( s );
   return source;
}

String ProcessImplementation::ToHistorySource( const String& xmlSource ) const
{
   String source = ToSource( "XPSM 1.0[no-read-only-parameters,no-execution-time,no-description]" );

   if ( !xmlSource.IsEmpty() )
   {
      size_type p = xmlSource.Find( "</ProcessingHistory>" );
      if ( p != String::notFound )
         return xmlSource.Left( p ) << source << "</ProcessingHistory>";
   }

   return String( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                  "<ProcessingHistory version=\"1.0\">" )
               << source
               << "</ProcessingHistory>";
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProcessImplementation.cpp - Released 2025-04-07T08:53:32Z
