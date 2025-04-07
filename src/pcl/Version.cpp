//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Version.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/MetaModule.h>
#include <pcl/Version.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int Version::Major()
{
   return 2;
}

// ----------------------------------------------------------------------------

int Version::Minor()
{
   return 9;
}

// ----------------------------------------------------------------------------

int Version::Release()
{
   return 4;
}

// ----------------------------------------------------------------------------

int Version::Build()
{
   return 1052;
}

// ----------------------------------------------------------------------------

int Version::BetaRelease()
{
   return 0;
}

// ----------------------------------------------------------------------------

String Version::LanguageCode()
{
   return "eng";  // ISO 639.2
}

// ----------------------------------------------------------------------------

String Version::AsString()
{
   String v = String().Format( "PCL %d.%d.%d", Major(), Minor(), Release() );
   if ( BetaRelease() > 0 )
      v.Append( String().Format( " beta %d", BetaRelease() ) );
   return v;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static int s_major = 0;
static int s_minor = 0;
static int s_release = 0;
static int s_revision = 0;
static int s_beta = 0;
static bool s_confidential = false;
static bool s_le = false;
static String s_language;
static String s_codename;

/*
 * Thread-safe version data initialization.
 */
static void Initialize()
{
   static AtomicInt s_initialized;
   static Mutex s_mutex;

   if ( !s_initialized )
   {
      volatile AutoLock lock( s_mutex );
      if ( s_initialized.Load() == 0 )
      {
         if ( API != nullptr )
         {
            uint32 major, minor, release, revision, beta, conf, le;
            char lang[ 8 ];
            (*API->Global->GetPixInsightVersion)( &major, &minor, &release, &revision, &beta, &conf, &le, lang );
            s_major = int( major );
            s_minor = int( minor );
            s_release = int( release );
            s_revision = int( revision );
            s_beta = int( beta );
            s_confidential = conf != 0u;
            s_le = le != 0u;
            s_language = lang;

            char16_type* s = (*API->Global->GetPixInsightCodename)( ModuleHandle() );
            if ( s != nullptr )
            {
               s_codename = String( s );
               if ( Module != nullptr )
                  Module->Deallocate( s );
            }
         }

         s_initialized.Store( 1 );
      }
   }
}

// ----------------------------------------------------------------------------

int PixInsightVersion::Major()
{
   Initialize();
   return s_major;
}

// ----------------------------------------------------------------------------

int PixInsightVersion::Minor()
{
   Initialize();
   return s_minor;
}

// ----------------------------------------------------------------------------

int PixInsightVersion::Release()
{
   Initialize();
   return s_release;
}

// ----------------------------------------------------------------------------

int PixInsightVersion::Revision()
{
   Initialize();
   return s_revision;
}

// ----------------------------------------------------------------------------

int PixInsightVersion::Build()
{
   return 1494; // kept fixed since core version 1.8.8-1
}

// ----------------------------------------------------------------------------

int PixInsightVersion::BetaRelease() // > 0 = Beta, < 0 = RC, 0 = Release
{
   Initialize();
   return s_beta;
}

// ----------------------------------------------------------------------------

bool PixInsightVersion::Confidential()
{
   Initialize();
   return s_confidential;
}

// ----------------------------------------------------------------------------

bool PixInsightVersion::LE()
{
   Initialize();
   return s_le;
}

// ----------------------------------------------------------------------------

String PixInsightVersion::LanguageCode()
{
   Initialize();
   return s_language;  // ISO 639.2
}

// ----------------------------------------------------------------------------

String PixInsightVersion::Codename()
{
   Initialize();
   return s_codename;
}

// ----------------------------------------------------------------------------

String PixInsightVersion::AsString( bool withCodename )
{
   Initialize();
   String v = String().Format( "PixInsight %s%d.%d.%d",
               LE() ? "LE " : "", Major(), Minor(), Release() );
   if ( Revision() != 0 )
      v.AppendFormat( "-%d", Revision() );
   if ( BetaRelease() != 0 )
      v.AppendFormat( " %s%d", (BetaRelease() < 0) ? "RC" : "beta ", Abs( BetaRelease() ) );
   if ( withCodename )
      v.Append( ' ' + Codename() );
   if ( Confidential() )
      v.Append( " (confidential)" );
   return v;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Version.cpp - Released 2025-04-07T08:53:32Z
