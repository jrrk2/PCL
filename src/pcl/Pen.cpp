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
   : UIObject( (*API->Pen->CreatePen)( ModuleHandle(), color, width, style, cap, join ) )
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
   if ( (*API->Pen->GetPenWidth)( handle, &width ) == api_false )
      throw APIFunctionError( "GetPenWidth" );
   return width;
}

// ----------------------------------------------------------------------------

void Pen::SetWidth( float width )
{
   EnsureUnique();
   (*API->Pen->SetPenWidth)( handle, width );
}

// ----------------------------------------------------------------------------

RGBA Pen::Color() const
{
   return (*API->Pen->GetPenColor)( handle );
}

// ----------------------------------------------------------------------------

void Pen::SetColor( RGBA color )
{
   EnsureUnique();
   (*API->Pen->SetPenColor)( handle, color );
}

// ----------------------------------------------------------------------------

Pen::style Pen::Style() const
{
   return style( (*API->Pen->GetPenStyle)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetStyle( Pen::style style )
{
   EnsureUnique();
   (*API->Pen->SetPenStyle)( handle, style );
}

// ----------------------------------------------------------------------------

Pen::cap Pen::Cap() const
{
   return cap( (*API->Pen->GetPenCap)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetCap( Pen::cap cap )
{
   EnsureUnique();
   (*API->Pen->SetPenCap)( handle, cap );
}

// ----------------------------------------------------------------------------

Pen::join Pen::Join() const
{
   return join( (*API->Pen->GetPenJoin)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetJoin( Pen::join join )
{
   EnsureUnique();
   (*API->Pen->SetPenJoin)( handle, join );
}

// ----------------------------------------------------------------------------

pcl::Brush Pen::Brush() const
{
   return pcl::Brush( (*API->Pen->GetPenBrush)( handle ) );
}

// ----------------------------------------------------------------------------

void Pen::SetBrush( const pcl::Brush& brush )
{
   EnsureUnique();
   (*API->Pen->SetPenBrush)( handle, brush.handle );
}

// ----------------------------------------------------------------------------

void* Pen::CloneHandle() const
{
   return (*API->Pen->ClonePen)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Pen.cpp - Released 2025-04-07T08:53:32Z
