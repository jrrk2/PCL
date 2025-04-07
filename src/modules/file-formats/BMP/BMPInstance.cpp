//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// BMPInstance.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "BMPInstance.h"
#include "BMPFormat.h"

#include <pcl/Bitmap.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void GetBitmapInfo( int& width, int& height, bool& grayscale, bool& alpha, const Bitmap* bmp )
{
   bmp->GetDimensions( width, height );
   grayscale = true;
   alpha = false;

   for ( int i = 0; i < height; ++i )
   {
      const RGBA* p = bmp->ScanLine( i );
      const RGBA* p1 = p + width;
      for ( ;; )
      {
         RGBA rgba = *p;

         if ( !alpha && (rgba & 0xff000000) != 0xff000000 )
         {
            alpha = true;
            if ( !grayscale )
               return;
         }

         if ( grayscale )
         {
            uint8 red = Red( rgba );
            uint8 green = Green( rgba );
            uint8 blue = Blue( rgba );

            if ( red != green || red != blue || green != blue )
            {
               grayscale = false;
               if ( alpha )
                  return;
            }
         }

         if ( ++p == p1 )
            break;
      }
   }
}

ImageDescriptionArray BitmapInstance::Open( const String& filePath, const IsoString& /*hints*/ )
{
   Close();

   m_bitmap = new Bitmap( m_path = filePath );

   if ( m_bitmap->IsNull() || m_bitmap->IsEmpty() )
   {
      Close();
      throw Error( "Error reading bitmap" );
   }

   bool grayscale, alpha;
   GetBitmapInfo( m_info.width, m_info.height, grayscale, alpha, m_bitmap.Ptr() );

   m_info.numberOfChannels = (grayscale ? 1 : 3) + (alpha ? 1 : 0);
   m_info.colorSpace = grayscale ? ColorSpace::Gray : ColorSpace::RGB;
   m_info.supported = true;

   ImageDescription d;

   d.info = m_info;

   d.options.bitsPerSample = 8;
   d.options.embedICCProfile= false;
   d.options.embedThumbnail = false;

   ImageDescriptionArray a;
   a.Add( d );
   return a;
}

// ----------------------------------------------------------------------------

bool BitmapInstance::IsOpen() const
{
   return !m_bitmap.IsNull();
}

// ----------------------------------------------------------------------------

String BitmapInstance::FilePath() const
{
   return m_path;
}

// ----------------------------------------------------------------------------

void BitmapInstance::Create( const String& filePath, int /*numberOfImages*/, const IsoString& hints )
{
   Close();

   m_path = filePath;
   m_hints = hints;

   m_bitmap = new Bitmap; // this is just to "look like open"; we'll create a true bitmap upon image write
   if ( m_bitmap->IsNull() )
   {
      Close();
      throw Error( "Unable to create bitmap" );
   }
}

// ----------------------------------------------------------------------------

void BitmapInstance::Close()
{
   m_bitmap.Destroy();
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadImageImp( GenericImage<P>& image, const AutoPointer<Bitmap>& bitmap, const ImageInfo& info )
{
   if ( bitmap.IsNull() || bitmap->IsNull() )
      throw Error( "Attempt to read a bitmap before opening a file" );

   image.AllocateData( info.width, info.height, info.numberOfChannels, ColorSpace::value_type( info.colorSpace ) );
   int n = image.NumberOfNominalChannels();

   for ( int y = 0; y < image.Height(); ++y )
   {
      const RGBA* b = bitmap->ScanLine( y );

      typename P::sample* p[ 4 ];
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
         p[c] = image.ScanLine( y, c );

      for ( int j = 0; j < image.Width(); ++j, ++b )
      {
         for ( int c = 0, s = 16; c < n; ++c, s -= 8 )
            UInt8PixelTraits::FromSample( *p[c]++, uint8( *b >> s ) );
         if ( image.HasAlphaChannels() )
            UInt8PixelTraits::FromSample( *p[n]++, uint8( *b >> 24 ) );
      }
   }
}

void BitmapInstance::ReadImage( Image& image )
{
   ReadImageImp( image, m_bitmap, m_info );
}

void BitmapInstance::ReadImage( DImage& image )
{
   ReadImageImp( image, m_bitmap, m_info );
}

void BitmapInstance::ReadImage( UInt8Image& image )
{
   ReadImageImp( image, m_bitmap, m_info );
}

void BitmapInstance::ReadImage( UInt16Image& image )
{
   ReadImageImp( image, m_bitmap, m_info );
}

void BitmapInstance::ReadImage( UInt32Image& image )
{
   ReadImageImp( image, m_bitmap, m_info );
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteImageImp( const String& path, AutoPointer<Bitmap>& bitmap, const GenericImage<P>& image, int quality, const IsoString& hints )
{
   if ( path.IsEmpty() || bitmap.IsNull() )
      throw Error( "Attempt to write a bitmap before creating a file" );

   if ( !hints.IsEmpty() )
   {
      IsoStringList tokens;
      hints.Break( tokens, ' ', true/*trim*/ );
      tokens.Remove( IsoString() );
      for ( IsoStringList::const_iterator i = tokens.Begin(); i != tokens.End(); ++i )
         if ( *i == "quality" )
         {
            if ( ++i == tokens.End() )
               break;
            if ( i->TryToInt( quality ) )
               quality = Range( quality, 0, 100 );
         }
   }

   bitmap.Destroy();
   ImageVariant v( &const_cast<GenericImage<P>&>( image ) );
   bitmap = new Bitmap( Bitmap::Render( v ) );
   bitmap->Save( path, quality );
}

void BitmapInstance::WriteImage( const Image& image )
{
   WriteImageImp( m_path, m_bitmap, image, DefaultQuality(), m_hints );
}

void BitmapInstance::WriteImage( const DImage& image )
{
   WriteImageImp( m_path, m_bitmap, image, DefaultQuality(), m_hints );
}

void BitmapInstance::WriteImage( const UInt8Image& image )
{
   WriteImageImp( m_path, m_bitmap, image, DefaultQuality(), m_hints );
}

void BitmapInstance::WriteImage( const UInt16Image& image )
{
   WriteImageImp( m_path, m_bitmap, image, DefaultQuality(), m_hints );
}

void BitmapInstance::WriteImage( const UInt32Image& image )
{
   WriteImageImp( m_path, m_bitmap, image, DefaultQuality(), m_hints );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BMPInstance.cpp - Released 2025-04-07T08:53:45Z
