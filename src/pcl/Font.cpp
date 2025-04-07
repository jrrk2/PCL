//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Font.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/Font.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Font::Font( family f, double ptSize )
   : UIObject( (*API->Font->CreateFontByFamily)( ModuleHandle(), f, ptSize ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFontByFamily" );
}

Font::Font( const String& face, double ptSize )
   : UIObject( (*API->Font->CreateFontByFace)( ModuleHandle(), face.c_str(), ptSize ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFontByFace" );
}

// ----------------------------------------------------------------------------

Font& Font::Null()
{
   static Font* nullFont = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullFont == nullptr )
      nullFont = new Font( nullptr );
   return *nullFont;
}

// ----------------------------------------------------------------------------

String Font::Face() const
{
   if ( IsNull() )
      return String();

   size_type len = 0;
   (*API->Font->GetFontFace)( handle, 0, &len );
   if ( len == 0 )
      throw APIFunctionError( "GetFontFace" );

   String face;
   face.SetLength( len );
   if ( (*API->Font->GetFontFace)( handle, face.Begin(), &len ) == api_false )
      throw APIFunctionError( "GetFontFace" );
   face.ResizeToNullTerminated();
   return face;
}

// ----------------------------------------------------------------------------

void Font::SetFace( const String& face )
{
   EnsureUnique();
   (*API->Font->SetFontFace)( handle, face.c_str() );
}

// ----------------------------------------------------------------------------

bool Font::IsExactMatch() const
{
   return (*API->Font->GetFontExactMatch)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int Font::PixelSize() const
{
   return (*API->Font->GetFontPixelSize)( handle );
}

// ----------------------------------------------------------------------------

void Font::SetPixelSize ( int pxSize )
{
   EnsureUnique();
   (*API->Font->SetFontPixelSize)( handle, pxSize );
}

// ----------------------------------------------------------------------------

double Font::PointSize() const
{
   double ptSize = 0.0;
   (*API->Font->GetFontPointSize)( handle, &ptSize );
   return ptSize;
}

// ----------------------------------------------------------------------------

void Font::SetPointSize( double ptSize )
{
   EnsureUnique();
   (*API->Font->SetFontPointSize)( handle, ptSize );
}

// ----------------------------------------------------------------------------

bool Font::IsFixedPitch() const
{
   return (*API->Font->GetFontFixedPitch)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetFixedPitch( bool enable )
{
   EnsureUnique();
   (*API->Font->SetFontFixedPitch)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsKerningEnabled() const
{
   return (*API->Font->GetFontKerning)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::EnableKerning( bool enable )
{
   EnsureUnique();
   (*API->Font->SetFontKerning)( handle, enable );
}

// ----------------------------------------------------------------------------

void Font::DisableKerning( bool disable )
{
   EnsureUnique();
   (*API->Font->SetFontKerning)( handle, !disable );
}

// ----------------------------------------------------------------------------

int Font::StretchFactor() const
{
   return (*API->Font->GetFontStretchFactor)( handle );
}

// ----------------------------------------------------------------------------

void Font::SetStretchFactor( int stretch )
{
   EnsureUnique();
   (*API->Font->SetFontStretchFactor)( handle, stretch );
}

// ----------------------------------------------------------------------------

int Font::Weight() const
{
   return (*API->Font->GetFontWeight)( handle );
}

// ----------------------------------------------------------------------------

void Font::SetWeight( int weight )
{
   EnsureUnique();
   (*API->Font->SetFontWeight)( handle, weight );
}

// ----------------------------------------------------------------------------

bool Font::IsItalic() const
{
   return (*API->Font->GetFontItalic)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetItalic( bool enable )
{
   EnsureUnique();
   (*API->Font->SetFontItalic)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsUnderline() const
{
   return (*API->Font->GetFontUnderline)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetUnderline( bool enable )
{
   EnsureUnique();
   (*API->Font->SetFontUnderline)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsOverline() const
{
   return (*API->Font->GetFontOverline)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetOverline( bool enable )
{
   EnsureUnique();
   (*API->Font->SetFontOverline)( handle, enable );
}

// ----------------------------------------------------------------------------

bool Font::IsStrikeOut() const
{
   return (*API->Font->GetFontStrikeOut)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Font::SetStrikeOut( bool enable )
{
   EnsureUnique();
   (*API->Font->SetFontStrikeOut)( handle, enable );
}

// ----------------------------------------------------------------------------

int Font::Ascent() const
{
   return (*API->Font->GetFontAscent)( handle );
}

// ----------------------------------------------------------------------------

int Font::Descent() const
{
   return (*API->Font->GetFontDescent)( handle );
}

// ----------------------------------------------------------------------------

int Font::Height() const
{
   return (*API->Font->GetFontHeight)( handle );
}

// ----------------------------------------------------------------------------

int Font::LineSpacing() const
{
   return (*API->Font->GetFontLineSpacing)( handle );
}

// ----------------------------------------------------------------------------

bool Font::IsCharDefined( int c ) const
{
   return (*API->Font->GetFontCharDefined)( handle, c ) != api_false;
}

// ----------------------------------------------------------------------------

int Font::MaxWidth() const
{
   return (*API->Font->GetFontMaxWidth)( handle );
}

// ----------------------------------------------------------------------------

int Font::Width( const String& s ) const
{
   return (*API->Font->GetStringPixelWidth)( handle, s.c_str() );
}

// ----------------------------------------------------------------------------

int Font::Width( int c ) const
{
   return (*API->Font->GetCharPixelWidth)( handle, c );
}

// ----------------------------------------------------------------------------

Rect Font::BoundingRect( const String& s ) const
{
   Rect r;
   (*API->Font->GetStringPixelRect)( handle, s.c_str(), &r.x0, &r.y0, &r.x1, &r.y1, 0x00 );
   return r;
}

// ----------------------------------------------------------------------------

Rect Font::TightBoundingRect( const String& s ) const
{
   Rect r;
   (*API->Font->GetStringPixelRect)( handle, s.c_str(), &r.x0, &r.y0, &r.x1, &r.y1, 0x01 );
   return r;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class InternalFontEnumerator
{
public:

   static api_bool api_func FontCallback( const char16_type* item, void* ptrToStringList )
   {
      reinterpret_cast<StringList*>( ptrToStringList )->Add( String( item ) );
      return api_true;
   }

   static api_bool api_func PtSizeCallback( const double* ptSize, void* ptrToArray )
   {
      reinterpret_cast<Array<double>*>( ptrToArray )->Add( *ptSize );
      return api_true;
   }
};

// ----------------------------------------------------------------------------

StringList Font::AvailableFonts( const String& writingSystem )
{
   StringList faces;

   IsoString ws8( writingSystem );

   size_type len = 0;
   (*API->Font->EnumerateFonts)( 0, 0, &len, 0, ws8.c_str() );

   if ( len > 0 )
   {
      String fontFace;
      fontFace.Reserve( len );
      if ( (*API->Font->EnumerateFonts)( InternalFontEnumerator::FontCallback,
                        fontFace.Begin(), &len, &faces, ws8.c_str() ) == api_false )
      {
         throw APIFunctionError( "EnumerateFonts" );
      }
   }

   return faces;
}

// ----------------------------------------------------------------------------

StringList Font::AvailableFontWritingSystems( const String& font )
{
   StringList writingSystems;

   size_type len = 0;
   (*API->Font->EnumerateWritingSystems)( 0, 0, &len, 0, font.c_str() );

   if ( len > 0 )
   {
      String system;
      system.Reserve( len );
      if ( (*API->Font->EnumerateWritingSystems)( InternalFontEnumerator::FontCallback,
                        system.Begin(), &len, &writingSystems, font.c_str() ) == api_false )
      {
         throw APIFunctionError( "EnumerateWritingSystems" );
      }
   }

   return writingSystems;
}

// ----------------------------------------------------------------------------

StringList Font::AvailableFontStyles( const String& font )
{
   StringList styles;

   size_type len = 0;
   (*API->Font->EnumerateFontStyles)( 0, 0, &len, 0, font.c_str() );

   if ( len > 0 )
   {
      String style;
      style.Reserve( len );
      if ( (*API->Font->EnumerateFontStyles)( InternalFontEnumerator::FontCallback,
                        style.Begin(), &len, &styles, font.c_str() ) == api_false )
      {
         throw APIFunctionError( "EnumerateFontStyles" );
      }
   }

   return styles;
}

// ----------------------------------------------------------------------------

Array<double> Font::OptimalFontPointSizes( const String& font, const String& style )
{
   Array<double> optimalSizes;
   double ptSize;
   if ( (*API->Font->EnumerateOptimalFontPointSizes)( InternalFontEnumerator::PtSizeCallback,
                                 &ptSize, &optimalSizes, font.c_str(), style.c_str() ) == api_false )
   {
      throw APIFunctionError( "EnumerateOptimalFontPointSizes" );
   }
   return optimalSizes;
}

// ----------------------------------------------------------------------------

bool Font::IsScalableFont( const String& font, const String& style )
{
   return (*API->Font->GetFontScalable)( font.c_str(), style.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool Font::IsFixedPitchFont( const String& font, const String& style )
{
   return (*API->Font->GetNominalFontFixedPitch)( font.c_str(), style.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool Font::IsItalicFont( const String& font, const String& style )
{
   return (*API->Font->GetNominalFontItalic)( font.c_str(), style.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

int Font::FontWeight( const String& font, const String& style )
{
   return (*API->Font->GetNominalFontWeight)( font.c_str(), style.c_str() );
}

// ----------------------------------------------------------------------------

void* Font::CloneHandle() const
{
   return (*API->Font->CloneFont)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

namespace FontFamily
{

String PCL_FUNC FamilyToFace( value_type family )
{
   const char* face;

   switch ( family )
   {
   default:
   case pcl::FontFamily::Default:
#ifdef __PCL_X11
      face = "Sans";
#endif
#ifdef __PCL_MACOSX
      face = "Lucida Grande";
#endif
#ifdef __PCL_WINDOWS
      face = "MS Sans Serif";
#endif
      break;

   case pcl::FontFamily::SansSerif:
#ifdef __PCL_X11
      face = "Sans";
#endif
#ifdef __PCL_MACOSX
      face = "Lucida Grande";
#endif
#ifdef __PCL_WINDOWS
      face = "Arial";
#endif
      break;

   case pcl::FontFamily::Serif:
#ifdef __PCL_X11
      face = "Serif";
#endif
#ifdef __PCL_MACOSX
      face = "Times";
#endif
#ifdef __PCL_WINDOWS
      face = "Times New Roman";
#endif
      break;

   case pcl::FontFamily::Script:
      face = "Script";
      break;

   case pcl::FontFamily::TypeWriter:
#ifdef __PCL_X11
      face = "Monospace";
#endif
#ifdef __PCL_MACOSX
      face = "Courier";
#endif
#ifdef __PCL_WINDOWS
      face = "Courier New";
#endif
      break;

   case pcl::FontFamily::Decorative:
      face = "Bookman Old Style";
      break;

   case pcl::FontFamily::Symbol:
      face = "Symbol";
      break;
   }

   return face;
}

} // FontFamily

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Font.cpp - Released 2025-04-07T08:53:32Z
