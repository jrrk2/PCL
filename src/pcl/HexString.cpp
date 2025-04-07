//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/HexString.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/String.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IsoString IsoString::ToHex( const void* data, size_type length )
{
   IsoString hex;
   hex.SetLength( length << 1 );
   iterator h = hex.Begin();
   for ( const uint8* p = reinterpret_cast<const uint8*>( data ); length > 0; ++p, --length )
   {
      // MSN
      int n = (*p >> 4) & 0x0f;
      if ( n < 10 )
         *h = char( n + '0' );
      else
         *h = char( n - 10 + 'a' );
      ++h;

      // LSN
      n = *p & 0x0f;
      if ( n < 10 )
         *h = char( n + '0' );
      else
         *h = char( n - 10 + 'a' );
      ++h;
   }
   return hex;
}

// ----------------------------------------------------------------------------

inline static
int HexDigitValue( char c )
{
   switch ( c )
   {
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      return c - '0';
   case 'a':
   case 'b':
   case 'c':
   case 'd':
   case 'e':
   case 'f':
      return 10 + c - 'a';
   case 'A':
   case 'B':
   case 'C':
   case 'D':
   case 'E':
   case 'F':
      return 10 + c - 'A';
   default:
      throw ParseError( String().Format( "Invalid hexadecimal digit %%%02x", int( c ) ) );
   }
}

ByteArray IsoString::FromHex() const
{
   size_type len = Length();
   if ( len & 1 )
      throw ParseError( "Invalid hex-encoded string length" );

   ByteArray B( len >> 1 );
   ByteArray::iterator b = B.Begin();
   for ( const_iterator i = m_data->string; i < m_data->end; )
   {
      int msb = HexDigitValue( *i++ ) << 4;
      int lsb = HexDigitValue( *i++ );
      *b++ = uint8( msb | lsb );
   }
   return B;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/HexString.cpp - Released 2025-04-07T08:53:32Z
