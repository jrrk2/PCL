//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Selection.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Selection_h
#define __PCL_Selection_h

/// \file pcl/Selection.h

#include <pcl/Defs.h>

#include <pcl/Iterator.h>
#include <pcl/Utility.h>

// Template formal parameters:
//
// FI   Forward iterator
// BI   Bidirectional iterator
// RI   Random access iterator
// UP   Unary predicate
// BP   Binary predicate
// T    Item type
// F    Function

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup selection_algorithms Selection Algorithms
 */

// ----------------------------------------------------------------------------

template <class RI, typename T> inline
RI __pcl_quick_select__( RI i, RI j, distance_type k, T* )
{
   distance_type n = j - i;
   if ( k < 0 || n < 1 || n <= k )
      return j;

   for ( distance_type l = 0, r = n-1; ; )
   {
      RI x0 = i + l;
      RI y  = i + r;

      if ( r <= l+1 )
      {
         if ( r == l+1 && *y < *x0 )
            Swap( *x0, *y );
         return i + k;
      }

      RI x = x0;

      Swap( *++x, *(i + ((l+r) >> 1)) );

      if ( *y < *x0 )
         Swap( *x0, *y );
      if ( *y < *x )
         Swap( *x, *y );
      if ( *x < *x0 )
         Swap( *x, *x0 );

      T v = *x;

      for ( ;; )
      {
         while ( *++x < v );
         while ( v < *--y );
         if ( y < x )
            break;
         Swap( *x, *y );
      }

      *(x0+1) = *y;
      *y = v;

      distance_type dy = y - i;
      if ( dy >= k )
         r = dy - 1;
      if ( dy <= k )
         l = x - i;
   }
}

/*!
 * Generic quick selection algorithm.
 *
 * Given a range defined by two random access iterators [i,j) and a distance k,
 * such that j-i > k, returns an iterator t whose associated value is the k-th
 * element in the sorted sequence [i,j) in ascending order.
 *
 * For example, the following call:
 *
 * \code t = Select( i, j, (j - i) >> 1 ); \endcode
 *
 * will return the \e median of the distribution of elements in the [i,j)
 * sequence: the central value for which the probabilities that a greater and a
 * lesser element exist are equal.
 *
 * \b References
 *
 * \li William H. Press et al., <em>Numerical Recipes 3rd Edition: The Art of
 * Scientific Computing,</em> Cambridge University Press, 2007, Section 8.5.
 *
 * \li Robert Sedgewick, Kevin Wayne, <em>Algorithms, 4th Edition,</em>
 * Addison-Wesley Professional, 2011, pp. 345-347.
 *
 * \warning This function performs a <em>destructive selection search</em>:
 * There is no guarantee that the existing order of elements in the [i,j)
 * sequence is preserved after calling this function.
 *
 * \ingroup selection_algorithms
 */
template <class RI> inline
RI Select( RI i, RI j, distance_type k )
{
   return __pcl_quick_select__( i, j, k, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class RI, class BP, class T> inline
RI __pcl_quick_select__( RI i, RI j, distance_type k, BP p, T* )
{
   distance_type n = j - i;
   if ( k < 0 || n < 1 || n <= k )
      return j;

   for ( distance_type l = 0, r = n-1; ; )
   {
      RI x0 = i + l;
      RI y  = i + r;

      if ( r <= l+1 )
      {
         if ( r == l+1 && p( *y, *x0 ) )
            Swap( *x0, *y );
         return i + k;
      }

      RI x = x0;

      Swap( *++x, *(i + ((l+r) >> 1)) );

      if ( p( *y, *x0 ) )
         Swap( *x0, *y );
      if ( p( *y, *x ) )
         Swap( *x, *y );
      if ( p( *x, *x0 ) )
         Swap( *x, *x0 );

      T v = *x;

      for ( ;; )
      {
         while ( p( *++x, v ) );
         while ( p( v, *--y ) );
         if ( y < x )
            break;
         Swap( *x, *y );
      }

      *(x0+1) = *y;
      *y = v;

      distance_type dy = y - i;
      if ( dy >= k )
         r = dy - 1;
      if ( dy <= k )
         l = x - i;
   }
}

/*!
 * Generic quick selection algorithm.
 *
 * Given a range defined by two random access iterators [i,j) and a distance k,
 * such that j-i > k, returns an iterator t whose associated value is the k-th
 * element in the sorted sequence [i,j) in ascending order. Element comparison
 * is given by a binary predicate \a p such that p( a, b ) is true for any pair
 * a, b of elements such that a precedes b.
 *
 * See Select( RI, RI, distance_type ) for more information.
 *
 * \ingroup selection_algorithms
 */
template <class RI, class BP> inline
RI Select( RI i, RI j, distance_type k, BP p )
{
   return __pcl_quick_select__( i, j, k, p, ItemType( i ) );
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Selection_h

// ----------------------------------------------------------------------------
// EOF pcl/Selection.h - Released 2025-04-07T08:52:44Z
