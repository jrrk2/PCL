//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Relational.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Relational_h
#define __PCL_Relational_h

/// \file pcl/Relational.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup generic_relational_operators Generic Relational Operators
 *
 * This section defines the template operator functions <em>not equal to</em>,
 * <em>less than or equal</em>, <em>greater than</em>, and <em>greater than or
 * equal</em>.
 *
 * For applicability of these generic operators, the following conditions are
 * assumed for the template argument types T1 and T2:
 *
 * \li For the != operator, a meaningful == operator exists for T1 and T2.
 *
 * \li For the <=, > and >= operators, a meaningful < operator exists for T1
 * and T2.
 */

/*!
 * Returns true iff two objects \a a and \a b are not equal.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
constexpr bool operator !=( const T1& a, const T2& b )
{
   return !(a == b);
}

/*!
 * Returns true iff an object \a a is less than or equal to another object \a b.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
constexpr bool operator <=( const T1& a, const T2& b )
{
   return !(b < a);
}

/*!
 * Returns true iff an object \a a is greater than another object \a b.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
constexpr bool operator >( const T1& a, const T2& b )
{
   return b < a;
}

/*!
 * Returns true iff an object \a a is greater than or equal to another object
 * \a b.
 * \ingroup generic_relational_operators
 */
template <class T1, class T2> inline
constexpr bool operator >=( const T1& a, const T2& b )
{
   return !(a < b);
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Relational_h

// ----------------------------------------------------------------------------
// EOF pcl/Relational.h - Released 2025-04-07T08:52:44Z
