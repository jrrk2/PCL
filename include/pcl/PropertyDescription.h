//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PropertyDescription.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_PropertyDescription_h
#define __PCL_PropertyDescription_h

/// \file pcl/PropertyDescription.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Variant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PropertyDescription
 * \brief A structure to describe a data property
 *
 * This structure is used (instantiated in PropertyDescriptionArray
 * containers) by the FileFormatImplementation and FileFormaInstance classes to
 * describe data properties stored in image files, among other classes.
 *
 * \sa FileFormatImplementation, FileFormatInstance
 */
struct PCL_CLASS PropertyDescription
{
   /*!
    * An enumeration of supported property data types.
    */
   using data_type = VariantType::value_type;

   IsoString id;                          //!< The identifier of this property.
   data_type type = VariantType::Invalid; //!< The property's data type.

   /*!
    * Default constructor. Constructs an %PropertyDescription object with
    * an empty property identifier and unspecified (invalid) data type.
    */
   PropertyDescription() = default;

   /*!
    * Constructs an %PropertyDescription object with the specified
    * property identifier and data type.
    */
   PropertyDescription( const IsoString& a_id, data_type a_type = VariantType::Invalid )
      : id( a_id )
      , type( a_type )
   {
   }

   /*!
    * Copy constructor.
    */
   PropertyDescription( const PropertyDescription& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PropertyDescription& operator =( const PropertyDescription& ) = default;

   /*!
    * Returns true iff this object represents a valid data property.
    */
   bool IsValid() const
   {
      return type != VariantType::Invalid;
   }

   /*!
    * Less-than relational operator. Returns true if this object precedes the
    * specified object \a x; otherwise returns false.
    *
    * Since property identifiers are unique within a property's scope (within
    * an image file, for example), comparisons of %PropertyDescription
    * structures only take into account the PropertyDescription::id member.
    */
   bool operator <( const PropertyDescription& x ) const
   {
      return id < x.id;
   }

   /*!
    * Equality operator. Returns true if this object refers to the same
    * property as the specified object \a x; otherwise returns false.
    *
    * Since property identifiers are unique within a property's scope (within
    * an image file, for example), comparisons of %PropertyDescription
    * structures only take into account the PropertyDescription::id member.
    */
   bool operator ==( const PropertyDescription& x ) const
   {
      return id == x.id;
   }
};

/*!
 * \class pcl::PropertyDescriptionArray
 * \brief Dynamic array of PropertyDescription structures
 *
 * %PropertyDescriptionArray is a template instantiation of Array for
 * PropertyDescription.
 */
using PropertyDescriptionArray = Array<PropertyDescription>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PropertyDescription_h

// ----------------------------------------------------------------------------
// EOF pcl/PropertyDescription.h - Released 2025-04-07T08:52:44Z
