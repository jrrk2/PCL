//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MetaProcess.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/ErrorHandler.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/MetaParameter.h>
#include <pcl/MetaProcess.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/View.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define MANDATORY( funcName )    \
   MandatoryError( Id(), funcName )

static void MandatoryError( const IsoString& procId, const char* funcName )
{
   throw Error( String( procId ) + ": MetaProcess::" +
                funcName + "() must be reimplemented in descendant class." );
}

// ----------------------------------------------------------------------------

MetaProcess::MetaProcess()
   : MetaObject( Module )
{
   if ( Module == nullptr )
      throw Error( "MetaProcess: Module not initialized - illegal MetaProcess instantiation." );
}

// ----------------------------------------------------------------------------

int MetaProcess::ProcessCommandLine( const StringList& /*argv*/ ) const
{
   MANDATORY( "ProcessCommandLine" );
   return -1;
}

// ----------------------------------------------------------------------------

bool MetaProcess::EditPreferences() const
{
   MANDATORY( "EditPreferences" );
   return false;
}

// ----------------------------------------------------------------------------

bool MetaProcess::BrowseDocumentation() const
{
#if PCL_API_Version >= 0x0126
   return API_Global_BrowseProcessDocumentation( this, 0/*flags*/ ) != api_false;
#else
   return false;
#endif
}

// ----------------------------------------------------------------------------

void MetaProcess::IPCStart( int instance, const IsoString& messageUID, const String& parameters ) const
{
   MANDATORY( "IPCStart" );
}

// ----------------------------------------------------------------------------

void MetaProcess::IPCStop( int instance, const IsoString& messageUID ) const
{
   MANDATORY( "IPCStop" );
}

// ----------------------------------------------------------------------------

void MetaProcess::IPCSetParameters( int instance, const IsoString& messageUID, const String& parameters ) const
{
   MANDATORY( "IPCSetParameters" );
}

// ----------------------------------------------------------------------------

int MetaProcess::IPCStatus( int instance, const IsoString& messageUID ) const
{
   MANDATORY( "IPCStatus" );
   return 0;
}

// ----------------------------------------------------------------------------

Bitmap MetaProcess::Icon() const
{
   IsoString svg = IconImageSVG();
   if ( !svg.IsEmpty() )
      return Bitmap( svg.Begin(), svg.Length(), "SVG" );

   String filePath = IconImageSVGFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   const char** xpm = IconImageXPM();
   if ( xpm != nullptr )
      return Bitmap( xpm );

   filePath = IconImageFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   return Bitmap();
}

// ----------------------------------------------------------------------------

// ### DEPRECATED
Bitmap MetaProcess::SmallIcon() const
{
   const char** xpm = SmallIconImageXPM();
   if ( xpm != nullptr )
      return Bitmap( xpm );

   String filePath = SmallIconImageFile();
   if ( !filePath.IsEmpty() )
      return Bitmap( filePath );

   return Bitmap();
}

// ----------------------------------------------------------------------------

const MetaParameter* MetaProcess::operator[]( size_type i ) const
{
   const MetaParameter* p = dynamic_cast<const MetaParameter*>( m_children[i] );
   if ( p == nullptr )
      throw Error( "MetaProcess: invalid non-parameter child instance" );
   return p;
}

// ----------------------------------------------------------------------------

Property MetaProcess::SignatureProperty( const IsoString& taskId ) const
{
   IsoString id = "PCL:Signature:" + taskId.Trimmed();
   if ( !Property::IsValidIdentifier( id ) )
      throw Error( "MetaProcess::SignatureProperty(): invalid property identifier '" + id + '\'' );

   int major, minor, release, build;
   IsoString dum1, dum2;
   Module->GetVersion( major, minor, release, build, dum1, dum2 );
   IsoString info;
   info << "process=" << Id() << IsoString().Format( ",version=%d.%d.%d", major, minor, release );
   if ( build > 0 )
      info << IsoString().Format( "-%d", build );
   info << ",timestamp=" << TimePoint::Now().ToIsoString();

   return Property( id, info );
}

// ----------------------------------------------------------------------------
// Process Context
// ----------------------------------------------------------------------------

#define process         reinterpret_cast<const MetaProcess*>( hp )
#define instance        reinterpret_cast<ProcessImplementation*>( hp )
#define constInstance   reinterpret_cast<const ProcessImplementation*>( hp )

class ProcessContextDispatcher
{
public:

   static void api_func InitializeClass( meta_process_handle hp )
   {
      try
      {
         return const_cast<MetaProcess*>( process )->InitializeClass();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static process_handle api_func CreateProcess( meta_process_handle hp )
   {
      try
      {
         return process->Create();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static void api_func DestroyProcess( process_handle hp )
   {
      try
      {
         if ( hp != 0 )
            delete instance;
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static process_handle api_func CloneProcess( const_process_handle hp )
   {
      try
      {
         return constInstance->Meta()->Clone( *constInstance );
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static process_handle api_func TestCloneProcess( const_process_handle hp )
   {
      try
      {
         return constInstance->Meta()->TestClone( *constInstance );
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func AssignProcess( process_handle hpDst, const_process_handle hpSrc )
   {
      try
      {
         reinterpret_cast<ProcessImplementation*>( hpDst )->Assign( *reinterpret_cast<const ProcessImplementation*>( hpSrc ) );
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func SetProcessServerHandle( process_handle hp, const_api_handle hs )
   {
      try
      {
         instance->m_serverHandle = hs;
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func InitializeProcess( process_handle hp )
   {
      try
      {
         instance->Initialize();
         return api_true;
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcess( process_handle hp, char16_type* info, uint32 maxLen )
   {
      try
      {
         String infoStr;
         bool valid = instance->Validate( infoStr );

         if ( !valid && !infoStr.IsEmpty() && info != nullptr && maxLen > 0 )
            infoStr.c_copy( info, maxLen );

         return api_bool( valid );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static uint32 api_func ProcessExecutionPreferences( meta_process_handle hp )
   {
      try
      {
         /*
          * Currently, only bit #0 of the result is used. It is 1 if global
          * execution is preferred, 0 if view execution is preferred. Future
          * versions may return more elaborated values.
          */
         return process->PrefersGlobalExecution() ? 1 : 0;
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessExecution( const_view_handle hv, const_process_handle hp, char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         String whyNotStr;
         bool ok = constInstance->CanExecuteOn( pcl::View( hv ), whyNotStr );

         if ( !ok && !whyNotStr.IsEmpty() && whyNot != nullptr && maxLen > 0 )
               whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessMask( const_view_handle hv, const_window_handle hw, const_process_handle hp )
   {
      try
      {
         return (api_bool)constInstance->IsMaskable( pcl::View( hv ), pcl::ImageWindow( window_handle( hw ) ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessHistoryUpdate( const_view_handle hv, const_process_handle hp )
   {
      try
      {
         return (api_bool)constInstance->IsHistoryUpdater( pcl::View( hv ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static uint32 api_func ProcessUndoMode( const_view_handle hv, const_process_handle hp )
   {
      try
      {
         return constInstance->UndoMode( pcl::View( hv ) );
      }
      ERROR_HANDLER
      return ~uint32( 0 );
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeProcessExecution( view_handle hv, process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         pcl::View v( hv );
         api_bool retVal = (api_bool)instance->BeforeExecution( v );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ExecuteProcess( view_handle hv, process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         pcl::View v( hv );
         api_bool retVal = (api_bool)instance->ExecuteOn( v );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterProcessExecution( view_handle hv, process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         pcl::View v( hv );
         instance->AfterExecution( v );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessGlobalExecution( const_process_handle hp, char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         String whyNotStr;
         bool ok = constInstance->CanExecuteGlobal( whyNotStr );

         if ( !ok && !whyNotStr.IsEmpty() && whyNot != nullptr && maxLen > 0 )
               whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeProcessGlobalExecution( process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         api_bool retVal = (api_bool)instance->BeforeGlobalExecution();

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ExecuteProcessGlobal( process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         api_bool retVal = (api_bool)instance->ExecuteGlobal();

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return retVal;
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterProcessGlobalExecution( process_handle hp )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         instance->AfterGlobalExecution();

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );
      }
      ERROR_HANDLER

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessImageExecution( const_image_handle hImage, const_process_handle hp, char16_type* whyNot, uint32 maxLen )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !API_SharedImage_GetImageFormat( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         ImageVariant image;
         void* h = const_cast<image_handle>( hImage );
         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               image = ImageVariant( new pcl::Image( h ) );
               break;
            case 64 :
               image = ImageVariant( new pcl::DImage( h ) );
               break;
            default :
               return api_false; // ?!
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               image = ImageVariant( new UInt8Image( h ) );
               break;
            case 16 :
               image = ImageVariant( new UInt16Image( h ) );
               break;
            case 32 :
               image = ImageVariant( new UInt32Image( h ) );
               break;
            default :
               return api_false; // ?!
            }

         image.SetOwnership( true );

         String whyNotStr;
         bool ok = constInstance->CanExecuteOn( image, whyNotStr );

         if ( !ok && !whyNotStr.IsEmpty() && whyNot != nullptr && maxLen > 0 )
               whyNotStr.c_copy( whyNot, maxLen );

         return api_bool( ok );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ExecuteProcessImage( image_handle hImage, process_handle hp, const char* hints, uint32/*flags*/ )
   {
      try
      {
         uint32 bitsPerSample;
         api_bool isFloat;
         if ( !API_SharedImage_GetImageFormat( hImage, &bitsPerSample, &isFloat ) )
            throw 0;

         ImageVariant image;
         if ( isFloat )
            switch ( bitsPerSample )
            {
            case 32 :
               image = ImageVariant( new pcl::Image( hImage ) );
               break;
            case 64 :
               image = ImageVariant( new pcl::DImage( hImage ) );
               break;
            default :
               return api_false; // ?!
            }
         else
            switch ( bitsPerSample )
            {
            case  8 :
               image = ImageVariant( new UInt8Image( hImage ) );
               break;
            case 16 :
               image = ImageVariant( new UInt16Image( hImage ) );
               break;
            case 32 :
               image = ImageVariant( new UInt32Image( hImage ) );
               break;
            default :
               return api_false; // ?!
            }

         image.SetOwnership( true );

         bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
         bool wasGUIOutput = Exception::IsGUIOutputEnabled();

         try
         {
            Exception::EnableConsoleOutput();
            Exception::DisableGUIOutput();

            api_bool retVal = (api_bool)instance->ExecuteOn( image, hints );

            Exception::EnableConsoleOutput( wasConsoleOutput );
            Exception::EnableGUIOutput( wasGUIOutput );

            return retVal;
         }
         catch ( ... )
         {
            Exception::EnableConsoleOutput( wasConsoleOutput );
            Exception::EnableGUIOutput( wasGUIOutput );
            throw;
         }
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static meta_interface_handle api_func DefaultProcessInterface( meta_process_handle hp )
   {
      try
      {
         return process->DefaultInterface();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static meta_interface_handle api_func SelectProcessInterface( const_process_handle hp )
   {
      try
      {
         return constInstance->SelectInterface();
      }
      ERROR_HANDLER
      return 0;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func ValidateProcessInterface( const_process_handle hp, meta_interface_handle hi )
   {
      try
      {
         return constInstance->IsValidInterface( reinterpret_cast<const ProcessInterface*>( hi ) );
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeReadingProcess( process_handle hp )
   {
      try
      {
         return (api_bool)instance->BeforeReading();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterReadingProcess( process_handle hp )
   {
      try
      {
         instance->AfterReading();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BeforeWritingProcess( const_process_handle hp )
   {
      try
      {
         return (api_bool)constInstance->BeforeWriting();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static void api_func AfterWritingProcess( const_process_handle hp )
   {
      try
      {
         constInstance->AfterWriting();
      }
      ERROR_HANDLER
   }

   // -------------------------------------------------------------------------

   static void api_func IPCStart( meta_process_handle hp, int32 coreInstance, const char* messageUID, const char16_type* parameters )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         process->IPCStart( coreInstance, IsoString( messageUID ), String( parameters ) );
      }
      catch ( Exception& x )
      {
         Console().WriteLn( "<end><cbr>" + x.FormatInfo() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static void api_func IPCStop( meta_process_handle hp, int32 coreInstance, const char* messageUID, const char16_type* /*parameters*/ )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         process->IPCStop( coreInstance, IsoString( messageUID ) );
      }
      catch ( Exception& x )
      {
         Console().WriteLn( "<end><cbr>" + x.FormatInfo() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static void api_func IPCSetParameters( meta_process_handle hp, int32 coreInstance, const char* messageUID, const char16_type* parameters )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         process->IPCSetParameters( coreInstance, IsoString( messageUID ), String( parameters ) );
      }
      catch ( Exception& x )
      {
         Console().WriteLn( "<end><cbr>" + x.FormatInfo() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );
   }

   // -------------------------------------------------------------------------

   static int32 api_func IPCGetStatus( meta_process_handle hp, int32 coreInstance, const char* messageUID )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();
      int status = -1;

      try
      {
         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         status = process->IPCStatus( coreInstance, IsoString( messageUID ) );
      }
      catch ( Exception& x )
      {
         Console().WriteLn( "<end><cbr>" + x.FormatInfo() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return status;
   }

   // -------------------------------------------------------------------------

   static int32 api_func ProcessCommandLine( meta_process_handle hp, int32 argc, const char16_type** argv )
   {
      bool wasConsoleOutput = Exception::IsConsoleOutputEnabled();
      bool wasGUIOutput = Exception::IsGUIOutputEnabled();

      try
      {
         StringList args;
         for ( int i = 0; i < argc; ++i )
            args << String( argv[i] );

         Exception::EnableConsoleOutput();
         Exception::DisableGUIOutput();

         int result = process->ProcessCommandLine( args );

         Exception::EnableConsoleOutput( wasConsoleOutput );
         Exception::EnableGUIOutput( wasGUIOutput );

         return result;
      }
      catch ( Exception& x )
      {
         Console().WriteLn( "<end><cbr>" + x.FormatInfo() );
      }
      catch ( ... )
      {
         try
         {
            throw;
         }
         ERROR_HANDLER
      }

      Exception::EnableConsoleOutput( wasConsoleOutput );
      Exception::EnableGUIOutput( wasGUIOutput );

      return -1;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func EditProcessPreferences( meta_process_handle hp )
   {
      try
      {
         return (api_bool)process->EditPreferences();
      }
      ERROR_HANDLER
      return api_false;
   }

   // -------------------------------------------------------------------------

   static api_bool api_func BrowseProcessDocumentation( meta_process_handle hp )
   {
      try
      {
         return (api_bool)process->BrowseDocumentation();
      }
      ERROR_HANDLER
      return api_false;
   }

}; // ProcessContextDispatcher

#undef process
#undef instance
#undef constInstance

// ----------------------------------------------------------------------------

void MetaProcess::PerformAPIDefinitions() const
{
   API_ProcessDefinition_EnterProcessDefinitionContext();

   {
      IsoString id = Id();
      API_ProcessDefinition_BeginProcessDefinition( this, id.c_str() );
   }

   {
      IsoString cats = Categories();
      if ( cats.IsEmpty() )
         cats = Category(); // ### NB: Required for compatibility with PCL 1.x
      if ( !cats.IsEmpty() )
         API_ProcessDefinition_SetProcessCategory( cats.c_str() );
   }

   API_ProcessDefinition_SetProcessVersion( Version() );

   {
      IsoString aliases = Aliases().Trimmed();
      if ( !aliases.IsEmpty() )
         API_ProcessDefinition_SetProcessAliasIdentifiers( aliases.c_str() );
   }

   {
      String desc = Description();
      if ( !desc.IsEmpty() )
         API_ProcessDefinition_SetProcessDescription( desc.c_str() );
   }

   {
      String cmnt = ScriptComment();
      if ( !cmnt.IsEmpty() )
         API_ProcessDefinition_SetProcessScriptComment( cmnt.c_str() );
   }

   {
      IsoString svg = IconImageSVG();
      if ( !svg.IsEmpty() )
         API_ProcessDefinition_SetProcessIconSVG( svg.c_str() );
      else
      {
         String filePath = IconImageSVGFile();
         if ( !filePath.IsEmpty() )
            API_ProcessDefinition_SetProcessIconSVGFile( filePath.c_str() );
         else
         {
            // ### DEPRECATED - Process icon images in raster bitmap formats.

            const char** xpm = IconImageXPM();
            if ( xpm != nullptr )
               API_ProcessDefinition_SetProcessIconImage( xpm );
            else
            {
               String path = IconImageFile();
               if ( !path.IsEmpty() )
                  API_ProcessDefinition_SetProcessIconImageFile( path.c_str() );
            }

            xpm = SmallIconImageXPM();
            if ( xpm != nullptr )
               API_ProcessDefinition_SetProcessIconSmallImage( xpm );
            else
            {
               String path = SmallIconImageFile();
               if ( !path.IsEmpty() )
                  API_ProcessDefinition_SetProcessIconSmallImageFile( path.c_str() );
            }
         }
      }
   }

   API_ProcessDefinition_SetProcessClassInitializationRoutine( ProcessContextDispatcher::InitializeClass );
   API_ProcessDefinition_SetProcessCreationRoutine( ProcessContextDispatcher::CreateProcess );
   API_ProcessDefinition_SetProcessDestructionRoutine( ProcessContextDispatcher::DestroyProcess );
   API_ProcessDefinition_SetProcessClonationRoutine( ProcessContextDispatcher::CloneProcess );
   API_ProcessDefinition_SetProcessTestClonationRoutine( ProcessContextDispatcher::TestCloneProcess );
   API_ProcessDefinition_SetProcessSetServerHandleRoutine( ProcessContextDispatcher::SetProcessServerHandle );

   if ( IsAssignable() )
      API_ProcessDefinition_SetProcessAssignmentRoutine( ProcessContextDispatcher::AssignProcess );

   if ( NeedsInitialization() )
      API_ProcessDefinition_SetProcessInitializationRoutine( ProcessContextDispatcher::InitializeProcess );

   if ( NeedsValidation() )
      API_ProcessDefinition_SetProcessValidationRoutine( ProcessContextDispatcher::ValidateProcess );

   if ( CanProcessCommandLines() )
      API_ProcessDefinition_SetProcessCommandLineProcessingRoutine( ProcessContextDispatcher::ProcessCommandLine, 0 );

   if ( CanEditPreferences() )
      API_ProcessDefinition_SetProcessEditPreferencesRoutine( ProcessContextDispatcher::EditProcessPreferences );

   if ( CanBrowseDocumentation() )
      API_ProcessDefinition_SetProcessBrowseDocumentationRoutine( ProcessContextDispatcher::BrowseProcessDocumentation );

   API_ProcessDefinition_SetProcessExecutionPreferencesRoutine( ProcessContextDispatcher::ProcessExecutionPreferences );

   if ( CanProcessViews() )
   {
      API_ProcessDefinition_SetProcessExecutionValidationRoutine( ProcessContextDispatcher::ValidateProcessExecution );
      API_ProcessDefinition_SetProcessMaskValidationRoutine( ProcessContextDispatcher::ValidateProcessMask );
      API_ProcessDefinition_SetProcessHistoryUpdateValidationRoutine( ProcessContextDispatcher::ValidateProcessHistoryUpdate );
      API_ProcessDefinition_SetProcessUndoModeRoutine( ProcessContextDispatcher::ProcessUndoMode );
      API_ProcessDefinition_SetProcessPreExecutionRoutine( ProcessContextDispatcher::BeforeProcessExecution );
      API_ProcessDefinition_SetProcessExecutionRoutine( ProcessContextDispatcher::ExecuteProcess );
      API_ProcessDefinition_SetProcessPostExecutionRoutine( ProcessContextDispatcher::AfterProcessExecution );
   }

   if ( CanProcessGlobal() )
   {
      API_ProcessDefinition_SetProcessGlobalExecutionValidationRoutine( ProcessContextDispatcher::ValidateProcessGlobalExecution );
      API_ProcessDefinition_SetProcessPreGlobalExecutionRoutine( ProcessContextDispatcher::BeforeProcessGlobalExecution );
      API_ProcessDefinition_SetProcessGlobalExecutionRoutine( ProcessContextDispatcher::ExecuteProcessGlobal );
      API_ProcessDefinition_SetProcessPostGlobalExecutionRoutine( ProcessContextDispatcher::AfterProcessGlobalExecution );
   }

   if ( CanProcessImages() )
   {
      API_ProcessDefinition_SetProcessImageExecutionValidationRoutine( ProcessContextDispatcher::ValidateProcessImageExecution );
      API_ProcessDefinition_SetProcessImageExecutionRoutine( ProcessContextDispatcher::ExecuteProcessImage );
   }

   API_ProcessDefinition_SetProcessDefaultInterfaceSelectionRoutine( ProcessContextDispatcher::DefaultProcessInterface );
   API_ProcessDefinition_SetProcessInterfaceSelectionRoutine( ProcessContextDispatcher::SelectProcessInterface );
   API_ProcessDefinition_SetProcessInterfaceValidationRoutine( ProcessContextDispatcher::ValidateProcessInterface );

   API_ProcessDefinition_SetProcessPreReadingRoutine( ProcessContextDispatcher::BeforeReadingProcess );
   API_ProcessDefinition_SetProcessPostReadingRoutine( ProcessContextDispatcher::AfterReadingProcess );
   API_ProcessDefinition_SetProcessPreWritingRoutine( ProcessContextDispatcher::BeforeWritingProcess );
   API_ProcessDefinition_SetProcessPostWritingRoutine( ProcessContextDispatcher::AfterWritingProcess );

   if ( CanProcessIPCMessages() )
   {
      API_ProcessDefinition_SetProcessIPCStartRoutine( ProcessContextDispatcher::IPCStart );
      API_ProcessDefinition_SetProcessIPCStopRoutine( ProcessContextDispatcher::IPCStop );
      API_ProcessDefinition_SetProcessIPCSetParametersRoutine( ProcessContextDispatcher::IPCSetParameters );
      API_ProcessDefinition_SetProcessIPCGetStatusRoutine( ProcessContextDispatcher::IPCGetStatus );
   }

   for ( size_type j = 0; j < Length(); ++j )
      (*this)[j]->PerformAPIDefinitions();

   API_ProcessDefinition_EndProcessDefinition();

   API_ProcessDefinition_ExitProcessDefinitionContext();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MetaProcess.cpp - Released 2025-04-07T08:53:32Z
