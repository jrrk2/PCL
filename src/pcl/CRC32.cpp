//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CRC32.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/Checksum.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
# pragma warning( disable: 4146 )   // unary minus operator applied to unsigned type, result still unsigned
#endif

uint32 CRC32( const void* data, size_type length )
{
   uint32 crc = 0xFFFFFFFF;
   if ( data != nullptr )
      for ( const uint8* p = reinterpret_cast<const uint8*>( data ); length > 0; --length, ++p )
      {
         crc ^= uint32( *p );
         for ( int j = 8; j > 0; --j )
            crc = (crc >> 1) ^ (0xEDB88320u & (-(crc & 1)));
      }
   return ~crc;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CRC32.cpp - Released 2025-04-07T08:53:32Z
