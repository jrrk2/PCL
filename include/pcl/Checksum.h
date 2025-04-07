//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Checksum.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Checksum_h
#define __PCL_Checksum_h

/// \file pcl/Checksum.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ByteArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup checksum_functions Checksum Calculation Functions
 */

/*!
 * Returns the <em>standard checksum</em> value calculated for a data sequence.
 * The returned value is the total number of bits set in the sequence of input
 * data bytes.
 *
 * \param data    Address of the first byte in the data sequence.
 * \param length  Length in bytes of the data sequence.
 *
 * \ingroup checksum_functions
 * \sa Checksum( const C& )
 */
size_type PCL_FUNC inline Checksum( const void* data, size_type length )
{
   PCL_PRECONDITION( data != nullptr )
   size_type S = 0;
   for ( const uint8* p = (const uint8*)data, * p1 = p+length; p < p1; ++p )
   {
      uint8 b = *p;
      if ( b & 0x01 ) ++S;
      if ( b & 0x02 ) ++S;
      if ( b & 0x04 ) ++S;
      if ( b & 0x08 ) ++S;
      if ( b & 0x10 ) ++S;
      if ( b & 0x20 ) ++S;
      if ( b & 0x40 ) ++S;
      if ( b & 0x80 ) ++S;
   }
   return S;
}

/*!
 * Returns the <em>standard checksum</em> value for a container.
 *
 * \param data    Reference to a container whose standard checksum will be
 *                calculated. The checksum number will be generated for the
 *                current data bytes in this container instance.
 *
 * \ingroup checksum_functions
 * \sa Checksum( const void*, size_type )
 */
template <class C> inline
uint32 Checksum( const C& data )
{
   return Checksum( data.Begin(), sizeof( *(data.Begin()) )*data.Length() );
}

/*!
 * Returns the CRC-32 error-detecting code calculated for a data sequence.
 *
 * \param data    Address of the first byte in the data sequence.
 * \param length  Length in bytes of the data sequence.
 *
 * \b References
 *
 * CRC calculation routine based on original code by Michael Barr. The employed
 * code requires the following copyright notice:
 *
 * <em>Copyright (c) 2000 by Michael Barr.  This software is placed into the
 * public domain and may be used for any purpose.  However, this notice must
 * not be changed or removed and no warranty is either expressed or implied by
 * its publication or distribution.</em>
 *
 * \ingroup checksum_functions
 * \sa CRC32( const C& )
 */
uint32 PCL_FUNC CRC32( const void* data, size_type length );

/*!
 * Returns the CRC-32 error-detecting code for a container.
 *
 * \param data    Reference to a container whose CRC-32 checksum will be
 *                calculated. The checksum code will be generated for the
 *                current data bytes in this container instance.
 *
 * \ingroup checksum_functions
 * \sa CRC32( const void*, size_type )
 */
template <class C> inline
uint32 CRC32( const C& data )
{
   return CRC32( data.Begin(), sizeof( *(data.Begin()) )*data.Length() );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Checksum_h

// ----------------------------------------------------------------------------
// EOF pcl/Checksum.h - Released 2025-04-07T08:52:44Z
