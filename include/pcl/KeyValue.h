//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/KeyValue.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_KeyValue_h
#define __PCL_KeyValue_h

/// \file pcl/KeyValue.h

#include <pcl/Defs.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class KeyValue
 * \brief Generic key-value association.
 */
template <typename T1, typename T2>
class PCL_CLASS KeyValue
{
public:

   T1 key;     //!< The key member of this key-value association
   T2 value;   //!< The value member of this key-value association

   /*!
    * Constructs a key-value association with default key and value members.
    */
   KeyValue()
      : key()
      , value()
   {
   }

   /*!
    * Constructs a key-value association with the specified key and value.
    */
   KeyValue( const T1& k, const T2& v )
      : key( k )
      , value( v )
   {
   }

   /*!
    * Constructs a key-value association with the specified key and a
    * default-constructed value.
    */
   KeyValue( const T1& k )
      : key( k )
      , value()
   {
   }

   /*!
    * Copy constructor.
    */
   KeyValue( const KeyValue& ) = default;

   /*!
    * Move constructor.
    */
   KeyValue( KeyValue&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   KeyValue& operator =( const KeyValue& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   KeyValue& operator =( KeyValue&& ) = default;

   /*!
    * Returns true iff this key-value association is equal to another object
    * \a x. key-value associations are equal if their respective key and value
    * members are equal.
    */
   bool operator ==( const KeyValue& x ) const
   {
      return key == x.key && value == x.value;
   }

   /*!
    * Returns true iff this key-value association precedes another object \a x.
    * In key-value comparisons, key members have precedence over value members.
    * The comparison algorithm is as follows:
    *
    * \code
    * if ( this->key != x.key )
    *    return this->key < x.key;
    * else
    *    return this->value < x.value;
    * \endcode
    *
    * The implementation of this operator only requires <em>less than</em>
    * semantics for the two types T1 and T2; it doesn't use equality operators.
    */
   bool operator <( const KeyValue& x ) const
   {
      return (key < x.key) ? true : ((x.key < key) ? false : value < x.value);
   }
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_key_value_associations String Key-Value Associations
 */

/*!
 * \class pcl::StringKeyValue
 * \ingroup string_key_value_associations
 * \brief Key-value association of two Unicode (UTF-16) strings.
 */
using StringKeyValue = KeyValue<String, String>;

/*!
 * \class pcl::IsoStringKeyValue
 * \ingroup string_key_value_associations
 * \brief Key-value association of two 8-bit strings.
 */
using IsoStringKeyValue = KeyValue<IsoString, IsoString>;

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_KeyValue_h

// ----------------------------------------------------------------------------
// EOF pcl/KeyValue.h - Released 2025-04-07T08:52:44Z
