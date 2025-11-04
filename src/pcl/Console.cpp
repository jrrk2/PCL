//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Console.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/View.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Console::Console()
{
   if ( (m_handle = API_Global_GetConsole()) == nullptr )
      throw APIFunctionError( "GetConsole" );
   m_thread = API_Thread_GetCurrentThread();
}

// ----------------------------------------------------------------------------

Console::~Console()
{
   m_thread = nullptr;
}

// ----------------------------------------------------------------------------

void Console::Write( const String& s )
{
   if ( m_thread == nullptr )
   {
      if ( API_Global_WriteConsole( m_handle, s.c_str(), api_false ) == api_false )
         throw APIFunctionError( "WriteConsole" );
   }
   else
      API_Thread_AppendThreadConsoleOutputText( m_thread, s.c_str(), api_false );
}

// ----------------------------------------------------------------------------

void Console::WriteLn( const String& s )
{
   if ( m_thread == nullptr )
   {
      if ( API_Global_WriteConsole( m_handle, s.c_str(), api_true ) == api_false )
         throw APIFunctionError( "WriteConsole" );
   }
   else
      API_Thread_AppendThreadConsoleOutputText( m_thread, s.c_str(), api_true );
}

// ----------------------------------------------------------------------------

void Console::WriteLn()
{
   if ( m_thread == nullptr )
   {
      if ( API_Global_WriteConsole( m_handle, 0, api_true ) == api_false )
         throw APIFunctionError( "WriteConsole" );
   }
   else
      API_Thread_AppendThreadConsoleOutputText( m_thread, 0, api_true );
}

// ----------------------------------------------------------------------------

int Console::ReadChar()
{
   if ( m_thread == nullptr )
      return API_Global_ReadConsoleChar( m_handle );
   return 0;
}

// ----------------------------------------------------------------------------

String Console::ReadString()
{
   if ( m_thread == nullptr )
   {
      char16_type* s = API_Global_ReadConsoleString( ModuleHandle(), m_handle );
      if ( s != nullptr )
      {
         String str( s );
         Module->Deallocate( s );
         return str;
      }
   }
   return String();
}

// ----------------------------------------------------------------------------

/*
 *
 * ### DISABLED FUNCTION - FOR SECURITY REASONS                            ###
 * ### DO NOT ENABLE THIS FUNCTION FOR PRODUCTION MODULES                  ###
 *
 *
String Console::Text() const
{
   if ( m_thread == nullptr )
   {
      char16_type* s = API_Global_GetConsoleText( ModuleHandle(), m_handle );
      if ( s != nullptr )
      {
         String str( s );
         Module->Deallocate( s );
         return str;
      }
   }
   return String();
}
 */

// ----------------------------------------------------------------------------

bool Console::Suspended() const
{
   return (API_Global_GetProcessStatus() & 0x00000002) != 0;
}

// ----------------------------------------------------------------------------

bool Console::Waiting() const
{
   return (API_Global_GetProcessStatus() & 0x00000004) != 0;
}

// ----------------------------------------------------------------------------

bool Console::AbortEnabled() const
{
   return (API_Global_GetProcessStatus() & 0x40000000) != 0;
}

// ----------------------------------------------------------------------------

bool Console::AbortRequested() const
{
   return (API_Global_GetProcessStatus() & 0x80000000) != 0;
}

// ----------------------------------------------------------------------------

void Console::ResetStatus()
{
   if ( m_thread == nullptr )
      if ( API_Global_ResetProcessStatus() == api_false )
         throw APIFunctionError( "ResetProcessStatus" );
}

// ----------------------------------------------------------------------------

void Console::EnableAbort()
{
   if ( m_thread == nullptr )
      if ( API_Global_EnableAbort() == api_false )
         throw APIFunctionError( "EnableAbort" );
}

// ----------------------------------------------------------------------------

void Console::DisableAbort()
{
   if ( m_thread == nullptr )
      if ( API_Global_DisableAbort() == api_false )
         throw APIFunctionError( "DisableAbort" );
}

// ----------------------------------------------------------------------------

void Console::Abort()
{
   if ( m_thread == nullptr )
      if ( API_Global_Abort() == api_false )
         throw APIFunctionError( "Abort" );
}

// ----------------------------------------------------------------------------

bool Console::IsValid() const
{
   return m_handle != nullptr && API_Global_ValidateConsole( m_handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool Console::IsCurrentThreadConsole() const
{
   return m_handle != nullptr && m_thread == API_Thread_GetCurrentThread();
}

// ----------------------------------------------------------------------------

void Console::Flush()
{
   if ( m_thread == nullptr )
      if ( API_Global_FlushConsole( m_handle ) == api_false )
         throw APIFunctionError( "FlushConsole" );
}

// ----------------------------------------------------------------------------

bool Console::Show( bool show )
{
   if ( m_thread == nullptr )
      return API_Global_ShowConsole( m_handle, api_bool( show ) ) != api_false;
   return false;
}

// ----------------------------------------------------------------------------

void Console::Clear()
{
   if ( m_thread == nullptr )
      Write( "<clr>" );
}

// ----------------------------------------------------------------------------

void Console::ExecuteCommand( const String& command )
{
   if ( m_thread != nullptr )
      throw Error( "Console::ExecuteCommand() can only be invoked from the root thread." );

   if ( API_Global_ExecuteCommand( ModuleHandle(), m_handle, command.c_str() ) == api_false )
      throw APIFunctionError( "ExecuteCommand" );
}

// ----------------------------------------------------------------------------

static String MakeScriptArguments( const StringKeyValueList& arguments )
{
   String scriptArgs;
   for ( const StringKeyValue& arg : arguments )
   {
      String value = arg.value;
      value.ReplaceString( "$", "\\$" ); // disable $ macro expansion in script arguments
      scriptArgs << " -p=\""
                 << arg.key
                 << ','
                 << value
                 << '\"';
   }
   return scriptArgs;
}

// ----------------------------------------------------------------------------

void Console::ExecuteScript( const String& filePath, const StringKeyValueList& arguments )
{
   ExecuteCommand( "run -x -r=m" + MakeScriptArguments( arguments ) +
                   " \"" + filePath + '\"' );
}

// ----------------------------------------------------------------------------

void Console::ExecuteScriptGlobal( const String& filePath, const StringKeyValueList& arguments )
{
   ExecuteCommand( "run -x -r=m" + MakeScriptArguments( arguments ) +
                   " -p=\"isGlobalTarget,true\""
                   " -p=\"isViewTarget,false\""
                   " \"" + filePath + '\"' );
}

// ----------------------------------------------------------------------------

void Console::ExecuteScriptOn( const View& view, const String& filePath, const StringKeyValueList& arguments )
{
   ExecuteCommand( "run -x -r=m" + MakeScriptArguments( arguments ) +
                   " -p=\"isGlobalTarget,false\""
                   " -p=\"isViewTarget,true\""
                   " -p=\"targetView," + view.FullId() + '\"' +
                   " \"" + filePath + '\"' );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Console.cpp - Released 2025-04-07T08:53:32Z
