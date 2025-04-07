//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Bitmap.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/ImageVariant.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Bitmap::Bitmap()
   : UIObject( (*API->Bitmap->CreateEmptyBitmap)( ModuleHandle() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateEmptyBitmap" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( int w, int h, pixel_format fmt )
   : UIObject( (*API->Bitmap->CreateBitmap)( ModuleHandle(), w, h, 0 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmap" );
   if ( fmt != BitmapFormat::ARGB32 )
      (*API->Bitmap->SetBitmapFormat)( handle, fmt );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const char** xpm )
   : UIObject( (*API->Bitmap->CreateBitmapXPM)( ModuleHandle(), xpm ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapXPM" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const String& filePath )
   : UIObject( (*API->Bitmap->CreateBitmapFromFile)( ModuleHandle(), filePath.c_str() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromFile" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const char* filePath )
   : UIObject( (*API->Bitmap->CreateBitmapFromFile8)( ModuleHandle(), filePath ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromFile8" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const IsoString& filePath )
   : UIObject( (*API->Bitmap->CreateBitmapFromFile8)( ModuleHandle(), filePath.c_str() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromFile8" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const void* data, size_type size, const char* format, uint32 flags )
   : UIObject( (*API->Bitmap->CreateBitmapFromData)( ModuleHandle(), data, size, format, flags ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromData" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const Bitmap& bmp, const pcl::Rect& r )
   : UIObject( (*API->Bitmap->CloneBitmapRect)( ModuleHandle(), bmp.handle, r.x0, r.y0, r.x1, r.y1 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CloneBitmapRect" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const Bitmap& bmp, int x0, int y0, int x1, int y1 )
   : UIObject( (*API->Bitmap->CloneBitmapRect)( ModuleHandle(), bmp.handle, x0, y0, x1, y1 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CloneBitmapRect" );
}

// ----------------------------------------------------------------------------

Bitmap& Bitmap::Null()
{
   static Bitmap* nullBitmap = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullBitmap == nullptr )
      nullBitmap = new Bitmap( reinterpret_cast<void*>( 0 ) );
   return *nullBitmap;
}

// ----------------------------------------------------------------------------

Bitmap::pixel_format Bitmap::PixelFormat() const
{
   return pixel_format( (*API->Bitmap->GetBitmapFormat)( handle ) );
}

// ----------------------------------------------------------------------------

void Bitmap::SetPixelFormat( pixel_format fmt )
{
   (*API->Bitmap->SetBitmapFormat)( handle, fmt );
}

// ----------------------------------------------------------------------------

// Implemented in Render.cpp
extern bool Render( Bitmap& bitmap, int x, int y, int zoom, int channel,
                    const pcl::ImageVariant& image,  bool transparency,
                    const pcl::ImageVariant* mask, int maskMode, bool maskInverted,
                    const uint8** LUT, bool fast, bool (*callback)() );

Bitmap Bitmap::Render( const ImageVariant& image, int zoom, display_channel displayChannel, bool transparency,
                       const ImageVariant* mask, mask_mode maskMode, bool maskInverted,
                       const uint8** LUT, bool fast, bool (*callback)() )
{
   if ( !image || image->IsEmptySelection() )
      return Null();

   zoom = pcl::Range( zoom, -32, +100 );
   if ( zoom == 0 )
      zoom = 1;

   if ( mask != 0 &&
         (!*mask ||
            mask->Width() != image->Width() ||
            mask->Height() != image->Height() ||
            mask->IsColor() && !image->IsColor()) )
   {
      mask = 0;
   }

   pcl::Rect r = image->SelectedRectangle();
   int w = r.Width();
   int h = r.Height();

   if ( zoom > 0 )
      w *= zoom, h *= zoom;
   else
      w /= -zoom, h /= -zoom;

   Bitmap bmp( pcl::Max( 1, w ), pcl::Max( 1, h ) );

   if ( !pcl::Render( bmp, 0, 0, zoom, displayChannel, image, transparency,
                      mask, maskMode, maskInverted, LUT, fast, callback ) )
      return Null();

   return bmp;
}

// ----------------------------------------------------------------------------

void Bitmap::GetDimensions( int& w, int& h ) const
{
   if ( handle == 0 )
   {
      w = h = 0;
      return;
   }

   if ( (*API->Bitmap->GetBitmapDimensions)( handle, &w, &h ) == api_false )
      throw APIFunctionError( "GetBitmapDimensions" );
}

// ----------------------------------------------------------------------------

bool Bitmap::IsEmpty() const
{
   return handle == 0 || (*API->Bitmap->IsEmptyBitmap)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

RGBA Bitmap::Pixel( int x, int y ) const
{
   return (*API->Bitmap->GetBitmapPixel)( handle, x, y );
}

// ----------------------------------------------------------------------------

void Bitmap::SetPixel( int x, int y, RGBA v )
{
   EnsureUnique();
   (*API->Bitmap->SetBitmapPixel)( handle, x, y, v );
}

// ----------------------------------------------------------------------------

const RGBA* Bitmap::ScanLine( int i ) const
{
   return (*API->Bitmap->GetBitmapScanLine)( handle, i );
}

// ----------------------------------------------------------------------------

RGBA* Bitmap::ScanLine( int i )
{
   EnsureUnique();
   return (*API->Bitmap->GetBitmapScanLine)( handle, i );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Mirrored() const
{
   return Bitmap( (*API->Bitmap->MirroredBitmap)( handle, api_true, api_true ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::MirroredHorizontally() const
{
   return Bitmap( (*API->Bitmap->MirroredBitmap)( handle, api_true, api_false ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::MirroredVertically() const
{
   return Bitmap( (*API->Bitmap->MirroredBitmap)( handle, api_false, api_true ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Scaled( double sx, double sy, bool precise ) const
{
   int w, h; GetDimensions( w, h );
   w = pcl::RoundInt( sx*w ); h = pcl::RoundInt( sy*h );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Scaled( double sxy, bool precise ) const
{
   int w, h; GetDimensions( w, h );
   w = pcl::RoundInt( sxy*w ); h = pcl::RoundInt( sxy*h );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::ScaledToWidth( int w, bool precise ) const
{
   int dum, h; GetDimensions( dum, h );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::ScaledToHeight( int h, bool precise ) const
{
   int w, dum; GetDimensions( w, dum );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::ScaledToSize( int w, int h, bool precise ) const
{
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Rotated( double angleRadians, bool precise ) const
{
   return Bitmap( (*API->Bitmap->RotatedBitmap)( handle, angleRadians, precise ) );
}

// ----------------------------------------------------------------------------

void Bitmap::Load( const String& fileName )
{
   if ( !IsUnique() )
      SetHandle( (*API->Bitmap->CreateEmptyBitmap)( ModuleHandle() ) );
   if ( (*API->Bitmap->LoadBitmap)( handle, fileName.c_str() ) == api_false )
      throw APIFunctionError( "LoadBitmap" );
}

// ----------------------------------------------------------------------------

void Bitmap::Save( const String& fileName, int quality ) const
{
   if ( (*API->Bitmap->SaveBitmap)( handle, fileName.c_str(), quality ) == api_false )
      throw APIFunctionError( "SaveBitmap" );
}

// ----------------------------------------------------------------------------

void Bitmap::Load( const void* data, size_type size, const char* format, uint32 flags )
{
   if ( (*API->Bitmap->LoadBitmapData)( handle, data, size, format, flags ) == api_false )
      throw APIFunctionError( "LoadBitmapData" );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Subimage( int x0, int y0, int x1, int y1 ) const
{
   return Bitmap( (*API->Bitmap->CloneBitmapRect)( ModuleHandle(), handle, x0, y0, x1, y1 ) );
}

// ----------------------------------------------------------------------------

void Bitmap::Copy( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->CopyBitmap)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::Fill( const pcl::Rect& r, RGBA fillWith )
{
   EnsureUnique();
   (*API->Bitmap->FillBitmap)( handle, r.x0, r.y0, r.x1, r.y1, fillWith );
}

// ----------------------------------------------------------------------------

void Bitmap::Or( const pcl::Rect& r, RGBA orWith )
{
   EnsureUnique();
   (*API->Bitmap->OrBitmap)( handle, r.x0, r.y0, r.x1, r.y1, orWith );
}

// ----------------------------------------------------------------------------

void Bitmap::Or( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->OrBitmaps)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::And( const pcl::Rect& r, RGBA andWith )
{
   EnsureUnique();
   (*API->Bitmap->AndBitmap)( handle, r.x0, r.y0, r.x1, r.y1, andWith );
}

// ----------------------------------------------------------------------------

void Bitmap::And( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->AndBitmaps)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::Xor( const pcl::Rect& r, RGBA xorWith )
{
   EnsureUnique();
   (*API->Bitmap->XorBitmap)( handle, r.x0, r.y0, r.x1, r.y1, xorWith );
}

// ----------------------------------------------------------------------------

void Bitmap::Xor( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->XorBitmaps)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::XorRect( const pcl::Rect& r, RGBA xorWith )
{
   EnsureUnique();
   (*API->Bitmap->XorBitmapRect)( handle, r.x0, r.y0, r.x1, r.y1, xorWith );
}

// ----------------------------------------------------------------------------

void Bitmap::ReplaceColor( const pcl::Rect& r, RGBA replaceThis, RGBA replaceWith )
{
   EnsureUnique();
   (*API->Bitmap->ReplaceBitmapColor)( handle, r.x0, r.y0, r.x1, r.y1, replaceThis, replaceWith );
}

// ----------------------------------------------------------------------------

void Bitmap::SetAlpha( const pcl::Rect& r, uint8 alpha )
{
   EnsureUnique();
   (*API->Bitmap->SetBitmapAlpha)( handle, r.x0, r.y0, r.x1, r.y1, alpha );
}

// ----------------------------------------------------------------------------

void* Bitmap::CloneHandle() const
{
   return (*API->Bitmap->CloneBitmap)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

double Bitmap::PhysicalPixelRatio() const
{
   double f = 1.0;
   (*API->Bitmap->GetBitmapDevicePixelRatio)( handle, &f );
   return f;
}

// ----------------------------------------------------------------------------

void Bitmap::SetPhysicalPixelRatio( double ratio )
{
   EnsureUnique();
   (*API->Bitmap->SetBitmapDevicePixelRatio)( handle, Max( 1.0, ratio ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::FromSVGFile( const String& filePath, int width, int height, SVGRenderOptions options )
{
   return Bitmap( (*API->Bitmap->CreateBitmapFromSVGFile)( ModuleHandle(), filePath.c_str(), width, height, options ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::FromSVG( const IsoString& svgSource, int width, int height, SVGRenderOptions options )
{
   return Bitmap( (*API->Bitmap->CreateBitmapFromSVG)( ModuleHandle(), svgSource.c_str(), width, height, options ) );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Bitmap.cpp - Released 2025-04-07T08:53:32Z
