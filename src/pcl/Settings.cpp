//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Settings.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Settings.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#ifdef ERROR
#undef ERROR
#endif

#define ERROR( what, key ) throw APIError( String( what ) + ": " + key )

// ----------------------------------------------------------------------------

static bool s_lastReadOK = false;

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, ByteArray& a )
{
   uint8* p = 0;
   size_type n = 0;
   s_lastReadOK = API_Global_ReadSettingsBlock( ModuleHandle(), (void**)&p, &n, key.c_str(), api_false ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      if ( n != 0 )
         a.Assign( p, p+n );
      else
         a.Clear();

      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, ByteArray& a )
{
   uint8* p = 0;
   size_type n = 0;
   s_lastReadOK = API_Global_ReadSettingsBlock( ModuleHandle(), (void**)&p, &n, key.c_str(), api_true ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      if ( n != 0 )
         a.Assign( p, p+n );
      else
         a.Clear();

      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, String& s )
{
   char16_type* p = 0;
   s_lastReadOK = API_Global_ReadSettingsString( ModuleHandle(), &p, key.c_str(), api_false ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      s = String( p );
      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, String& s )
{
   char16_type* p = 0;
   s_lastReadOK = API_Global_ReadSettingsString( ModuleHandle(), &p, key.c_str(), api_true ) != api_false;

   if ( s_lastReadOK && p != 0 )
   {
      s = String( p );
      Module->Deallocate( p );
   }

   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, bool& b )
{
   api_bool ab;
   s_lastReadOK = API_Global_ReadSettingsFlag( ModuleHandle(), &ab, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      b = ab != api_false;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, bool& b )
{
   api_bool ab;
   s_lastReadOK = API_Global_ReadSettingsFlag( ModuleHandle(), &ab, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      b = ab != api_false;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, int& i )
{
   int32 ai;
   s_lastReadOK = API_Global_ReadSettingsInteger( ModuleHandle(), &ai, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      i = ai;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, int& i )
{
   int32 ai;
   s_lastReadOK = API_Global_ReadSettingsInteger( ModuleHandle(), &ai, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      i = ai;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, unsigned& u )
{
   uint32 au;
   s_lastReadOK = API_Global_ReadSettingsUnsignedInteger( ModuleHandle(), &au, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      u = au;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, unsigned& u )
{
   uint32 au;
   s_lastReadOK = API_Global_ReadSettingsUnsignedInteger( ModuleHandle(), &au, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      u = au;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::Read( const IsoString& key, double& d )
{
   double ad;
   s_lastReadOK = API_Global_ReadSettingsReal( ModuleHandle(), &ad, key.c_str(), api_false ) != api_false;
   if ( s_lastReadOK )
      d = ad;
   return s_lastReadOK;
}

bool Settings::ReadGlobal( const IsoString& key, double& d )
{
   double ad;
   s_lastReadOK = API_Global_ReadSettingsReal( ModuleHandle(), &ad, key.c_str(), api_true ) != api_false;
   if ( s_lastReadOK )
      d = ad;
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

bool Settings::LastReadOK()
{
   return s_lastReadOK;
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, const ByteArray& a )
{
   if ( API_Global_WriteSettingsBlock( ModuleHandle(), a.Begin(), a.Length(), key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local ByteArray data", key );
}

void Settings::WriteGlobal( const IsoString& key, const ByteArray& a )
{
   if ( API_Global_WriteSettingsBlock( ModuleHandle(), a.Begin(), a.Length(), key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global ByteArray data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, const pcl::String& s )
{
   if ( API_Global_WriteSettingsString( ModuleHandle(), s.c_str(), key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local String data", key );
}

void Settings::WriteGlobal( const IsoString& key, const pcl::String& s )
{
   if ( API_Global_WriteSettingsString( ModuleHandle(), s.c_str(), key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global String data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, bool b )
{
   if ( API_Global_WriteSettingsFlag( ModuleHandle(), b, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local boolean data", key );
}

void Settings::WriteGlobal( const IsoString& key, bool b )
{
   if ( API_Global_WriteSettingsFlag( ModuleHandle(), b, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global boolean data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, int i )
{
   if ( API_Global_WriteSettingsInteger( ModuleHandle(), i, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local integer data", key );
}

void Settings::WriteGlobal( const IsoString& key, int i )
{
   if ( API_Global_WriteSettingsInteger( ModuleHandle(), i, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global integer data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, unsigned u )
{
   if ( API_Global_WriteSettingsUnsignedInteger( ModuleHandle(), u, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local unsigned integer data", key );
}

void Settings::WriteGlobal( const IsoString& key, unsigned u )
{
   if ( API_Global_WriteSettingsUnsignedInteger( ModuleHandle(), u, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global unsigned integer data", key );
}

// ----------------------------------------------------------------------------

void Settings::Write( const IsoString& key, double d )
{
   if ( API_Global_WriteSettingsReal( ModuleHandle(), d, key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error writing local floating point data", key );
}

void Settings::WriteGlobal( const IsoString& key, double d )
{
   if ( API_Global_WriteSettingsReal( ModuleHandle(), d, key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error writing global floating point data", key );
}

// ----------------------------------------------------------------------------

void Settings::Remove( const IsoString& key )
{
   if ( API_Global_DeleteSettingsItem( ModuleHandle(), key.c_str(), api_false ) == api_false )
      ERROR( "Settings: Error removing local settings key", key );
}

void Settings::RemoveGlobal( const IsoString& key )
{
   if ( API_Global_DeleteSettingsItem( ModuleHandle(), key.c_str(), api_true ) == api_false )
      ERROR( "Settings: Error removing global settings key", key );
}

// ----------------------------------------------------------------------------

bool Settings::CanReadGlobal( const IsoString& key )
{
   return (API_Global_GetSettingsItemGlobalAccess( ModuleHandle(), key.c_str() ) & 0x02) != 0;
}

bool Settings::CanWriteGlobal( const IsoString& key )
{
   return (API_Global_GetSettingsItemGlobalAccess( ModuleHandle(), key.c_str() ) & 0x01) != 0;
}

void Settings::SetGlobalKeyAccess( const IsoString& key, bool allowRead, bool allowWrite )
{
   uint32 flags = 0;
   if ( allowRead ) flags |= 0x02;
   if ( allowWrite ) flags |= 0x01;
   if ( API_Global_SetSettingsItemGlobalAccess( ModuleHandle(), key.c_str(), flags ) == api_false )
      ERROR( "Settings: Error setting global key access", key );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Settings.cpp - Released 2025-04-07T08:53:32Z
