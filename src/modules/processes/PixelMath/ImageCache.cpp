//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard PixelMath Process Module Version 1.9.3
// ----------------------------------------------------------------------------
// ImageCache.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard PixelMath PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ImageCache.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ImageCache* TheImageCache = nullptr;

// ----------------------------------------------------------------------------

ImageCache::ImageCache()
{
   if ( TheImageCache == nullptr )
      TheImageCache = this;
}

ImageCache::~ImageCache()
{
   if ( this == TheImageCache )
      TheImageCache = nullptr;
}

// ----------------------------------------------------------------------------

void ImageCache::InvalidateImage( const IsoString& imageId )
{
   IsoString prefix = imageId + '#';

   /*
    * InsertionPoint() will find the first cache item whose identifier starts
    * with imageId, if at least one exists. This is because all cache items
    * have identifiers with a "#<params>" suffix.
    */
   SortedArray<CacheItem>::const_iterator i =
         InsertionPoint( m_cache.Begin(), m_cache.End(), CacheItem( prefix ) );
   if ( i != m_cache.End() )
      if ( i->key.StartsWith( prefix ) )
      {
         /*
          * Remove a contiguous sequence of cache items with the same image
          * identifier prefix.
          */
         SortedArray<CacheItem>::const_iterator j = i;
         while ( ++j != m_cache.End() )
            if ( !j->key.StartsWith( prefix ) )
               break;
         m_cache.Remove( i, j );
      }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageCache.cpp - Released 2025-04-07T08:53:56Z
