//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Cursor.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/Bitmap.h>
#include <pcl/Cursor.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Cursor::Cursor( std_cursor stdShape )
   : UIObject( (*API->Cursor->CreateCursor)( ModuleHandle(), stdShape ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateCursor" );
}

Cursor::Cursor( const Bitmap& p, const pcl::Point& hotSpot )
   : UIObject( (*API->Cursor->CreateBitmapCursor)( ModuleHandle(), p.handle, hotSpot.x, hotSpot.y ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBitmapCursor" );
}

Cursor::Cursor( const Bitmap& p, int hotSpotX, int hotSpotY )
   : UIObject( (*API->Cursor->CreateBitmapCursor)( ModuleHandle(), p.handle, hotSpotX, hotSpotY ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBitmapCursor" );
}

// ----------------------------------------------------------------------------

Cursor& Cursor::Null()
{
   static Cursor* nullCursor = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullCursor == nullptr )
      nullCursor = new Cursor( nullptr );
   return *nullCursor;
}

// ----------------------------------------------------------------------------

pcl::Point Cursor::HotSpot() const
{
   pcl::Point p;
   (*API->Cursor->GetCursorHotSpot)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

pcl::Point Cursor::Position()
{
   pcl::Point p;
   (*API->Global->GetCursorPosition)( &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void Cursor::SetPosition( int x, int y )
{
   (*API->Global->SetCursorPosition)( x, y );
}

// ----------------------------------------------------------------------------

void* Cursor::CloneHandle() const
{
   return (*API->Cursor->CloneCursor)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Cursor.cpp - Released 2025-04-07T08:53:32Z
