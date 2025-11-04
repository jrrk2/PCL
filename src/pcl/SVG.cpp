//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SVG.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/SVG.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SVG::SVG( const String& filePath, int width, int height )
   : UIObject( API_SVG_CreateSVGFile( ModuleHandle(), filePath.c_str(), width, height, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSVGFile" );
}

// ----------------------------------------------------------------------------

SVG::SVG( int width, int height )
   : UIObject( API_SVG_CreateSVGBuffer( ModuleHandle(), width, height, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSVGBuffer" );
}

// ----------------------------------------------------------------------------

SVG& SVG::Null()
{
   static SVG* nullSVG = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullSVG == nullptr )
      nullSVG = new SVG( reinterpret_cast<void*>( 0 ) );
   return *nullSVG;
}

// ----------------------------------------------------------------------------

void SVG::GetDimensions( int& w, int& h ) const
{
   if ( IsNull() )
   {
      w = h = 0;
      return;
   }

   if ( API_SVG_GetSVGDimensions( handle, &w, &h ) == api_false )
      throw APIFunctionError( "GetSVGDimensions" );
}

// ----------------------------------------------------------------------------

void SVG::SetDimensions( int w, int h ) const
{
   if ( API_SVG_SetSVGDimensions( handle, w, h ) == api_false )
      throw APIFunctionError( "SetSVGDimensions" );
}

// ----------------------------------------------------------------------------

DRect SVG::ViewBox() const
{
   DRect r;
   if ( API_SVG_GetSVGViewBox( handle, &r.x0, &r.y0, &r.x1, &r.y1 ) == api_false )
      throw APIFunctionError( "GetSVGViewBox" );
   return r;
}

// ----------------------------------------------------------------------------

void SVG::SetViewBox( double x0, double y0, double x1, double y1 )
{
   if ( API_SVG_SetSVGViewBox( handle, x0, y0, x1, y1 ) == api_false )
      throw APIFunctionError( "SetSVGViewBox" );
}

// ----------------------------------------------------------------------------

int SVG::Resolution() const
{
   return API_SVG_GetSVGResolution( handle );
}

// ----------------------------------------------------------------------------

void SVG::SetResolution( int r )
{
   API_SVG_SetSVGResolution( handle, r );
}

// ----------------------------------------------------------------------------

String SVG::FilePath() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      API_SVG_GetSVGFilePath( handle, 0, &len );
      if ( len > 0 )
      {
         String path;
         path.SetLength( len );
         if ( API_SVG_GetSVGFilePath( handle, path.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetSVGFilePath" );
         path.ResizeToNullTerminated();
         return path;
      }
   }

   return String();
}

// ----------------------------------------------------------------------------

ByteArray SVG::Data() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      API_SVG_GetSVGDataBuffer( handle, 0, &len );
      if ( len > 0 )
      {
         ByteArray data( len );
         if ( API_SVG_GetSVGDataBuffer( handle, data.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetSVGDataBuffer" );
         return data;
      }
   }

   return ByteArray();
}

// ----------------------------------------------------------------------------

String SVG::Title() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      API_SVG_GetSVGTitle( handle, 0, &len );
      if ( len > 0 )
      {
         String title;
         title.SetLength( len );
         if ( API_SVG_GetSVGTitle( handle, title.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetSVGTitle" );
         title.ResizeToNullTerminated();
         return title;
      }
   }

   return String();
}

// ----------------------------------------------------------------------------

void SVG::SetTitle( const String& title )
{
   API_SVG_SetSVGTitle( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

String SVG::Description() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      API_SVG_GetSVGDescription( handle, 0, &len );
      if ( len > 0 )
      {
         String description;
         description.SetLength( len );
         if ( API_SVG_GetSVGDescription( handle, description.Begin(), &len ) == api_false )
            throw APIFunctionError( "GetSVGDescription" );
         description.ResizeToNullTerminated();
         return description;
      }
   }

   return String();
}

// ----------------------------------------------------------------------------

void SVG::SetDescription( const String& desc )
{
   API_SVG_SetSVGDescription( handle, desc.c_str() );
}

// ----------------------------------------------------------------------------

bool SVG::IsPainting() const
{
   return API_SVG_IsSVGPainting( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void* SVG::CloneHandle() const
{
   throw Error( "Cannot clone a SVG handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SVG.cpp - Released 2025-04-07T08:53:32Z
