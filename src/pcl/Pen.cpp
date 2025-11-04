//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Pen.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/Brush.h>
#include <pcl/Pen.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Pen::Pen( RGBA color, float width, Pen::style style, Pen::cap cap, Pen::join join )
   : UIObject( API_Pen_CreatePen( ModuleHandle(), color, width, style, cap, join ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreatePen" );
}

// ----------------------------------------------------------------------------

Pen& Pen::Null()
{
   static Pen* nullPen = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullPen == nullptr )
      nullPen = new Pen( reinterpret_cast<void*>( 0 ) );
   return *nullPen;
}

// ----------------------------------------------------------------------------

float Pen::Width() const
{
   float width;
   if ( API_Pen_GetPenWidth( handle, &width ) == api_false )
      throw APIFunctionError( "GetPenWidth" );
   return width;
}

// ----------------------------------------------------------------------------

void Pen::SetWidth( float width )
{
   EnsureUnique();
   API_Pen_SetPenWidth( handle, width );
}

// ----------------------------------------------------------------------------

RGBA Pen::Color() const
{
   return API_Pen_GetPenColor( handle );
}

// ----------------------------------------------------------------------------

void Pen::SetColor( RGBA color )
{
   EnsureUnique();
   API_Pen_SetPenColor( handle, color );
}

// ----------------------------------------------------------------------------

Pen::style Pen::Style() const
{
   return style( API_Pen_GetPenStyle( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetStyle( Pen::style style )
{
   EnsureUnique();
   API_Pen_SetPenStyle( handle, style );
}

// ----------------------------------------------------------------------------

Pen::cap Pen::Cap() const
{
   return cap( API_Pen_GetPenCap( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetCap( Pen::cap cap )
{
   EnsureUnique();
   API_Pen_SetPenCap( handle, cap );
}

// ----------------------------------------------------------------------------

Pen::join Pen::Join() const
{
   return join( API_Pen_GetPenJoin( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetJoin( Pen::join join )
{
   EnsureUnique();
   API_Pen_SetPenJoin( handle, join );
}

// ----------------------------------------------------------------------------

pcl::Brush Pen::Brush() const
{
   return pcl::Brush( API_Pen_GetPenBrush( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetBrush( const pcl::Brush& brush )
{
   EnsureUnique();
   API_Pen_SetPenBrush( handle, brush.handle );
}

// ----------------------------------------------------------------------------

void* Pen::CloneHandle() const
{
   return API_Pen_ClonePen( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Pen.cpp - Released 2025-04-07T08:53:32Z
