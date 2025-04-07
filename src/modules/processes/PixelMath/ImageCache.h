//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// ImageCache.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ImageCache_h
#define __ImageCache_h

#include <pcl/ImageVariant.h>
#include <pcl/SortedArray.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageCache
{
public:

   ImageCache();
   virtual ~ImageCache();

   bool HasImage( const IsoString& key ) const
   {
      return m_cache.Contains( key );
   }

   ImageVariant Image( const IsoString& key ) const
   {
      SortedArray<CacheItem>::const_iterator i = m_cache.Search( key );
      if ( i != m_cache.End() )
         return i->image;
      return ImageVariant();
   }

   void AddImage( const IsoString& key, const ImageVariant& image )
   {
      m_cache << CacheItem( key, image );
   }

   void ClearImages()
   {
      m_cache.Clear();
   }

   bool ClearImages( size_type& count, size_type& size )
   {
      count = NumberOfImages();
      size = TotalImageSize();
      ClearImages();
      return count > 0;
   }

   bool IsEmpty() const
   {
      return m_cache.IsEmpty();
   }

   size_type NumberOfImages() const
   {
      return m_cache.Length();
   }

   size_type TotalImageSize() const
   {
      size_type sz = 0;
      for ( const CacheItem& item : m_cache )
         sz += item.image.ImageSize();
      return sz;
   }

   template <class S1, class S2>
   static IsoString MakeKey( const S1& imageId, const S2& keyData )
   {
      return IsoString( imageId ) + '#' + IsoString( keyData );
   }

   void InvalidateImage( const IsoString& imageId );

private:

   struct CacheItem
   {
      IsoString    key;
      ImageVariant image;

      CacheItem( const IsoString& k, const ImageVariant& i = ImageVariant() )
         : key( k )
         , image( i )
      {
      }

      CacheItem( const CacheItem& ) = default;

      bool operator ==( const CacheItem& x ) const
      {
         return key == x.key;
      }

      bool operator <( const CacheItem& x ) const
      {
         return key < x.key;
      }
   };

   SortedArray<CacheItem> m_cache;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ImageCache* TheImageCache;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageCache_h

// ----------------------------------------------------------------------------
// EOF ImageCache.h - Released 2025-04-07T08:53:56Z
