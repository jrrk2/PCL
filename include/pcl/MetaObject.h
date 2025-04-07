//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MetaObject.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MetaObject_h
#define __PCL_MetaObject_h

/// \file pcl/MetaObject.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/IndirectArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MetaObject
 * \brief Root base class for all PixInsight module components.
 *
 * In PCL, a \e metaobject is a special class describing in formal terms the
 * functionality and properties of a \e component in a PixInsight module.
 *
 * The %MetaObject class provides basic functionality to implement the existing
 * hierarchical relationships between a module and all of its components. A
 * %MetaObject contains a list of %MetaObject instances, which generates a tree
 * structure based on parent-children container relations. This structural
 * layout has been used in PCL to automatize communication between the core
 * PixInsight application, modules, and all module components.
 *
 * \sa MetaModule, MetaProcess, MetaParameter, MetaFileFormat
 */
class PCL_CLASS MetaObject
{
public:

   /*!
    * A container used to implement the list of children module components in
    * %MetaObject.
    */
   using children_list = IndirectArray<MetaObject>;

   /*!
    * Constructs a %MetaObject as a child of the specified \a parent object.
    */
   MetaObject( MetaObject* parent )
      : m_parent( parent )
   {
      if ( m_parent != nullptr )
         m_parent->m_children.Add( this );
   }

   /*!
    * Virtual destructor. Destroys this %MetaObject and all of its children
    * module components.
    */
   virtual ~MetaObject() noexcept( false )
   {
      m_children.Destroy();
   }

   /*!
    * Copy constructor. Copy semantics are disabled for %MetaObject because
    * this class represents unique server-side objects.
    */
   MetaObject( const MetaObject& ) = delete;

   /*!
    * Copy assignment. Copy semantics are disabled for %MetaObject because
    * this class represents unique server-side objects.
    */
   MetaObject& operator =( const MetaObject& ) = delete;

   /*!
    * Move constructor. Move semantics are disabled for %MetaObject because
    * because of parent-child server-side object relations.
    */
   MetaObject( MetaObject&& x ) = delete;

   /*!
    * Move assignment. Move semantics are disabled for %MetaObject because
    * because of parent-child server-side object relations.
    */
   MetaObject& operator =( MetaObject&& x ) = delete;

   /*!
    * Returns the number of module components that depend on this %MetaObject.
    */
   size_type Length() const
   {
      return m_children.Length();
   }

   /*!
    * Returns a pointer to the unmodifiable parent %MetaObject.
    */
   const MetaObject* Parent() const
   {
      return m_parent;
   }

   /*!
    * Returns a pointer to the parent %MetaObject.
    */
   MetaObject* Parent()
   {
      return m_parent;
   }

   /*!
    * Returns a pointer to a %MetaObject children module component, selected by
    * its array index \a i.
    */
   const MetaObject* operator[]( size_type i ) const
   {
      return m_children[i];
   }

protected:

   MetaObject*   m_parent = nullptr;
   children_list m_children;

   /*!
    * \internal
    */
   virtual void PerformAPIDefinitions() const = 0;

   friend class MetaModule;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MetaObject_h

// ----------------------------------------------------------------------------
// EOF pcl/MetaObject.h - Released 2025-04-07T08:52:44Z
