//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Search.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Search_h
#define __PCL_Search_h

/// \file pcl/Search.h

#include <pcl/Defs.h>

#include <pcl/Iterator.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup search_algorithms Search Algorithms
 *
 * Template formal parameters:
 *
 * FI   Forward iterator \n
 * BI   Bidirectional iterator \n
 * RI   Random access iterator \n
 * UP   Unary predicate \n
 * BP   Binary predicate \n
 * T    Item type \n
 * F    Function
 */

// ----------------------------------------------------------------------------

/*!
 * Generic linear search algorithm.
 *
 * Returns the first iterator k in the range [i,j) such that *k == v. Returns
 * j if no such occurrence of v is found.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T> inline
FI LinearSearch( FI i, FI j, const T& v ) noexcept
{
   for( ; i != j; ++i )
      if ( *i == v )
         break;
   return i;
}

// ----------------------------------------------------------------------------

/*!
 * Generic linear search algorithm.
 *
 * Returns the first iterator k in the range [i,j) such that p( *k, v ) is
 * true. Returns j if no such occurrence of v is found.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T, class BP> inline
FI LinearSearch( FI i, FI j, const T& v, BP p ) noexcept( noexcept( p ) )
{
   for( ; i != j; ++i )
      if ( p( *i, v ) )
         break;
   return i;
}

// ----------------------------------------------------------------------------

/*!
 * Generic linear search algorithm (last occurrence).
 *
 * Returns the last iterator k in the range [i,j) such that *k == v. Returns
 * j if no such occurrence of v is found.
 *
 * \ingroup search_algorithms
 */
template <class BI, class T> inline
BI LinearSearchLast( BI i, BI j, const T& v ) noexcept
{
   for( BI k = j; i != k; )
      if ( *--k == v )
         return k;
   return j;
}

// ----------------------------------------------------------------------------

/*!
 * Generic linear search algorithm (last occurrence).
 *
 * Returns the last iterator k in the range [i,j) such that *k == v. Returns
 * j if no such occurrence of v is found.
 *
 * \ingroup search_algorithms
 */
template <class BI, class T, class BP> inline
BI LinearSearchLast( BI i, BI j, const T& v, BP p ) noexcept( noexcept( p ) )
{
   for( BI k = j; i != k; )
      if ( p( *--k, v ) )
         return k;
   return j;
}

// ----------------------------------------------------------------------------

/*!
 * Generic binary search algorithm.
 *
 * Returns the first iterator k in the range [i,j) such that *k == v. Returns j
 * if no such occurrence of v is found.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, *j < *i must be false.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T> inline
FI BinarySearch( FI i, FI j, const T& v ) noexcept
{
   for ( distance_type n = Distance( i, j ); n > 0; )
   {
      distance_type h = n >> 1;
      FI m = i;
      Advance( m, h );

      if ( *m < v )
      {
         i = ++m;
         n -= h+1;
      }
      else
         n = h;
   }

   return (i != j && !(v < *i)) ? i : j;
}

// ----------------------------------------------------------------------------

/*!
 * Generic binary search algorithm.
 *
 * Returns the first iterator k in the range [i,j) such that both p( *k, v )
 * and p( v, *k ) are false. Returns j if no such occurrence of v is found.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, p( *j, *i ) must be false.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T, class BP> inline
FI BinarySearch( FI i, FI j, const T& v, BP p ) noexcept( noexcept( p ) )
{
   for ( distance_type n = Distance( i, j ); n > 0; )
   {
      distance_type h = n >> 1;
      FI m = i;
      Advance( m, h );

      if ( p( *m, v ) )
      {
         i = ++m;
         n -= h+1;
      }
      else
         n = h;
   }

   return (i != j && !p( v, *i )) ? i : j;
}

// ----------------------------------------------------------------------------

/*!
 * Generic binary search algorithm (last occurrence).
 *
 * Returns the last iterator k in the range [i,j) such that *k == v. Returns j
 * if no such occurrence of v is found.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, *j < *i must be false.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T> inline
FI BinarySearchLast( FI i, FI j, const T& v ) noexcept
{
   FI k = BinarySearch( i, j, v );
   if ( k != j )
      for( FI l = k; ++l != j && *l == v; )
         ++k;
   return k;
}

// ----------------------------------------------------------------------------

/*!
 * Generic binary search algorithm (last occurrence).
 *
 * Returns the last iterator k in the range [i,j) such that both p( *k, v )
 * and p( v, *k ) are false. Returns j if no such occurrence of v is found.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, p( *j, *i ) must be false.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T, class BP> inline
FI BinarySearchLast( FI i, FI j, const T& v, BP p ) noexcept( noexcept( p ) )
{
   FI k = BinarySearch( i, j, v, p );
   if ( k != j )
      for( FI l = k; ++l != j && !p( *l, v ) && !p( v, *l ); )
         ++k;
   return k;
}

// ----------------------------------------------------------------------------

/*!
 * Generic binary search algorithm.
 *
 * Returns the first iterator k in the range [i,j) such that both p1( *k, v )
 * and p2( v, *k ) are false. Returns j if no such occurrence of v is found.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, both p1( *j, *i ) and p2( *j, *i ) must be
 * false.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T, class BP1, class BP2> inline
FI BinarySearch( FI i, FI j, const T& v, BP1 p1, BP2 p2 ) noexcept( noexcept( p1 ) && noexcept( p2 ) )
{
   for ( distance_type n = Distance( i, j ); n > 0; )
   {
      distance_type h = n >> 1;
      FI m = i;
      Advance( m, h );

      if ( p1( *m, v ) )
      {
         i = ++m;
         n -= h+1;
      }
      else
         n = h;
   }

   return (i != j && !p2( v, *i )) ? i : j;
}

// ----------------------------------------------------------------------------

/*!
 * Generic insertion point algorithm.
 *
 * Returns an iterator k in the range [i,j) such that:
 *
 * \li for all a in [i,j) that precedes k, v \< *a is false.
 *
 * \li for all b in [i,j) that postcedes k, *b \< v is false.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, *j < *i must be false.
 *
 * The resulting iterator k corresponds to a location where the value \a v could
 * be inserted in the [i,j) sequence without breaking the existing order of
 * values, as defined by operator <.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T> inline
FI InsertionPoint( FI i, FI j, const T& v ) noexcept
{
   for ( distance_type n = Distance( i, j ); n > 0; )
   {
      distance_type h = n >> 1;
      FI m = i;
      Advance( m, h );

      if ( v < *m )
         n = h;
      else
      {
         i = ++m;
         n -= h+1;
      }
   }

   return i;
}

// ----------------------------------------------------------------------------

/*!
 * Generic insertion point algorithm.
 *
 * Returns an iterator k in the range [i,j) such that:
 *
 * \li for all a in [i,j) that precedes k, p( v, *a ) is false.
 *
 * \li for all b in [i,j) that postcedes k, p( *b, v ) is false.
 *
 * The sequence [i,j) must be ordered in ascending order: for every pair a,b in
 * [i,j) such that i precedes j, p( *j, *i ) must be false.
 *
 * The resulting iterator k corresponds to a location where the value \a v could
 * be inserted in the [i,j) sequence without breaking the existing order of
 * values, as defined by the binary predicate p.
 *
 * \ingroup search_algorithms
 */
template <class FI, class T, class BP> inline
FI InsertionPoint( FI i, FI j, const T& v, BP p ) noexcept( noexcept( p ) )
{
   for ( distance_type n = Distance( i, j ); n > 0; )
   {
      distance_type h = n >> 1;
      FI m = i;
      Advance( m, h );

      if ( p( v, *m ) )
         n = h;
      else
      {
         i = ++m;
         n -= h+1;
      }
   }

   return i;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the starting iterator of the first subsequence within the range
 * [i1,j1) that is identical to the sequence [i2,j2) when compared element by
 * element, or j1 if no such subsequence is found.
 *
 * \ingroup search_algorithms
 */
template <class FI1, class FI2> inline
FI1 Search( FI1 i1, FI1 j1, FI2 i2, FI2 j2 ) noexcept
{
   distance_type n1 = Distance( i1, j1 );
   distance_type n2 = Distance( i2, j2 );
   if ( n1 > 0 && n2 > 0 )
      for ( ; n2 <= n1; ++i1, --n1 )
      {
         FI1 k1 = i1;
         FI2 k2 = i2;
         for ( ; *k1 == *k2; ++k1 )
            if ( ++k2 == j2 )
               return i1;
      }
   return j1;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the starting iterator of the first subsequence within the range
 * [i1,j1) that corresponds to the sequence [i2,j2) when the binary predicate p
 * is applied element by element, or j1 if no such subsequence is found.
 *
 * \ingroup search_algorithms
 */
template <class FI1, class FI2, class BP> inline
FI1 Search( FI1 i1, FI1 j1, FI2 i2, FI2 j2, BP p ) noexcept( noexcept( p ) )
{
   distance_type n1 = Distance( i1, j1 );
   distance_type n2 = Distance( i2, j2 );
   if ( n1 > 0 && n2 > 0 )
      for ( ; n2 <= n1; ++i1, --n1 )
      {
         FI1 k1 = i1;
         FI2 k2 = i2;
         for ( ; p( *k1, *k2 ); ++k1 )
            if ( ++k2 == j2 )
               return i1;
      }
   return j1;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the starting iterator of the last subsequence within the range
 * [i1,j1) that is identical to the sequence [i2,j2) when compared element by
 * element, or j1 if no such subsequence is found.
 *
 * \ingroup search_algorithms
 */
template <class BI1, class FI2> inline
BI1 SearchLast( BI1 i1, BI1 j1, FI2 i2, FI2 j2 ) noexcept
{
   distance_type n1 = Distance( i1, j1 );
   distance_type n2 = Distance( i2, j2 );
   if ( n1 > 0 && n2 > 0 && n2 <= n1 )
   {
      i1.Advance( n1 - n2 );
      for ( ;; )
      {
         BI1 k1 = i1;
         FI2 k2 = i2;
         for ( ; *k1 == *k2; ++k1 )
            if ( ++k2 == j2 )
               return i1;
         if ( --n1 < n2 )
            break;
         --i1;
      }
   }
   return j1;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the starting iterator of the last subsequence within the range
 * [i1,j1) that corresponds to the sequence [i2,j2) when the binary predicate p
 * is applied element by element, or j1 if no such subsequence is found.
 *
 * \ingroup search_algorithms
 */
template <class BI1, class FI2, class BP> inline
BI1 SearchLast( BI1 i1, BI1 j1, FI2 i2, FI2 j2, BP p ) noexcept( noexcept( p ) )
{
   distance_type n1 = Distance( i1, j1 );
   distance_type n2 = Distance( i2, j2 );
   if ( n1 > 0 && n2 > 0 && n2 <= n1 )
   {
      i1.Advance( n1 - n2 );
      for ( ;; )
      {
         BI1 k1 = i1;
         FI2 k2 = i2;
         for ( ; p( *k1, *k2 ); ++k1 )
            if ( ++k2 == j2 )
               return i1;
         if ( --n1 < n2 )
            break;
         --i1;
      }
   }
   return j1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Search_h

// ----------------------------------------------------------------------------
// EOF pcl/Search.h - Released 2025-04-07T08:52:44Z
