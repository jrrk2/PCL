//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SHA1.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Cryptography.h>
#include <pcl/Exception.h>

extern "C" {
#include <RFC6234/sha.h>
}

namespace pcl
{

// ----------------------------------------------------------------------------

#define CTX reinterpret_cast<SHA1Context*>( m_context )

SHA1::~SHA1()
{
   if ( m_context != nullptr )
      delete CTX, m_context = nullptr;
}

void SHA1::Initialize()
{
   if ( m_context == nullptr )
      m_context = new SHA1Context;
   SHA1Reset( CTX );
}

void SHA1::Update( const void* data, size_type size )
{
   if ( data != nullptr )
      if ( size > 0 )
      {
         if ( m_context == nullptr )
         {
            m_context = new SHA1Context;
            SHA1Reset( CTX );
         }
         const uint8* bytes = reinterpret_cast<const uint8*>( data );
         do
         {
            size_type blockSize = Min( size_type( 0xfffffff0 ), size );
            SHA1Input( CTX, bytes, unsigned( blockSize ) );
            size -= blockSize;
            bytes += blockSize;
         }
         while ( size > 0 );
      }
}

void SHA1::DoFinalize( void* hash )
{
   if ( m_context == nullptr )
      throw Error( "SHA1::Finalize(): Invalid call on uninitialized object." );
   SHA1Result( CTX, static_cast<uint8_t*>( hash ) );
   SHA1Reset( CTX );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SHA1.cpp - Released 2025-04-07T08:53:32Z
