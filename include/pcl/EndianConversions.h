//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/EndianConversions.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_EndianConversions_h
#define __PCL_EndianConversions_h

/// \file pcl/EndianConversions.h

#include <pcl/Defs.h>

#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup endianness_conversion_and_detection Endianness Conversion and Detection Functions
 */

/*!
 * Converts a 16-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endianness_conversion_and_detection
 */
inline uint16 BigToLittleEndian( uint16 x )
{
   return (x << 8) | (x >> 8);
}

/*!
 * Converts a 32-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endianness_conversion_and_detection
 */
inline uint32 BigToLittleEndian( uint32 x )
{
   return (RotL( x, 8 ) & 0x00ff00ffu) | (RotR( x, 8 ) & 0xff00ff00u);

}

/*!
 * Converts a 64-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endianness_conversion_and_detection
 */
inline uint64 BigToLittleEndian( uint64 x )
{
   return (uint64( BigToLittleEndian( uint32( x ) ) ) << 32) | uint64( BigToLittleEndian( uint32( x >> 32 ) ) );
}

/*!
 * A convenience synonym function for little-to-big endian conversions, which
 * we define for the sake of code legibility. It is obviously equivalent to
 * BigToLittleEndian( x ).
 *
 * \ingroup endianness_conversion_and_detection
 */
template <typename T> inline T LittleToBigEndian( T x )
{
   return BigToLittleEndian( x );
}

// ----------------------------------------------------------------------------

union __pcl_endian_check__ { uint32 w; uint8 b[ sizeof( uint32 ) ]; };

constexpr __pcl_endian_check__ __pcl_endian_check_sample__ = { 0xdeadbeef };

#ifdef __clang__

inline bool IsLittleEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xef;
}

inline bool IsBigEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xde;
}

#else

// Clang fails here with "read of member 'b' of union with active member 'w' is
// not allowed in a constant expression".

/*!
 * Returns true iff the caller is running on a little-endian architecture.
 *
 * \ingroup endianness_conversion_and_detection
 */
constexpr bool IsLittleEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xef;
}

/*!
 * Returns true iff the caller is running on a big-endian architecture.
 *
 * \ingroup endianness_conversion_and_detection
 */
constexpr bool IsBigEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xde;
}

#endif // __clang__

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_EndianConversions_h

// ----------------------------------------------------------------------------
// EOF pcl/EndianConversions.h - Released 2025-04-07T08:52:44Z
