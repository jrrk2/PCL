//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Memory.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Memory_h
#define __PCL_Memory_h

#include <pcl/Defs.h>

// Template formal parameters:
//
// FI   Forward iterator
// BI   Bidirectional iterator
// RI   Random access iterator
// UP   Unary predicate
// BP   Binary predicate
// T    Item type
// F    Function

// ### TODO: Document this file

namespace pcl
{

// ----------------------------------------------------------------------------

template <class FI, class T> inline
void Fill( FI i, FI j, const T& v )
{
   for ( ; i != j; ++i )
      *i = v;
}

// ----------------------------------------------------------------------------

template <class FI, class T, class F> inline
void Fill( FI i, FI j, const T& v, F f )
{
   for ( ; i != j; ++i )
      f( *i, v );
}

// ----------------------------------------------------------------------------

template <class FI1, class FI2> inline
FI1 Copy( FI1 i1, FI2 i2, FI2 j2 )
{
   for ( ; i2 != j2; ++i1, ++i2 )
      *i1 = *i2;
   return i1;
}

// ----------------------------------------------------------------------------

template <class FI1, class FI2, class F> inline
FI1 Copy( FI1 i1, FI2 i2, FI2 j2, F f )
{
   for ( ; i2 != j2; ++i1, ++i2 )
      f( *i1, *i2 );
   return i1;
}

// ----------------------------------------------------------------------------

template <class BI1, class BI2> inline
BI1 CopyBackward( BI1 j1, BI2 i2, BI2 j2 )
{
   while ( i2 != j2 )
      *--j1 = *--j2;
   return j1;
}

// ----------------------------------------------------------------------------

template <class BI1, class BI2, class F> inline
BI1 CopyBackward( BI1 j1, BI2 i2, BI2 j2, F f )
{
   while ( i2 != j2 )
      f( *--j1, *--j2 );
   return j1;
}

// ----------------------------------------------------------------------------

template <class BI1, class FI2> inline
BI1 CopyReversed( BI1 j1, FI2 i2, FI2 j2 )
{
   for ( ; i2 != j2; ++i2 )
      *--j1 = *i2;
   return j1;
}

// ----------------------------------------------------------------------------

template <class BI1, class FI2, class F> inline
BI1 CopyReversed( BI1 j1, FI2 i2, FI2 j2, F f )
{
   for ( ; i2 != j2; ++i2 )
      f( *--j1, *i2 );
   return j1;
}

// ----------------------------------------------------------------------------

template <class RI1, class RI2> inline
RI1 Move( RI1 i1, RI2 i2, RI2 j2 )
{
   RI1 j1;
   if ( i1 < i2 )
      j1 = Copy( i1, i2, j2 );
   else
   {
      j1 = i1 + (j2-i2);
      CopyBackward( j1, i2, j2 );
   }
   return j1;
}

// ----------------------------------------------------------------------------

template <class RI1, class RI2, class F> inline
RI1 Move( RI1 i1, RI2 i2, RI2 j2, F f )
{
   RI1 j1;
   if ( i1 < i2 )
      j1 = Copy( i1, i2, j2, f );
   else
   {
      j1 = i1 + (j2-i2);
      CopyBackward( j1, i2, j2, f );
   }
   return j1;
}

// ----------------------------------------------------------------------------

template <class RI1, class RI2> inline
RI1 MoveBackward( RI1 j1, RI2 i2, RI2 j2 )
{
   RI1 i1;
   if ( j2 < j1 )
      i1 = CopyBackward( j1, i2, j2 );
   else
   {
      i1 = j1 - (j2-i2);
      Copy( i1, i2, j2 );
   }
   return i1;
}

// ----------------------------------------------------------------------------

template <class RI1, class RI2, class F> inline
RI1 MoveBackward( RI1 j1, RI2 i2, RI2 j2, F f )
{
   RI1 i1;
   if ( j2 < j1 )
      i1 = CopyBackward( j1, i2, j2, f );
   else
   {
      i1 = j1 - (j2-i2);
      Copy( i1, i2, j2, f );
   }
   return i1;
}

// ----------------------------------------------------------------------------

template <class FI, class T1, class T2> inline
void Replace( FI i, FI j, const T1& v1, const T2& v2 )
{
   for ( ; i != j; ++i )
      if ( *i == v1 )
         *i = v2;
}

// ----------------------------------------------------------------------------

template <class FI, class T1, class T2, class BP> inline
void Replace( FI i, FI j, const T1& v1, const T2& v2, BP p )
{
   for ( ; i != j; ++i )
      if ( p( *i, v1 ) )
         *i = v2;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Memory_h

// ----------------------------------------------------------------------------
// EOF pcl/Memory.h - Released 2025-04-07T08:52:44Z
