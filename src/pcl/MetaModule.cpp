//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MetaModule.cpp - Released 2025-04-07T08:53:32Z
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

#ifdef __PCL_WINDOWS
# include <windows.h>
#else
# include <unistd.h>
#endif

#if defined( __PCL_MACOSX ) || defined( __PCL_FREEBSD )
# include <sys/types.h>
# include <sys/param.h>
# include <sys/sysctl.h>
#endif

#ifdef __PCL_MACOSX
# include <mach/mach_host.h>
# include <mach/mach_port.h>
#endif

#ifdef __PCL_LINUX
# include <pcl/ExternalProcess.h>
# include <pcl/Thread.h>
#endif

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Version.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MetaModule* Module = nullptr;

// ----------------------------------------------------------------------------

MetaModule::MetaModule()
   : MetaObject( nullptr )
{
   if ( Module != nullptr )
      throw Error( "MetaModule: Module redefinition not allowed" );
   Module = this;
}

MetaModule::~MetaModule()
{
   if ( this == Module )
      Module = nullptr;
}

/*
 * ### REMOVE - deprecated function
 */
const char* MetaModule::UniqueId() const
{
   return nullptr;
}

bool MetaModule::IsInstalled() const
{
   return API != nullptr;
}

void MetaModule::ProcessEvents( bool excludeUserInputEvents )
{
   thread_handle thread = (*API->Thread->GetCurrentThread)();
   if ( thread == 0 ) // if root thread
      (*API->Global->ProcessEvents)( excludeUserInputEvents );
   else
   {
      uint32 threadStatus = (*API->Thread->GetThreadStatus)( thread );
      if ( threadStatus & 0x80000000 ) // see Thread.cpp
         throw ProcessAborted();
   }
}

// ----------------------------------------------------------------------------

/*
 * Length of the version marker string: "PIXINSIGHT_MODULE_VERSION_".
 *
 * PIXINSIGHT_MODULE_VERSION_
 * 12345678901234567890123456
 *          1         2
 *
 * Note that we cannot define the marker string in source code, or the module
 * authentication routines would detect our instance, instead of the actual
 * version information string. This would prevent module authentication.
 */
#define LengthOfVersionMarker 26

void MetaModule::GetVersion( int& major, int& minor, int& release, int& build,
                             IsoString& language, IsoString& status ) const
{
   // Set undefined states for all variables, in case of error.
   major = minor = release = build = 0;
   language.Clear();
   status.Clear();

   IsoString vs( Version() );

   // A version string must begin with a version marker
   if ( vs.Length() < LengthOfVersionMarker )
      return;

   // Split the string of version numbers into tokens separated by dots
   StringList tokens;
   vs.Break( tokens, '.', false/*trim*/, LengthOfVersionMarker/*startIndex*/ );

   // Required: MM.mm.rr.bbbb.LLL
   // Optional: .<status>
   if ( tokens.Length() < 5 || tokens.Length() > 6 )
      return;

   // Extract version numbers
   try
   {
      int MM   = tokens[0].ToInt( 10 );
      int mm   = tokens[1].ToInt( 10 );
      int rr   = tokens[2].ToInt( 10 );
      int bbbb = tokens[3].ToInt( 10 );

      major = MM;
      minor = mm;
      release = rr;
      build = bbbb;
   }
   catch ( ... ) // silently eat all parse exceptions here
   {
      return;
   }

   // Language code
   language = tokens[4]; // ### TODO: Verify validity of ISO 639.2 code

   // Optional status word
   if ( tokens.Length() == 6 )
      status = tokens[5];  // ### TODO: Verify validity of the status word
}

IsoString MetaModule::ReadableVersion() const
{
   int major, minor, release, build;
   IsoString dum1, dum2;
   GetVersion( major, minor, release, build, dum1, dum2 );
   IsoString version = Name() + IsoString().Format( " module version %d.%d.%d", major, minor, release );
   if ( build > 0 )
      version.AppendFormat( "-%d", build );
   return version;
}

// ----------------------------------------------------------------------------

int MetaModule::NumberOfProcessors() const
{
   static AtomicInt numberOfProcessors;
   static Mutex mutex;

   if ( numberOfProcessors.Load() == 0 )
   {
      volatile AutoLock lock( mutex );
      if ( numberOfProcessors.Load() == 0 )
         numberOfProcessors.Store( pcl::Max( 1, PixInsightSettings::GlobalInteger( "System/NumberOfProcessors" ) ) );
   }

   return numberOfProcessors;
}

// ----------------------------------------------------------------------------

bool MetaModule::GetPhysicalMemoryStatus( size_type& totalBytes, size_type& availableBytes ) const
{
#ifdef __PCL_FREEBSD

   totalBytes = availableBytes = 0;
   int mib[ 2 ] = { CTL_HW, HW_PHYSMEM };
   size_t size = sizeof( totalBytes );
   if ( sysctl( mib, 2, &totalBytes, &size, 0, 0 ) == 0 )
   {
      mib[1] = HW_USERMEM;
      size = sizeof( availableBytes );
      if ( sysctl( mib, 2, &availableBytes, &size, 0, 0 ) == 0 )
         return totalBytes > 0 && availableBytes > 0;
   }
   return false;

#endif

#ifdef __PCL_LINUX

   ExternalProcess P;
   for ( int try_ = 0;; )
   {
      P.Start( "cat", StringList() << "/proc/meminfo" );
      if ( P.WaitForStarted() )
         if ( P.WaitForFinished() )
            if ( !P.HasCrashed() )
               if ( P.ExitCode() == 0 )
                  break;
      if ( ++try_ == 3 )
         return false;
      Sleep( 500 );
   }

   IsoString info = IsoString( P.StandardOutput() );
   if ( info.IsEmpty() )
      return false;

   unsigned long memTotalkB = 0, memAvailablekB = 0, memFreekB = 0, cachedkB = 0;
   IsoStringList lines;
   info.Break( lines, '\n', true/*trim*/ );
   for ( const IsoString& line : lines )
      if ( line.StartsWithIC( "MemTotal" ) )
      {
         if ( ::sscanf( line.c_str(), "%*s %lu %*s", &memTotalkB ) != 1 )
            return false;
      }
      else if ( line.StartsWithIC( "MemAvailable" ) )
      {
         // Current kernel versions provide a MemAvailable item since 2014:
         // https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=34e431b0ae398fc54ea69ff85ec700722c9da773
         if ( ::sscanf( line.c_str(), "%*s %lu %*s", &memAvailablekB ) != 1 )
            return false;
         if ( memTotalkB > 0 )
            break;
      }
      else if ( line.StartsWithIC( "MemFree" ) )
      {
         if ( ::sscanf( line.c_str(), "%*s %lu %*s", &memFreekB ) != 1 )
            return false;
      }
      else if ( line.StartsWithIC( "Cached" ) )
      {
         if ( ::sscanf( line.c_str(), "%*s %lu %*s", &cachedkB ) != 1 )
            return false;
      }

   totalBytes = memTotalkB * 1024;

   // On old kernels/distros (e.g. RHEL 6.x), try to guess an approximate value
   // as 'free' + 'cached', which is wrong but should be pessimistic i.e. safe.
   if ( memAvailablekB == 0 )
      memAvailablekB = memFreekB + cachedkB;
   availableBytes = memAvailablekB * 1024;

   return totalBytes > 0 && availableBytes > 0;

#endif // __PCL_LINUX

#ifdef __PCL_MACOSX

   // https://unix.stackexchange.com/questions/175806/accounting-for-missing-memory-from-sysctl
   totalBytes = availableBytes = 0;
   size_t size = sizeof( totalBytes );
   if ( sysctlbyname( "hw.memsize", &totalBytes, &size, NULL, 0 ) == 0 )
   {
      mach_port_t host = mach_host_self();
      vm_size_t pageSize;
      if ( host_page_size( host, &pageSize ) == KERN_SUCCESS )
      {
# ifdef HOST_VM_INFO64
         struct vm_statistics64 vm_stat;
         natural_t count = HOST_VM_INFO64_COUNT;
         if ( host_statistics64( host, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count ) == KERN_SUCCESS )
# else
         struct vm_statistics vm_stat;
         natural_t count = HOST_VM_INFO_COUNT;
         if ( host_statistics( host, HOST_VM_INFO, (host_info_t)&vm_stat, &count ) == KERN_SUCCESS )
# endif
         {
            availableBytes = size_type( vm_stat.free_count + vm_stat.active_count + vm_stat.inactive_count )
                           * size_type( pageSize );
         }
      }

      // https://codereview.chromium.org/279293002/
      mach_port_deallocate( mach_task_self(), host );

      return totalBytes > 0 && availableBytes > 0;
   }

   return false;

#endif // __PCL_MACOSX

#ifdef __PCL_WINDOWS

   MEMORYSTATUSEX m;
   m.dwLength = sizeof( m );
   GlobalMemoryStatusEx( &m );
   totalBytes = size_type( m.ullTotalPhys );
   availableBytes = size_type( m.ullAvailPhys );
   return totalBytes > 0 && availableBytes > 0;

#endif
}

// ----------------------------------------------------------------------------

void MetaModule::LoadResource( const String& filePath, const String& rootPath )
{
   if ( (*API->Module->LoadResource)( ModuleHandle(), filePath.c_str(), rootPath.c_str() ) == api_false )
      throw APIFunctionError( "LoadResource" );
}

void MetaModule::UnloadResource( const String& filePath, const String& rootPath )
{
   if ( (*API->Module->UnloadResource)( ModuleHandle(), filePath.c_str(), rootPath.c_str() ) == api_false )
      throw APIFunctionError( "UnloadResource" );
}

// ----------------------------------------------------------------------------

Variant MetaModule::EvaluateScript( const String& sourceCode, const IsoString& language )
{
   api_property_value result;
   if ( (*API->Module->EvaluateScript)( ModuleHandle(), &result, sourceCode.c_str(), language.c_str() ) == api_false )
      throw APIFunctionError( "EvaluateScript" );
   return VariantFromAPIPropertyValue( result );
}

// ----------------------------------------------------------------------------

bool MetaModule::HasEntitlement( const IsoString& entitlement )
{
   return (*API->Module->HasEntitlement)( ModuleHandle(), entitlement.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------
// Global Context
// ----------------------------------------------------------------------------

class GlobalContextDispatcher
{
public:

   static void api_func OnLoad()
   {
      try
      {
         Module->OnLoad();
      }
      ERROR_HANDLER
   }

   static void api_func OnUnload()
   {
      try
      {
         Module->OnUnload();

         if ( Module != nullptr )
            for ( size_type i = 0; i < Module->Length(); ++i )
               if ( (*Module)[i] != nullptr )
               {
                  const ProcessInterface* iface = dynamic_cast<const ProcessInterface*>( (*Module)[i] );
                  if ( iface != nullptr )
                     if ( iface->LaunchCount() != 0 )
                     {
                        if ( iface->IsAutoSaveGeometryEnabled() )
                           iface->SaveGeometry();
                        iface->SaveSettings();
                     }
               }
      }
      ERROR_HANDLER
   }

   static void* api_func Allocate( size_type sz )
   {
      try
      {
         return Module->Allocate( sz );
      }
      ERROR_HANDLER
      return nullptr;
   }

   static void api_func Deallocate( void* p )
   {
      try
      {
         Module->Deallocate( p );
      }
      ERROR_HANDLER
   }
}; // GlobalContextDispatcher

// ----------------------------------------------------------------------------

void MetaModule::PerformAPIDefinitions() const
{
   (*API->ModuleDefinition->EnterModuleDefinitionContext)();

   (*API->ModuleDefinition->SetModuleOnLoadRoutine)( GlobalContextDispatcher::OnLoad );
   (*API->ModuleDefinition->SetModuleOnUnloadRoutine)( GlobalContextDispatcher::OnUnload );
   (*API->ModuleDefinition->SetModuleAllocationRoutine)( GlobalContextDispatcher::Allocate );
   (*API->ModuleDefinition->SetModuleDeallocationRoutine)( GlobalContextDispatcher::Deallocate );

   /*
    * Meta object Definitions
    */
   for ( size_type i = 0; i < Module->Length(); ++i )
   {
      const MetaObject* o = (*Module)[i];
      if ( o != nullptr )
         o->PerformAPIDefinitions();
   }

   (*API->ModuleDefinition->ExitModuleDefinitionContext)();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MetaModule.cpp - Released 2025-04-07T08:53:32Z
