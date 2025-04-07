//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/API.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#ifdef __PCL_WINDOWS
#  include <stdio.h> // _setmaxstdio()
#endif

namespace pcl
{

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Global API Data
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PCL_DATA const APIInterface* API = nullptr;

static api_handle s_moduleHandle = nullptr;

api_handle PCL_FUNC ModuleHandle()
{
   return s_moduleHandle;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// API Exceptions
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void APIError::GetAPIErrorCode()
{
   if ( API != nullptr && API->Global != nullptr && API->Global->LastError != 0 )
      apiErrorCode = (*API->Global->LastError)();
   else
      apiErrorCode = 0;
}

String APIError::FormatInfo() const
{
   String info = Error::FormatInfo();

   if ( apiErrorCode != 0 )
   {
      if ( info.IsEmpty() )
         info << String().Format( "API error code 0x%04x", apiErrorCode );
      else
         info << String().Format( " (0x%04x)", apiErrorCode );

      String apiMessage;
      if ( apiErrorCode != 0 )
      {
         size_type len = 0;
         (*API->Global->ErrorMessage)( apiErrorCode, 0, &len );
         if ( len > 0 )
         {
            apiMessage.SetLength( len );
            (*API->Global->ErrorMessage)( apiErrorCode, apiMessage.Begin(), &len );
            apiMessage.ResizeToNullTerminated();
         }
      }

      if ( !apiMessage.IsEmpty() )
         info << ": " << apiMessage;
   }

   return info;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// API Initialization Routines
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class APIInitializer
{
public:

   static void HackingError( const String& msg )
   {
      throw Error( "Possible hacking attempt: " + msg );
   }

   static uint32 InitAPI( function_resolver R, uint32 apiVersion )
   {
      try
      {
         if ( API != nullptr )
            HackingError( "Unexpected API module initialization call." );

         if ( R == nullptr )
            HackingError( "Invalid API function resolver address." );

         if ( apiVersion < PCL_API_Version )
            throw Error( String().Format( "Unsupported API version %X (expected >= %X).",
                                          apiVersion, PCL_API_Version ) );

         if ( Module == nullptr )
            throw Error( "Module metadata not available. "
                         "Please instantiate a MetaModule descendant upon module installation." );

         API = new APIInterface( R );

         Module->PerformAPIDefinitions();

#ifdef __PCL_WINDOWS
         /*
          * Win32 specific: Set a maximum for the number of simultaneously open
          * files at the stream I/O level:
          * https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setmaxstdio?view=msvc-160
          */
         if ( _setmaxstdio( 8192 ) == -1 )
            (void)_setmaxstdio( 2048 ); // try to fallback to a 'fail-safe' maximum
#endif
         return 0; // ok
      }

      ERROR_CLEANUP(
         if ( API != nullptr )
         {
            delete API;
            API = nullptr;
         }
      )
      return 1;
   }

   static uint32 BeginIdentification()
   {
      try
      {
         if ( description != nullptr )
            HackingError( "Invalid API identification call (phase 0)." );

         if ( Module == nullptr )
            throw Error( "Module metadata not available. "
                         "Please instantiate a MetaModule descendent class upon module installation." );

         description = new api_module_description;

         description->uniqueId          = Module->UniqueId(); // ### deprecated
         description->versionInfo       = Module->Version();
         description->name              = Module->Name().Release();
         description->description       = Module->Description().Release();
         description->company           = Module->Company().Release();
         description->author            = Module->Author().Release();
         description->copyright         = Module->Copyright().Release();
         description->tradeMarks        = Module->TradeMarks().Release();
         description->originalFileName  = Module->OriginalFileName().Release();

         int year, month, day;
         Module->GetReleaseDate( year, month, day );

         description->releaseDate.year  = year;
         description->releaseDate.month = month;
         description->releaseDate.day   = day;

         description->apiVersion        = PCL_API_Version;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

   static uint32 Identify( api_module_description** d )
   {
      try
      {
         if ( description == nullptr )
            HackingError( "Invalid API identification call (phase 1)." );

         if ( d == nullptr )
            HackingError( "Invalid API module description structure." );

         *d = description;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

   static uint32 EndIdentification()
   {
      try
      {
         if ( description == nullptr )
            HackingError( "Invalid API identification call (phase 255)." );

         String::allocator alloc = String().Allocator();
         if ( description->name != nullptr )
            alloc.DeallocateBlock( const_cast<char*>( description->name ) );
         if ( description->description != nullptr )
            alloc.DeallocateBlock( const_cast<char16_type*>( description->description ) );
         if ( description->company != nullptr )
            alloc.DeallocateBlock( const_cast<char16_type*>( description->company ) );
         if ( description->author != nullptr )
            alloc.DeallocateBlock( const_cast<char16_type*>( description->author ) );
         if ( description->copyright != nullptr )
            alloc.DeallocateBlock( const_cast<char16_type*>( description->copyright ) );
         if ( description->tradeMarks != nullptr )
            alloc.DeallocateBlock( const_cast<char16_type*>( description->tradeMarks ) );
         if ( description->originalFileName != nullptr )
            alloc.DeallocateBlock( const_cast<char16_type*>( description->originalFileName ) );

         delete description;
         description = nullptr;

         return 0; // ok
      }

      ERROR_HANDLER
      return 1;
   }

private:

   static api_module_description* description;

}; // APIInitializer

api_module_description* APIInitializer::description = nullptr;

} // pcl

// ----------------------------------------------------------------------------

// global namespace

/*!
 * PixInsight Module Identification (PMIDN) function.
 *
 * This function will be called by the PixInsight core application when it
 * needs to retrieve identification and descriptive data from a module. This
 * happens when a module is about to be installed, but also when a module is
 * being inspected, either for security reasons or after a direct user request.
 *
 * \param description   Pointer to a pointer to an API module description
 *          structure. See the declaration of api_module_description in
 *          API/APIDefs.h for details. This structure must be provided by the
 *          called module as POD, and its starting address must be written to
 *          the pointer pointed to by this argument in a call to this function
 *          with \a phase = 1 (see below).
 *
 * \param phase         Module identification request:\n
 *          \n
 *          <table border="1" cellpadding="4" cellspacing="0">
 *          <tr>
 *          <td>\a phase = 0x00 - Prepare for module identification.</td>
 *          </tr>
 *          <tr>
 *          <td>\a phase = 0x01 - Report module descriptive data in a structure
 *          of type 'api_module_description', whose address must be stored in
 *          the pointer pointed to by the \a description argument.</td>
 *          </tr>
 *          <tr>
 *          <td>\a phase = 0xff - Module identification process completed. The
 *          module description structure provided when \a phase = 1 can be
 *          deallocated and disposed as necessary.</td>
 *          </tr>
 *          </table>
 *
 * Other values of \a phase may be passed in additional calls to a PMIDN. Those
 * values and calls are reserved for special modules pertaining to the core of
 * the PixInsight platform, and hence not in the user land. In current versions
 * of PixInsight, such special calls must be ignored by the invoked module.
 *
 * A PMIDN must return zero upon success. Any other return value will be
 * interpreted as a module-specific error code.
 *
 * Module developers using the standard PixInsight Class Library (PCL)
 * distribution don't have to care about PMIDN, since it is already implemented
 * internally by PCL.
 *
 * \note A PMIDN is mandatory for all PixInsight modules, and must be
 * implemented as a publicly visible symbol following the standard 'C' naming
 * and calling conventions.
 */
PCL_MODULE_EXPORT uint32
IdentifyPixInsightModule( api_module_description** description,
                          int32                    phase )
{
   switch ( phase )
   {
   case 0:
      // Prepare for module identification.
      return APIInitializer::BeginIdentification();
   case 1:
      // Report module description data.
      return APIInitializer::Identify( description );
   case 255:
      // Done with module identification.
      return APIInitializer::EndIdentification();
   default:
      // Reserved identification request - ignore it.
      break;
   }

   return 0;
}

/*!
 * PixInsight Module Initialization (PMINI) function.
 *
 * This function will be called by the PixInsight core application when a
 * module is being installed. It provides a module with the necessary elements
 * to perform a bidirectional communication with the core application via the
 * standard PCL API callback system.
 *
 * \param hModule       Handle to the module being installed. This handle
 *                      uniquely identifies the module, and must be used in all
 *                      subsequent API calls requiring a module handle.
 *
 * \param R             Pointer to an API function resolver callback. See the
 *                      declaration of function_resolver in API/APIDefs.h and
 *                      the automatically generated file pcl/APIInterface.cpp
 *                      for details.
 *
 * \param apiVersion    API version number.
 *
 * \param reserved      Reserved for special invocations to core platform
 *                      modules. Must not be used or altered in any way.
 *
 * A PMINI must return zero upon success. Any other return value will be
 * interpreted as a module-specific error code.
 *
 * Module developers using the standard PixInsight Class Library (PCL)
 * distribution don't have to care about PMINI, since it is already implemented
 * internally by PCL.
 *
 * \note A PMINI is mandatory for all PixInsight modules, and must be
 * implemented as a publicly visible symbol following the standard 'C' naming
 * and calling conventions.
 */
PCL_MODULE_EXPORT uint32
InitializePixInsightModule( api_handle        hModule,
                            function_resolver R,
                            uint32            apiVersion,
                            void*             reserved )
{
   s_moduleHandle = hModule;
   return APIInitializer::InitAPI( R, apiVersion );
}

// ----------------------------------------------------------------------------
// EOF pcl/API.cpp - Released 2025-04-07T08:53:32Z
