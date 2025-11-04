//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ExternalProcess.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/AutoLock.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ExternalProcess.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ExternalProcessPrivate
{
public:

   static api_bool EnvironmentEnumerationCallback( const char16_type* var, void* data )
   {
#define environment  reinterpret_cast<StringList*>( data )
      try
      {
         environment->Append( String( var ) );
         return api_true;
      }
      catch ( ... )
      {
         return api_false;
      }
#undef environment
   }

   static void Throw( int apiErrorCode )
   {
      String errorMessage = "ExternalProcess: ";
      /*
	switch ( apiErrorCode )
      {
      case ExternalProcessContext::FailedToStart:
         errorMessage += "Failed to start process";
         break;
      case ExternalProcessContext::Crashed:
         errorMessage += "The process crashed";
         break;
      case ExternalProcessContext::TimedOut:
         errorMessage += "The process timed out";
         break;
      case ExternalProcessContext::ReadError:
         errorMessage += "Read error";
         break;
      case ExternalProcessContext::WriteError:
         errorMessage += "Write error";
         break;
      default:
      case ExternalProcessContext::UnknownError:
         errorMessage += "Unknown error";
         break;
      case ExternalProcessContext::InvalidError:
         errorMessage += "Internal error: Invalid object";
         break;
      }
      */
      throw Error( errorMessage );
   }
};

// ----------------------------------------------------------------------------

ExternalProcess::ExternalProcess()
   : UIObject( API_ExternalProcess_CreateExternalProcess( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateExternalProcess" );
}

// ----------------------------------------------------------------------------

ExternalProcess::ExternalProcess( void* h )
   : UIObject( h )
{
}

// ----------------------------------------------------------------------------

ExternalProcess& ExternalProcess::Null()
{
   static ExternalProcess* nullExternalProcess = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullExternalProcess == nullptr )
      nullExternalProcess = new ExternalProcess( nullptr );
   return *nullExternalProcess;
}

// ----------------------------------------------------------------------------

void ExternalProcess::Start( const String& program, const StringList& arguments )
{
   Array<const char16_type*> argv;
   for ( const String& arg : arguments )
      argv.Add( arg.c_str() );
   if ( API_ExternalProcess_StartExternalProcess( handle, program.c_str(), argv.Begin(), argv.Length() ) == api_false )
      throw APIFunctionError( "StartExternalProcess" );
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForStarted( int ms )
{
   return API_ExternalProcess_WaitForExternalProcessStarted( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForFinished( int ms )
{
   return API_ExternalProcess_WaitForExternalProcessFinished( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForDataAvailable( int ms )
{
   return API_ExternalProcess_WaitForExternalProcessDataAvailable( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::WaitForDataWritten( int ms )
{
   return API_ExternalProcess_WaitForExternalProcessDataWritten( handle, ms ) != api_false;
}

// ----------------------------------------------------------------------------

void ExternalProcess::Terminate()
{
   if ( API_ExternalProcess_TerminateExternalProcess( handle ) == api_false )
      throw APIFunctionError( "TerminateExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Kill()
{
   if ( API_ExternalProcess_KillExternalProcess( handle ) == api_false )
      throw APIFunctionError( "KillExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::CloseStandardInput()
{
   if ( API_ExternalProcess_CloseExternalProcessStream( handle, ExternalProcessContext::StandardInput ) == api_false )
      throw APIFunctionError( "CloseExternalProcessStream" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::CloseStandardOutput()
{
   if ( API_ExternalProcess_CloseExternalProcessStream( handle, ExternalProcessContext::StandardOutput ) == api_false )
      throw APIFunctionError( "CloseExternalProcessStream" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::CloseStandardError()
{
   if ( API_ExternalProcess_CloseExternalProcessStream( handle, ExternalProcessContext::StandardError ) == api_false )
      throw APIFunctionError( "CloseExternalProcessStream" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardOutput( const String& filePath, bool append )
{
   if ( API_ExternalProcess_RedirectExternalProcessToFile( handle,
               ExternalProcessContext::StandardOutput, filePath.c_str(), append ) == api_false )
      throw APIFunctionError( "RedirectExternalProcessToFile" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardOutput( ExternalProcess& process )
{
   if ( API_ExternalProcess_PipeExternalProcess( handle,
               ExternalProcessContext::StandardOutput, process.handle ) == api_false )
      throw APIFunctionError( "PipeExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardError( const String& filePath, bool append )
{
   if ( API_ExternalProcess_RedirectExternalProcessToFile( handle,
               ExternalProcessContext::StandardError, filePath.c_str(), append ) == api_false )
      throw APIFunctionError( "RedirectExternalProcessToFile" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::RedirectStandardInput( const String& filePath )
{
   if ( API_ExternalProcess_RedirectExternalProcessToFile( handle,
               ExternalProcessContext::StandardInput, filePath.c_str(), api_false/*append*/ ) == api_false )
      throw APIFunctionError( "RedirectExternalProcessToFile" );
}

// ----------------------------------------------------------------------------

String ExternalProcess::WorkingDirectory() const
{
   size_type len = 0;
   API_ExternalProcess_GetExternalProcessWorkingDirectory( handle, 0, &len );

   String dirPath;
   if ( len > 0 )
   {
      dirPath.SetLength( len );
      if ( API_ExternalProcess_GetExternalProcessWorkingDirectory( handle, dirPath.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetExternalProcessWorkingDirectory" );
      dirPath.ResizeToNullTerminated();
   }
   return dirPath;
}

// ----------------------------------------------------------------------------

void ExternalProcess::SetWorkingDirectory( const String& dirPath )
{
   if ( API_ExternalProcess_SetExternalProcessWorkingDirectory( handle, dirPath.c_str() ) == api_false )
      throw APIFunctionError( "SetExternalProcessWorkingDirectory" );
}

// ----------------------------------------------------------------------------

bool ExternalProcess::IsRunning() const
{
   return API_ExternalProcess_GetExternalProcessIsRunning( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::IsStarting() const
{
   return API_ExternalProcess_GetExternalProcessIsStarting( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ExternalProcess::HasCrashed() const
{
   return API_ExternalProcess_GetExternalProcessExitStatus( handle ) == ExternalProcessContext::CrashedExit;
}

// ----------------------------------------------------------------------------

ExternalProcess::pid_type ExternalProcess::PID() const
{
   return pid_type( API_ExternalProcess_GetExternalProcessPID( handle ) );
}

// ----------------------------------------------------------------------------

int ExternalProcess::ExitCode() const
{
   return API_ExternalProcess_GetExternalProcessExitCode( handle );
}

// ----------------------------------------------------------------------------

size_type ExternalProcess::BytesAvailable() const
{
   return API_ExternalProcess_GetExternalProcessBytesAvailable( handle );
}

// ----------------------------------------------------------------------------

size_type ExternalProcess::BytesToWrite() const
{
   return API_ExternalProcess_GetExternalProcessBytesToWrite( handle );
}

// ----------------------------------------------------------------------------

ByteArray ExternalProcess::StandardOutput()
{
   /*
    * ### NB: ExternalProcess::ReadFromExternalProcess allocates the read data
    *         in the module's heap.
    */
   uint8* data = 0;
   size_type size = 0;
   if ( API_ExternalProcess_ReadFromExternalProcess( ModuleHandle(), handle,
               ExternalProcessContext::StandardOutput, reinterpret_cast<void**>( &data ), &size ) == api_false )
      throw APIFunctionError( "ReadFromExternalProcess" );
   ByteArray b;
   b.Import( data, data+size );
   return b;
}

// ----------------------------------------------------------------------------

ByteArray ExternalProcess::StandardError()
{
   /*
    * ### NB: ExternalProcess::ReadFromExternalProcess allocates the read data
    *         in the module's heap.
    */
   uint8* data = 0;
   size_type size = 0;
   if ( API_ExternalProcess_ReadFromExternalProcess( ModuleHandle(), handle,
               ExternalProcessContext::StandardError, reinterpret_cast<void**>( &data ), &size ) == api_false )
      throw APIFunctionError( "ReadFromExternalProcess" );
   ByteArray b;
   b.Import( data, data+size );
   return b;
}

// ----------------------------------------------------------------------------

ByteArray ExternalProcess::Read()
{
   /*
    * ### NB: ExternalProcess::ReadFromExternalProcess allocates the read data
    *         in the module's heap.
    */
   uint8* data = 0;
   size_type size = 0;
   if ( API_ExternalProcess_ReadFromExternalProcess( ModuleHandle(), handle,
               ExternalProcessContext::CombinedOutput, reinterpret_cast<void**>( &data ), &size ) == api_false )
      throw APIFunctionError( "ReadFromExternalProcess" );
   ByteArray b;
   b.Import( data, data+size );
   return b;
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const ByteArray& data )
{
   if ( !data.IsEmpty() )
      if ( API_ExternalProcess_WriteToExternalProcess( handle, data.Begin(), data.Length() ) == api_false )
         throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const String& text )
{
   if ( !text.IsEmpty() )
      if ( API_ExternalProcess_WriteToExternalProcess( handle, text.c_str(), text.Size() ) == api_false )
         throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const IsoString& text )
{
   if ( !text.IsEmpty() )
      if ( API_ExternalProcess_WriteToExternalProcess( handle, text.c_str(), text.Size() ) == api_false )
         throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const char* text )
{
   if ( text != nullptr )
      if ( *text != '\0' )
         if ( API_ExternalProcess_WriteToExternalProcess( handle, text, strlen( text ) ) == api_false )
            throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

void ExternalProcess::Write( const void* data, size_type count )
{
   if ( data != nullptr )
      if ( count > 0 )
         if ( API_ExternalProcess_WriteToExternalProcess( handle, data, count ) == api_false )
            throw APIFunctionError( "WriteToExternalProcess" );
}

// ----------------------------------------------------------------------------

StringList ExternalProcess::Environment() const
{
   StringList environment;
   if ( API_ExternalProcess_EnumerateExternalProcessEnvironment( handle,
                        ExternalProcessPrivate::EnvironmentEnumerationCallback, &environment ) == api_false )
      return StringList();
   return environment;
}

// ----------------------------------------------------------------------------

void ExternalProcess::SetEnvironment( const StringList& environment )
{
   Array<const char16_type*> vars;
   for ( const String& env : environment )
      vars.Add( env.c_str() );
   if ( API_ExternalProcess_SetExternalProcessEnvironment( handle, vars.Begin(), vars.Length() ) == api_false )
      throw APIFunctionError( "SetExternalProcessEnvironment" );
}

// ----------------------------------------------------------------------------

int ExternalProcess::ExecuteProgram( const String& program, const StringList& arguments )
{
   Array<const char16_type*> argv;
   for ( const String& arg : arguments )
      argv.Add( arg.c_str() );
   int retVal = API_ExternalProcess_ExecuteProgram( program.c_str(), argv.Begin(), argv.Length() );
   if ( retVal < -1 )
      ExternalProcessPrivate::Throw( ExternalProcessContext::FailedToStart );
   return retVal;
}

// ----------------------------------------------------------------------------

ExternalProcess::pid_type ExternalProcess::StartProgram( const String& program, const StringList& arguments, const String& workingDirectory )
{
   Array<const char16_type*> argv;
   for ( const String& arg : arguments )
      argv.Add( arg.c_str() );
   uint64 pid = 0;
   api_bool ok = API_ExternalProcess_StartProgram( program.c_str(), argv.Begin(), argv.Length(), workingDirectory.c_str(), &pid );
   if ( ok == api_false || pid == 0 )
      ExternalProcessPrivate::Throw( ExternalProcessContext::FailedToStart );
   return pid_type( pid );
}

// ----------------------------------------------------------------------------

void* ExternalProcess::CloneHandle() const
{
   throw Error( "Cannot clone an ExternalProcess handle" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ExternalProcess*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ExternalProcessEventDispatcher
{
public:

   static void Started( external_process_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onStarted != nullptr )
         (receiver->*handlers->onStarted)( *sender );
   }

   static void Finished( external_process_handle hSender, control_handle hReceiver, int32 exitCode, int32 exitStatus )
   {
      if ( handlers->onFinished != nullptr )
         (receiver->*handlers->onFinished)( *sender, exitCode, exitStatus == ExternalProcessContext::NormalExit );
   }

   static void StandardOutputDataAvailable( external_process_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onStandardOutputDataAvailable != nullptr )
         (receiver->*handlers->onStandardOutputDataAvailable)( *sender );
   }

   static void StandardErrorDataAvailable( external_process_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onStandardErrorDataAvailable != nullptr )
         (receiver->*handlers->onStandardErrorDataAvailable)( *sender );
   }

   static void Error( external_process_handle hSender, control_handle hReceiver, int32 errorCode )
   {
      if ( handlers->onError != nullptr )
         (receiver->*handlers->onError)( *sender, ExternalProcess::error_code( errorCode ) );
   }

}; // ExternalProcessEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ExternalProcess::OnStarted( process_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ExternalProcess_SetExternalProcessStartedEventRoutine( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::Started : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessStartedEventRoutine" );
   m_handlers->onStarted = handler;
}

void ExternalProcess::OnFinished( process_exit_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ExternalProcess_SetExternalProcessFinishedEventRoutine( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::Finished : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessFinishedEventRoutine" );
   m_handlers->onFinished = handler;
}

void ExternalProcess::OnStandardOutputDataAvailable( process_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ExternalProcess_SetExternalProcessStandardOutputDataAvailableEventRoutine( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::StandardOutputDataAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessStandardOutputDataAvailableEventRoutine" );
   m_handlers->onStandardOutputDataAvailable = handler;
}

void ExternalProcess::OnStandardErrorDataAvailable( process_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ExternalProcess_SetExternalProcessStandardErrorDataAvailableEventRoutine( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::StandardErrorDataAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessStandardErrorDataAvailableEventRoutine" );
   m_handlers->onStandardErrorDataAvailable = handler;
}

void ExternalProcess::OnError( process_error_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ExternalProcess_SetExternalProcessErrorEventRoutine( handle, &receiver,
                     (handler != nullptr) ? ExternalProcessEventDispatcher::Error : nullptr ) == api_false )
      throw APIFunctionError( "SetExternalProcessErrorEventRoutine" );
   m_handlers->onError = handler;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ExternalProcess.cpp - Released 2025-04-07T08:53:32Z
