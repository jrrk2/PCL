//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Brush.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/Brush.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Brush::Brush( RGBA color, style style )
   : UIObject( API_Brush_CreateBrush( ModuleHandle(), color, style ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBrush" );
}

Brush::Brush( const Bitmap& pm )
   : UIObject( API_Brush_CreateBitmapBrush( ModuleHandle(), pm.handle ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateBitmapBrush" );
}

Brush& Brush::Null()
{
   static Brush* nullBrush = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullBrush == nullptr )
      nullBrush = new Brush( nullptr );
   return *nullBrush;
}

RGBA Brush::Color() const
{
   return API_Brush_GetBrushColor( handle );
}

void Brush::SetColor( RGBA color )
{
   EnsureUnique();
   API_Brush_SetBrushColor( handle, color );
}

Brush::style Brush::Style() const
{
   return style( API_Brush_GetBrushStyle( handle ) );
}

void Brush::SetStyle( Brush::style style )
{
   EnsureUnique();
   API_Brush_SetBrushStyle( handle, style );
}

Bitmap Brush::Stipple() const
{
   return Bitmap( API_Brush_GetBrushBitmap( handle ) );
}

void Brush::SetStipple( const Bitmap& pm )
{
   EnsureUnique();
   API_Brush_SetBrushBitmap( handle, pm.handle );
}

void* Brush::CloneHandle() const
{
   return API_Brush_CloneBrush( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

GradientBrush::stop_list GradientBrush::Stops() const
{
   if ( !IsNull() )
   {
      size_type count = 0;
      API_Brush_GetBrushGradientStops( handle, 0, &count );
      if ( count > 0 )
      {
         stop_list stops( count );
         if ( API_Brush_GetBrushGradientStops( handle, reinterpret_cast<api_gradient_stop*>( stops.Begin() ), &count ) == api_false )
            throw APIFunctionError( "GetBrushGradientStops" );
         return stops;
      }
   }

   return stop_list();
}

GradientBrush::spread_mode GradientBrush::SpreadMode() const
{
   switch ( API_Brush_GetBrushGradientSpread( handle ) )
   {
   case 0: return GradientSpreadMode::Pad;
   case 1: return GradientSpreadMode::Reflect;
   case 2: return GradientSpreadMode::Repeat;
   default:
      throw APIFunctionError( "GetBrushGradientSpread" );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

LinearGradientBrush::LinearGradientBrush( double x1, double y1, double x2, double y2, const stop_list& stops, spread_mode spread )
   : GradientBrush( API_Brush_CreateLinearGradientBrush( ModuleHandle(), x1, y1, x2, y2, spread,
                                                              reinterpret_cast<const api_gradient_stop*>( stops.Begin() ),
                                                              stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateLinearGradientBrush" );
}

LinearGradientBrush::LinearGradientBrush( const DRect& r, const stop_list& stops, spread_mode spread )
   : GradientBrush( API_Brush_CreateLinearGradientBrush( ModuleHandle(), r.x0, r.y0, r.x1, r.y1, spread,
                                                              reinterpret_cast<const api_gradient_stop*>( stops.Begin() ),
                                                              stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateLinearGradientBrush" );
}

LinearGradientBrush::LinearGradientBrush( const Rect& r, const stop_list& stops, spread_mode spread )
   : GradientBrush( (void*)0 )
{
   DRect d( r );
   void* h = API_Brush_CreateLinearGradientBrush(
                  ModuleHandle(), d.x0, d.y0, d.x1, d.y1, spread,
                  reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() );
   if ( h == 0 )
      throw APIFunctionError( "CreateLinearGradientBrush" );

   SetHandle( h );
}

void LinearGradientBrush::GetParameters( double& x1, double& y1, double& x2, double& y2 ) const
{
   if ( API_Brush_GetBrushLinearGradientParameters( handle, &x1, &y1, &x2, &y2 ) == api_false )
      throw APIFunctionError( "GetBrushLinearGradientParameters" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RadialGradientBrush::RadialGradientBrush( double cx, double cy, double r, double fx, double fy, const stop_list& stops, spread_mode spread )
   : GradientBrush( API_Brush_CreateRadialGradientBrush( ModuleHandle(), cx, cy, r,
                                                   (fx == uint32_max) ? cx : fx,
                                                   (fy == uint32_max) ? cy : fy,
                                                   spread,
                                                   reinterpret_cast<const api_gradient_stop*>( stops.Begin() ),
                                                   stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateRadialGradientBrush" );
}

RadialGradientBrush::RadialGradientBrush( const DPoint& c, double r, const DPoint& f, const stop_list& stops, spread_mode spread )
   : GradientBrush( API_Brush_CreateRadialGradientBrush( ModuleHandle(), c.x, c.y, r,
                                                   (f.x == uint32_max) ? c.x : f.x,
                                                   (f.y == uint32_max) ? c.y : f.y,
                                                   spread,
                                                   reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateRadialGradientBrush" );
}

RadialGradientBrush::RadialGradientBrush( const Point& c, double r, const Point& f, const stop_list& stops, spread_mode spread )
   : GradientBrush( (void*)0 )
{
   DPoint dc( c );
   DPoint df( f );
   void* h = API_Brush_CreateRadialGradientBrush( ModuleHandle(), dc.x, dc.y, r,
                  (df.x == uint32_max) ? dc.x : df.x, (df.y == uint32_max) ? dc.y : df.y,
                  spread, reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() );
   if ( h == 0 )
      throw APIFunctionError( "CreateRadialGradientBrush" );

   SetHandle( h );
}

void RadialGradientBrush::GetParameters( double& cx, double& cy, double& r, double& fx, double& fy ) const
{
   if ( API_Brush_GetBrushRadialGradientParameters( handle, &cx, &cy, &r, &fx, &fy ) == api_false )
      throw APIFunctionError( "GetBrushRadialGradientParameters" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConicalGradientBrush::ConicalGradientBrush( double cx, double cy, double a, const stop_list& stops )
   : GradientBrush( API_Brush_CreateConicalGradientBrush( ModuleHandle(), cx, cy, a,
                                                   reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateConicalGradientBrush" );
}

ConicalGradientBrush::ConicalGradientBrush( const DPoint& c, double a, const stop_list& stops )
   : GradientBrush( API_Brush_CreateConicalGradientBrush( ModuleHandle(), c.x, c.y, a,
                                                               reinterpret_cast<const api_gradient_stop*>( stops.Begin() ),
                                                               stops.Length() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateConicalGradientBrush" );
}

ConicalGradientBrush::ConicalGradientBrush( const Point& c, double a, const stop_list& stops )
   : GradientBrush( (void*)0 )
{
   DPoint dc( c );
   void* h = API_Brush_CreateConicalGradientBrush( ModuleHandle(), dc.x, dc.y, a,
                                 reinterpret_cast<const api_gradient_stop*>( stops.Begin() ), stops.Length() );
   if ( h == 0 )
      throw APIFunctionError( "CreateConicalGradientBrush" );

   SetHandle( h );
}

void ConicalGradientBrush::GetParameters( double& cx, double& cy, double& a ) const
{
   if ( API_Brush_GetBrushConicalGradientParameters( handle, &cx, &cy, &a ) == api_false )
      throw APIFunctionError( "GetBrushConicalGradientParameters" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Brush.cpp - Released 2025-04-07T08:53:32Z
