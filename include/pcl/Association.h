//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Association.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Association_h
#define __PCL_Association_h

/// \file pcl/Association.h

#include <pcl/Defs.h>

#include <pcl/Relational.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Association
 * \brief Generic association of two objects.
 */
template <typename T1, typename T2>
class PCL_CLASS Association
{
public:

   T1 first;   //!< First member of this association
   T2 second;  //!< Second member of this association

   /*!
    * Constructs an association with default-constructed member values.
    */
   Association() = default;

   /*!
    * Copy constructor.
    */
   Association( const Association<T1,T2>& ) = default;

   /*!
    * Move constructor.
    */
   Association( Association<T1,T2>&& ) = default;

   /*!
    * Constructs an association with the specified values \a x1 and \a x2,
    * respectively for the \a first and \a second members.
    */
   Association( const T1& x1, const T2& x2 )
      : first( x1 )
      , second( x2 )
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup association_utilities Association Operators and Utility Functions
 */

/*!
 * Returns an Association whose members are copies of the specified objects
 * \a x1 and \a x2.
 * \ingroup association_utilities
 */
template <typename T1, typename T2> inline
Association<T1,T2> Associate( const T1& x1, const T2& x2 )
{
   return Association<T1,T2>( x1, x2 );
}

/*!
 * Returns true iff two associations, \a x1 and \a x2, are equal. Two
 * associations are equal if their homolog members are equal.
 * \ingroup association_utilities
 */
template <typename T1, typename T2> inline
bool operator ==( const Association<T1,T2>& x1, const Association<T1,T2>& x2 )
{
   return x1.first == x2.first && x1.second == x2.second;
}

/*!
 * Returns true iff an association \a x1 is less than other association \a x2.
 * The comparison algorithm is as follows:
 *
 * \code
 * if ( x1.first == x2.first )
 *    return x1.second < x2.second;
 * else
 *    return x1.first < x2.first;
 * \endcode
 *
 * So in association comparisons the first member of each association has
 * precedence over the second member.
 *
 * The implementation of this operator only requires <em>less than</em>
 * semantics for the two types T1 and T2; it doesn't use equality operators.
 * \ingroup association_utilities
 */
template <typename T1, typename T2> inline
bool operator <( const Association<T1,T2>& x1, const Association<T1,T2>& x2 )
{
   //return (x1.first == x2.first) ? x1.second < x2.second : x1.first < x2.first;
   return (x1.first < x2.first) ? true : ((x2.first < x1.first) ? false : x1.second < x2.second);
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Association_h

// ----------------------------------------------------------------------------
// EOF pcl/Association.h - Released 2025-04-07T08:52:44Z
