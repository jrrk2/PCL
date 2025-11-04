//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Sizer.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/Control.h>
#include <pcl/Sizer.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Sizer::Sizer( bool vertical )
   : UIObject( API_Sizer_CreateSizer( ModuleHandle(), vertical ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateSizer" );
}

// ----------------------------------------------------------------------------

Sizer& Sizer::Null()
{
   static Sizer* nullSizer = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullSizer == nullptr )
      nullSizer = new Sizer( reinterpret_cast<void*>( 0 ) );
   return *nullSizer;
}

// ----------------------------------------------------------------------------

Control& Sizer::ParentControl() const
{
   control_handle hParent = API_Sizer_GetSizerParentControl( handle ); // N.B.: client handle
   return (hParent != 0) ? *reinterpret_cast<Control*>( hParent ) : Control::Null();
}

// ----------------------------------------------------------------------------

bool Sizer::IsVertical() const
{
   return API_Sizer_GetSizerOrientation( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int Sizer::NumberOfItems() const
{
   return API_Sizer_GetSizerCount( handle );
}

// ----------------------------------------------------------------------------

int Sizer::IndexOf( const Sizer& s ) const
{
   return s.IsNull() ? -1 : API_Sizer_GetSizerIndex( handle, s.handle );
}

// ----------------------------------------------------------------------------

int Sizer::IndexOf( const Control& c ) const
{
   return c.IsNull() ? -1 : API_Sizer_GetSizerControlIndex( handle, c.handle );
}

// ----------------------------------------------------------------------------

void Sizer::Add( Sizer& s, int stretchFactor )
{
   if ( !s.IsNull() )
      API_Sizer_InsertSizer( handle, -1, s.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Add( Control& c, int stretchFactor, Sizer::item_alignment align )
{
   if ( !c.IsNull() )
      API_Sizer_InsertSizerControl( handle, -1, c.handle, stretchFactor, align );
}

// ----------------------------------------------------------------------------

void Sizer::AddSpacing( int size, bool autoScaling )
{
   API_Sizer_InsertSizerSpacing( handle, -1, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

void Sizer::AddStretch( int stretchFactor )
{
   API_Sizer_InsertSizerStretch( handle, -1, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Insert( int index, Sizer& s, int stretchFactor )
{
   if ( !s.IsNull() )
      API_Sizer_InsertSizer( handle, index, s.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Insert( int index, Control& c, int stretchFactor, Sizer::item_alignment align )
{
   if ( !c.IsNull() )
      API_Sizer_InsertSizerControl( handle, index, c.handle, stretchFactor, align );
}

// ----------------------------------------------------------------------------

void Sizer::InsertSpacing( int index, int size, bool autoScaling )
{
   API_Sizer_InsertSizerSpacing( handle, index, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

void Sizer::InsertStretch( int index, int stretchFactor )
{
   API_Sizer_InsertSizerStretch( handle, index, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::Remove( Sizer& s )
{
   if ( !s.IsNull() )
      API_Sizer_RemoveSizer( handle, s.handle );
}

// ----------------------------------------------------------------------------

void Sizer::Remove( Control& c )
{
   if ( !c.IsNull() )
      API_Sizer_RemoveSizerControl( handle, c.handle );
}

// ----------------------------------------------------------------------------

void Sizer::SetStretchFactor( Sizer& s, int stretchFactor )
{
   if ( !s.IsNull() )
      API_Sizer_SetSizerStretchFactor( handle, s.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::SetStretchFactor( Control& c, int stretchFactor )
{
   if ( !c.IsNull() )
      API_Sizer_SetSizerControlStretchFactor( handle, c.handle, stretchFactor );
}

// ----------------------------------------------------------------------------

void Sizer::SetAlignment( Sizer& s, Sizer::item_alignment align )
{
   if ( !s.IsNull() )
      API_Sizer_SetSizerAlignment( handle, s.handle, align );
}

// ----------------------------------------------------------------------------

void Sizer::SetAlignment( Control& c, Sizer::item_alignment align )
{
   if ( !c.IsNull() )
      API_Sizer_SetSizerControlAlignment( handle, c.handle, align );
}

// ----------------------------------------------------------------------------

int Sizer::Margin( bool autoScaling ) const
{
   int margin = API_Sizer_GetSizerMargin( handle );
   return autoScaling ? PhysicalPixelsToLogical( margin ) : margin;
}

// ----------------------------------------------------------------------------

void Sizer::SetMargin( int size, bool autoScaling )
{
   API_Sizer_SetSizerMargin( handle, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

int Sizer::Spacing( bool autoScaling ) const
{
   int spacing = API_Sizer_GetSizerSpacing( handle );
   return autoScaling ? PhysicalPixelsToLogical( spacing ) : spacing;
}

// ----------------------------------------------------------------------------

void Sizer::SetSpacing( int size, bool autoScaling )
{
   API_Sizer_SetSizerSpacing( handle, autoScaling ? LogicalPixelsToPhysical( size ) : size );
}

// ----------------------------------------------------------------------------

double Sizer::DisplayPixelRatio() const
{
   double r;
   if ( API_Sizer_GetSizerDisplayPixelRatio( handle, &r ) == api_false )
      throw APIFunctionError( "GetSizerDisplayPixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

void* Sizer::CloneHandle() const
{
   throw Error( "Cannot clone a Sizer handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Sizer.cpp - Released 2025-04-07T08:53:32Z
