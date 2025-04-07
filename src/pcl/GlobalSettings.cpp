//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/GlobalSettings.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/GlobalSettings.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool PixInsightSettings::GlobalFlag( const IsoString& globalId )
{
   api_bool b = api_false;
   (void)(*API->Global->GetGlobalFlag)( globalId.c_str(), &b );
   return b != api_false;
}

// ----------------------------------------------------------------------------

int PixInsightSettings::GlobalInteger( const IsoString& globalId )
{
   int32 i = 0;
   (void)(*API->Global->GetGlobalInteger)( globalId.c_str(), &i, api_true );
   return i;
}

// ----------------------------------------------------------------------------

unsigned PixInsightSettings::GlobalUnsigned( const IsoString& globalId )
{
   uint32 u = 0;
   (void)(*API->Global->GetGlobalInteger)( globalId.c_str(), &u, api_false );
   return u;
}

// ----------------------------------------------------------------------------

double PixInsightSettings::GlobalReal( const IsoString& globalId )
{
   double r = 0.0;
   (void)(*API->Global->GetGlobalReal)( globalId.c_str(), &r );
   return r;
}

// ----------------------------------------------------------------------------

RGBA PixInsightSettings::GlobalColor( const IsoString& globalId )
{
   uint32 rgba = 0;
   (void)(*API->Global->GetGlobalColor)( globalId.c_str(), &rgba );
   return rgba;
}

// ----------------------------------------------------------------------------

Font PixInsightSettings::GlobalFont( const IsoString& globalId )
{
   size_type len = 0;
   (*API->Global->GetGlobalFont)( globalId.c_str(), 0, &len, 0 );

   if ( len > 0 )
   {
      int sizeInPoints;
      String fontFace;
      fontFace.SetLength( len );
      if ( (*API->Global->GetGlobalFont)( globalId.c_str(), fontFace.Begin(), &len, &sizeInPoints ) == api_false )
         throw APIFunctionError( "GetGlobalFont" );
      fontFace.ResizeToNullTerminated();
      return Font( fontFace, sizeInPoints );
   }

   return Font::Null();
}

// ----------------------------------------------------------------------------

String PixInsightSettings::GlobalString( const IsoString& globalId )
{
   size_type len = 0;
   (*API->Global->GetGlobalString)( globalId.c_str(), 0, &len );

   String str;
   if ( len > 0 )
   {
      str.SetLength( len );
      if ( (*API->Global->GetGlobalString)( globalId.c_str(), str.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetGlobalString" );
      str.ResizeToNullTerminated();
   }
   return str;
}

// ----------------------------------------------------------------------------

PixInsightSettings::variable_type PixInsightSettings::GlobalVariableType( const IsoString& globalId )
{
   const char* id = globalId.c_str();

   if ( (*API->Global->GetGlobalFlag)( id, 0 ) != api_false )
      return GlobalVariableType::Flag;

   if ( (*API->Global->GetGlobalInteger)( id, 0, api_true ) != api_false )
      return GlobalVariableType::Integer;

   if ( (*API->Global->GetGlobalInteger)( id, 0, api_false ) != api_false )
      return GlobalVariableType::Unsigned;

   if ( (*API->Global->GetGlobalReal)( id, 0 ) != api_false )
      return GlobalVariableType::Real;

   if ( (*API->Global->GetGlobalString)( id, 0, 0 ) != api_false )
      return GlobalVariableType::String;

   if ( (*API->Global->GetGlobalColor)( id, 0 ) != api_false )
      return GlobalVariableType::Color;

   if ( (*API->Global->GetGlobalFont)( id, 0, 0, 0 ) != api_false )
      return GlobalVariableType::Font;

   return GlobalVariableType::Undefined;
}

// ----------------------------------------------------------------------------

void PixInsightSettings::BeginUpdate()
{
   if ( (*API->Global->IsGlobalSettingsUpdateContextActive)() )
      throw Error( "Illegal call to PixInsightSettings::BeginUpdate()" );

   if ( !(*API->Global->EnterGlobalSettingsUpdateContext)() )
      throw APIFunctionError( "EnterGlobalSettingsUpdateContext" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::EndUpdate()
{
   if ( !(*API->Global->IsGlobalSettingsUpdateContextActive)() )
      throw Error( "Illegal call to PixInsightSettings::EndUpdate()" );

   if ( !(*API->Global->ExitGlobalSettingsUpdateContext)() )
      throw APIFunctionError( "ExitGlobalSettingsUpdateContext" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::CancelUpdate()
{
   if ( !(*API->Global->IsGlobalSettingsUpdateContextActive)() )
      throw Error( "Illegal call to PixInsightSettings::CancelUpdate()" );

   if ( !(*API->Global->CancelGlobalSettingsUpdate)( ModuleHandle(), 0 ) )
      throw APIFunctionError( "CancelGlobalSettingsUpdate" );
   if ( !(*API->Global->ExitGlobalSettingsUpdateContext)() )
      throw APIFunctionError( "ExitGlobalSettingsUpdateContext" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalFlag( const IsoString& globalId, bool b )
{
   if ( !(*API->Global->SetGlobalFlag)( globalId.c_str(), api_bool( b ) ) )
      throw APIFunctionError( "SetGlobalFlag" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalInteger( const IsoString& globalId, int i )
{
   if ( !(*API->Global->SetGlobalInteger)( globalId.c_str(), i, api_true ) )
      throw APIFunctionError( "SetGlobalInteger" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalUnsigned( const IsoString& globalId, unsigned u )
{
   if ( !(*API->Global->SetGlobalInteger)( globalId.c_str(), u, api_false ) )
      throw APIFunctionError( "SetGlobalInteger" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalReal( const IsoString& globalId, double f )
{
   if ( !(*API->Global->SetGlobalReal)( globalId.c_str(), f ) )
      throw APIFunctionError( "SetGlobalReal" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalColor( const IsoString& globalId, RGBA c )
{
   if ( !(*API->Global->SetGlobalColor)( globalId.c_str(), c ) )
      throw APIFunctionError( "SetGlobalColor" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalFont( const IsoString& globalId, const Font& f )
{
   String fontFace = f.Face();
   if ( !(*API->Global->SetGlobalFont)( globalId.c_str(), fontFace.c_str(), RoundInt( f.PointSize() ) ) )
      throw APIFunctionError( "SetGlobalFont" );
}

// ----------------------------------------------------------------------------

void PixInsightSettings::SetGlobalString( const IsoString& globalId, const String& s )
{
   if ( !(*API->Global->SetGlobalString)( globalId.c_str(), s.c_str() ) )
      throw APIFunctionError( "SetGlobalString" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/GlobalSettings.cpp - Released 2025-04-07T08:53:32Z
