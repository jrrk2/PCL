//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIParamListTypes.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIParamListTypes_h
#define __INDIParamListTypes_h

#include <pcl/Array.h>
#include <pcl/String.h>

#include "IndigoDefs.h"

#include <indigo/indigo_bus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct INDIDeviceListItem
{
   String DeviceName;
   String DeviceLabel;

   bool operator==( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName == rhs.DeviceName;
   }

   bool operator<( const INDIDeviceListItem& rhs ) const
   {
      return DeviceName < rhs.DeviceName;
   }
};

struct INDIPropertyListItem
{
   String Device;
   String Property;
   indigo_property_type PropertyType;
   String PropertyTypeStr;
   String Element;
   indigo_property_state PropertyState;
   String PropertyNumberFormat;
   String PropertyLabel;
   String ElementLabel;
   String PropertyKey;
   String PropertyValue;
   String PropertyTarget;
   String NewPropertyValue;

   bool operator==( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey == rhs.PropertyKey;
   }

   bool operator<( const INDIPropertyListItem& rhs ) const
   {
      return PropertyKey < rhs.PropertyKey;
   }
};

struct BelongsToSameDevice
{
   bool operator()( const INDIPropertyListItem& left, const INDIPropertyListItem& right )
   {
      return left.Device == right.Device;
   }
};

struct ElementValue
{
   String Element;
   String Value;

   template <typename T>
   ElementValue( const String& element, const T& value )
      : Element( element )
      , Value( String( value ) )
   {
   }

   bool operator==( const ElementValue& rhs ) const
   {
      return Element == rhs.Element && Value == rhs.Value;
   }

   bool operator<( const ElementValue& rhs ) const
   {
      return (Element != rhs.Element) ? Element < rhs.Element : Value < rhs.Value;
   }
};

struct INDINewPropertyItem
{
   typedef Array<ElementValue> element_value_list;

   String Device;
   String Property;
   String PropertyKey;
   String PropertyType;
   element_value_list ElementValues;

   INDINewPropertyItem() = default;

   INDINewPropertyItem( const String& device, const String& property, const String& type )
      : Device( device )
      , Property( property )
      , PropertyType( type )
   {
   }

   template <typename T>
   INDINewPropertyItem( const String& device, const String& property, const String& type,
                        const String& element, const T& value )
      : Device( device )
      , Property( property )
      , PropertyType( type )
   {
      ElementValues << ElementValue( element, value );
   }

   template <typename T1, typename T2>
   INDINewPropertyItem( const String& device, const String& property, const String& type,
                        const String& element1, const T1& value1,
                        const String& element2, const T2& value2 )
      : Device( device )
      , Property( property )
      , PropertyType( type )
   {
      ElementValues << ElementValue( element1, value1 )
                    << ElementValue( element2, value2 );
   }

   bool operator==( const INDINewPropertyItem& rhs ) const
   {
      return Device == rhs.Device
          && Property == rhs.Property
          && PropertyKey == rhs.PropertyKey
          && PropertyType == rhs.PropertyType
          && ElementValues == rhs.ElementValues;
   }
};

struct INDINewPropertyListItem
{
   String PropertyKey;
   String PropertyType;
   String NewPropertyValue;

   bool operator==( const INDINewPropertyListItem& rhs ) const
   {
      return PropertyKey == rhs.PropertyKey
          && PropertyType == rhs.PropertyType
          && NewPropertyValue == rhs.NewPropertyValue;
   }
};

typedef Array<INDIDeviceListItem> INDIDeviceListItemArray;
typedef Array<INDIPropertyListItem> INDIPropertyListItemArray;
typedef Array<INDINewPropertyListItem> INDINewPropertyListItemArray;

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIParamListTypes_h

// ----------------------------------------------------------------------------
// EOF INDIParamListTypes.h - Released 2025-04-07T08:53:56Z
