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
   : UIObject( (*API->SVG->CreateSVGFile)( ModuleHandle(), filePath.c_str(), width, height, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSVGFile" );
}

// ----------------------------------------------------------------------------

SVG::SVG( int width, int height )
   : UIObject( (*API->SVG->CreateSVGBuffer)( ModuleHandle(), width, height, 0/*flags*/ ) )
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

   if ( (*API->SVG->GetSVGDimensions)( handle, &w, &h ) == api_false )
      throw APIFunctionError( "GetSVGDimensions" );
}

// ----------------------------------------------------------------------------

void SVG::SetDimensions( int w, int h ) const
{
   if ( (*API->SVG->SetSVGDimensions)( handle, w, h ) == api_false )
      throw APIFunctionError( "SetSVGDimensions" );
}

// ----------------------------------------------------------------------------

DRect SVG::ViewBox() const
{
   DRect r;
   if ( (*API->SVG->GetSVGViewBox)( handle, &r.x0, &r.y0, &r.x1, &r.y1 ) == api_false )
      throw APIFunctionError( "GetSVGViewBox" );
   return r;
}

// ----------------------------------------------------------------------------

void SVG::SetViewBox( double x0, double y0, double x1, double y1 )
{
   if ( (*API->SVG->SetSVGViewBox)( handle, x0, y0, x1, y1 ) == api_false )
      throw APIFunctionError( "SetSVGViewBox" );
}

// ----------------------------------------------------------------------------

int SVG::Resolution() const
{
   return (*API->SVG->GetSVGResolution)( handle );
}

// ----------------------------------------------------------------------------

void SVG::SetResolution( int r )
{
   (*API->SVG->SetSVGResolution)( handle, r );
}

// ----------------------------------------------------------------------------

String SVG::FilePath() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      (*API->SVG->GetSVGFilePath)( handle, 0, &len );
      if ( len > 0 )
      {
         String path;
         path.SetLength( len );
         if ( (*API->SVG->GetSVGFilePath)( handle, path.Begin(), &len ) == api_false )
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
      (*API->SVG->GetSVGDataBuffer)( handle, 0, &len );
      if ( len > 0 )
      {
         ByteArray data( len );
         if ( (*API->SVG->GetSVGDataBuffer)( handle, data.Begin(), &len ) == api_false )
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
      (*API->SVG->GetSVGTitle)( handle, 0, &len );
      if ( len > 0 )
      {
         String title;
         title.SetLength( len );
         if ( (*API->SVG->GetSVGTitle)( handle, title.Begin(), &len ) == api_false )
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
   (*API->SVG->SetSVGTitle)( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

String SVG::Description() const
{
   if ( !IsNull() )
   {
      size_type len = 0;
      (*API->SVG->GetSVGDescription)( handle, 0, &len );
      if ( len > 0 )
      {
         String description;
         description.SetLength( len );
         if ( (*API->SVG->GetSVGDescription)( handle, description.Begin(), &len ) == api_false )
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
   (*API->SVG->SetSVGDescription)( handle, desc.c_str() );
}

// ----------------------------------------------------------------------------

bool SVG::IsPainting() const
{
   return (*API->SVG->IsSVGPainting)( handle ) != api_false;
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
