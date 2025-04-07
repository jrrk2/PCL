//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Property.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Property_h
#define __PCL_Property_h

/// \file pcl/Property.h

#include <pcl/Defs.h>

#include <pcl/PropertyDescription.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Property
 * \brief An identifier-value association
 *
 * %Property is a simple identifier-value association, useful to transport
 * properties associated with views and images on the PixInsight platform.
 *
 * The View and FileFormatInstance classes include member functions to generate
 * PropertyArray containers, namely View::Properties(),
 * FileFormatInstance::ReadProperties() and
 * FileFormatInstance::ReadImageProperties().
 */
class PCL_CLASS Property
{
public:

   /*!
    * Represents the string class used to transport property identifiers.
    */
   using identifier_type = IsoString;

   /*!
    * Represents the class used to transport property values.
    */
   using value_type = Variant;

   /*!
    * Represents the data type of a property value.
    */
   using data_type = value_type::data_type;

   /*!
    * Default constructor. Constructs an invalid %Property object with an empty
    * property identifier and an invalid property value.
    */
   Property() = default;

   /*!
    * Copy constructor.
    */
   Property( const Property& ) = default;

   /*!
    * Move constructor.
    */
   Property( Property&& ) = default;

   /*!
    * Constructs a %Property object with the specified property \a identifier
    * and property \a value.
    */
   Property( const identifier_type& identifier, const value_type& value )
      : m_id( identifier )
      , m_value( value )
   {
   }

   /*!
    * Constructs a %Property object with the specified property \a identifier
    * and an undefined (invalid) property value.
    */
   Property( const identifier_type& identifier )
      : m_id( identifier )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Property& operator =( const Property& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Property& operator =( Property&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~Property()
   {
   }

   /*!
    * Returns the property identifier.
    */
   const identifier_type& Identifier() const
   {
      return m_id;
   }

   /*!
    * Returns the property identifier. This function is a convenience synonym
    * for Identifier().
    */
   const identifier_type& Id() const
   {
      return Identifier();
   }

   /*!
    * Returns the property value.
    */
   const value_type& Value() const
   {
      return m_value;
   }

   /*!
    * Assigns the specified \a value to this property. The previous value will
    * be replaced with the new one and hence will be lost.
    */
   void SetValue( const value_type& value )
   {
      m_value = value;
   }

   /*!
    * Returns the data type of this property, which is the data type of its
    * value.
    */
   data_type Type() const
   {
      return m_value.Type();
   }

   /*!
    * Returns a description of this property, including its identifier and
    * value data type.
    */
   PropertyDescription Description() const
   {
      return PropertyDescription( m_id, m_value.Type() );
   }

   /*!
    * Returns true iff the specified string \a id is a valid property
    * identifier. Refer to XISF::IsValidPropertyId() for property identifier
    * syntax rules.
    */
   static bool IsValidIdentifier( const IsoString& id )
   {
      if ( id.IsEmpty() )
         return false;
      IsoStringList tokens;
      id.Break( tokens, ':' );
      for ( const IsoString& token : tokens )
         if ( !token.IsValidIdentifier() )
            return false;
      return true;
   }

   static bool IsValidIdentifier( const IsoString::ustring_base& id )
   {
      return IsValidIdentifier( IsoString( id ) );
   }

   /*!
    * Returns true iff this property is valid. A valid property has a valid
    * identifier and a valid value.
    */
   bool IsValid() const
   {
      return IsValidIdentifier( m_id ) && m_value.IsValid();
   }

   /*!
    * Equality operator. Returns true iff this property is equal to \a other
    * property.
    *
    * Two %Property objects are equal if their identifiers are equal. Keep in
    * mind that this operator compares properties, not their values.
    */
   bool operator ==( const Property& other ) const
   {
      return m_id == other.m_id;
   }

   /*!
    * Less than operator. Returns true iff this property precedes \a other
    * property.
    *
    * For comparisons of %Property objects, only their identifiers are taken
    * into account. Keep in mind that this operator compares properties, not
    * their values.
    */
   bool operator <( const Property& other ) const
   {
      return m_id < other.m_id;
   }

protected:

   identifier_type m_id;
   value_type      m_value;
};

/*!
 * \class pcl::PropertyArray
 * \brief Dynamic array of Property objects
 *
 * %PropertyArray is a template instantiation of Array<> for Property.
 */
using PropertyArray = Array<Property>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Property_h

// ----------------------------------------------------------------------------
// EOF pcl/Property.h - Released 2025-04-07T08:52:44Z
