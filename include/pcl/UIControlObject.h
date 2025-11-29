//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/UIControlObject.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_UIControlObject_h
#define __PCL_UIControlObject_h

/// \file pcl/UIControlObject.h

#include <pcl/Defs.h>

#include <pcl/Relational.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class UIControlObject
 * \brief Root base class for all user interface objects.
 *
 * %UIControlObject is a high-level, managed client-side interface to a server-side
 * object living in the PixInsight core application. %UIControlObject is the root base
 * class of all PCL classes representing objects that can be constructed and
 * destroyed in the PixInsight platform, such as most user interface elements,
 * file format instances, and process instances.
 *
 * For example, Bitmap and ProcessInstance are derived classes of %UIControlObject
 * because you can generate new instances of the server-side objects
 * represented by these classes. Contrarily, FileFormat and Process are not
 * %UIControlObject descendants because these classes represent installed components
 * that cannot be created or modified by a module.
 *
 * All the server-side objects represented by %UIControlObject and its derived classes
 * are <em>reference counted</em> objects. When you create an instance of
 * %UIControlObject (or a derived class) in your module, you either generate a new
 * object in the platform, or increase the reference counter of an already
 * existing object. When a server-side object becomes unreferenced, that is,
 * when no living %UIControlObject represents it, it is either destroyed or recycled
 * by the PixInsight core application as soon as possible, in order to reuse
 * and optimize the available resources. This process is known as <em>garbage
 * collection</em>, and works in a completely automatic and transparent fashion
 * in the PixInsight platform.
 */

class PCL_CLASS Control;

class PCL_CLASS UIControlObject
{
public:

   /*!
    * Virtual destructor.
    *
    * After destruction of this %UIControlObject, the server-side object is also
    * destroyed or recycled if it is no longer referenced by other high-level
    * interfaces (e.g., other instances of %UIControlObject in this or other modules).
    */
   virtual ~UIControlObject() noexcept( false );

   /*!
    * Returns true iff this is a \e null %UIControlObject instance.
    *
    * A null %UIControlObject does not represent an existing object in the PixInsight
    * core application.
    */
   bool IsNull() const
   {
      return handle == nullptr;
   }

   /*!
    * Returns a reference to a null %UIControlObject instance.
    *
    * A null %UIControlObject does not represent an existing object in the PixInsight
    * core application.
    */
   static UIControlObject& Null();

   /*!
    * Returns the total number of existing references to the server-side object
    * managed by this %UIControlObject.
    *
    * The returned value is the sum of all references to the server-side object
    * on the whole PixInsight platform, including all installed modules and the
    * PixInsight core application.
    */
   size_type RefCount() const;

   /*!
    * Returns true iff the server-side object is uniquely referenced by this
    * %UIControlObject instance.
    *
    * A uniquely referenced server-side object will be destroyed or recycled
    * upon destruction of this %UIControlObject.
    */
   bool IsUnique() const
   {
      return RefCount() < 2;
   }

   /*!
    * Returns true iff this %UIControlObject instance is an alias to an existing object
    * in the calling module.
    *
    * An alias and its aliased object are interchangeable; other than
    * module-specific functionality, their behaviors are identical since they
    * refer to a unique object living in the PixInsight core application. An
    * alias %UIControlObject is generated each time a new %UIControlObject descendant class
    * is instantiated using (explicitly or indirectly) a copy constructor. For
    * example:
    *
    * \code
    * ImageWindow w1( 256, 256 ); // ImageWindow is an UIControlObject derived class
    * ImageWindow w2 = w1;    // w2 is an alias to w1.
    * bool b1 = w1.IsAlias(); // b1 is now false
    * bool b2 = w2.IsAlias(); // b2 is now true
    * w2.Show(); // OK, shows w1.
    * w1.HideMask();
    * bool b3 = w2.IsMaskVisible(); // b3 is now false
    * w2.Close(); // OK, the window is destroyed and both w1 and w2 are now invalid objects.
    * \endcode
    *
    * The only limit specific to aliased objects is the fact that an alias
    * Control (or descendant) cannot set event handlers. See the documentation
    * for Control( const Control& ) for more information.
    */
   bool IsAlias() const
   {
      return alias;
   }

   /*!
    * Returns true iff this managed object represents an unreferenced
    * server-side object. Such objects are said to be 'garbage' in the
    * programming jargon, and are subject to eventual destruction through a
    * process known as \e garbage \e collection.
    */
   bool IsGarbage() const
   {
      return RefCount() < 1;
   }

   /*!
    * Returns true iff this instance and other %UIControlObject \a o reference the same
    * server-side object.
    *
    * If two %UIControlObject instances reference the same server-side object, each of
    * them act as an alias for the other: In all respects, both high-level
    * instances are interchangeable.
    */
   bool IsSameObject( const UIControlObject& o ) const
   {
      return handle == o.handle;
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * If there are other %UIControlObject instances that refer to the same server-side
    * object, this member function attempts to create a new server-side object
    * as a copy of the currently managed object, and references it.
    *
    * If the server-side object is only referenced by this %UIControlObject instance,
    * calling this member function has no effect.
    *
    * Some derived classes represent object types whose instances are unique by
    * nature. In those cases, calling this member function has no effect.
    */
   virtual void EnsureUnique();

   /*!
    * A convenience synonym for IsSameObject( o ).
    */
   bool operator ==( const UIControlObject& o ) const
   {
      return IsSameObject( o );
   }

   /*!
    * Returns true iff this %UIControlObject instance precedes another %UIControlObject \a o.
    * Performs a comparison based on the low-level handle internally maintained
    * by each %UIControlObject.
    *
    * This member function is actually intended for fast access to large sets
    * of %UIControlObject instances stored in containers (as for example the whole set
    * of user interface objects defined in a module). It is seldom used
    * explicitly by modules.
    */
   bool operator <( const UIControlObject& o ) const
   {
      return handle < o.handle;
   }

   /*!
    * Returns a string that uniquely identifies the type of the server-side UI
    * object managed by this %UIControlObject.
    */
   IsoString ObjectType() const;

   /*!
    * Returns the identifier of this UI object. The object identifier is a
    * text string associated with the server-side UI object managed by this
    * %UIControlObject instance.
    *
    * When this function is called for the first time for a given object, and
    * no object identifier has been set explicitly in a previous call to
    * SetObjectId(), a new identifier is generated automatically. In this case
    * the generated object identifier is guaranteed to be unique across the
    * entire PixInsight platform.
    *
    * Object identifiers are useful to select individual instances. For
    * example, an object identifier can be used to restrict the scope of an
    * interface style sheet to a particular control. See the documentation for
    * Control::SetStyleSheet() for more information.
    *
    * \sa SetObjectId()
    */
   String ObjectId() const;

   /*!
    * Sets the identifier of this UI object. Forcing an object identifier can
    * be useful to select one or more controls for CSS styling with the
    * Control::SetStyleSheet() member function. See the documentation for
    * ObjectId() for more information.
    *
    * \sa ObjectId()
    */
   void SetObjectId( const String& id );

protected:

   Control * handle = nullptr;
   bool  alias  = false;

   /*!
    * Constructs a null %UIControlObject instance.
    *
    * A null %UIControlObject does not correspond to an existing object in the
    * PixInsight core application.
    */
   UIControlObject() = default;

   /*!
    * Copy constructor.
    *
    * Unless a derived class redefines it otherwise (which is rare), this
    * constructor does not create a new object in the PixInsight core
    * application. It simply creates an alias %UIControlObject instance that
    * references the same server-side object as the source instance \a x.
    */
   UIControlObject( const UIControlObject& x );

   /*!
    * Move constructor.
    */
   UIControlObject( UIControlObject&& x )
      : handle( x.handle )
      , alias( x.alias )
   {
      x.handle = nullptr;
      x.alias = false;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   UIControlObject& operator =( const UIControlObject& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   UIControlObject& operator =( UIControlObject&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * \internal
    */
   UIControlObject( Control * );

   /*!
    * \internal
    */
   UIControlObject( const Control * );

   /*!
    * \internal
    */
   UIControlObject( std::nullptr_t )
   {
   }

   /*!
    * \internal
    */
   void SetHandle( void* );

   /*!
    * \internal
    */
   void TransferHandle( void* );

   /*!
    * \internal
    * This virtual function is called to construct object duplicates. Derived
    * classes must reimplement this function to return the handle of a newly
    * constructed UI object.
    */
   virtual void* CloneHandle() const
   {
      return nullptr;
   }

   /*!
    * \internal
    */
   bool Assign( const UIControlObject& x )
   {
      if ( x.handle != handle )
      {
         SetHandle( x.handle );
         return true;
      }
      return false;
   }

   /*!
    * \internal
    */
   bool Transfer( UIControlObject& x )
   {
      if ( &x != this )
      {
         SetHandle( nullptr );
         handle = x.handle;
         alias = x.alias;
         x.handle = nullptr;
         x.alias = false;
         return true;
      }
      return false;
   }

   friend class UIControlObjectIndex;
   friend class UIEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_UIControlObject_h

// ----------------------------------------------------------------------------
// EOF pcl/UIControlObject.h - Released 2025-04-07T08:52:44Z
