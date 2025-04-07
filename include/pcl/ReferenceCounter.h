//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ReferenceCounter.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ReferenceCounter_h
#define __PCL_ReferenceCounter_h

/// \file pcl/ReferenceCounter.h

#include <pcl/Defs.h>

#include <pcl/Atomic.h>

#ifdef __PCL_REFCOUNT_CHECK_DETACHMENT
#  include <pcl/Exception.h>
#else
#  include <pcl/Diagnostics.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ReferenceCounter
 * \brief Thread-safe reference counter for copy-on-write data structures.
 *
 * %ReferenceCounter is used internally by most PCL container and image classes
 * to implement reference-counted, copy-on-write <em>shared data</em>.
 *
 * %ReferenceCounter is a thread-safe reference counter because it implements
 * its internal counter as an instance of the AtomicInt class. This means that
 * counter increment and decrement are implemented as atomic operations.
 *
 * Copy-on-write is a fundamental technique for optimization of resources and
 * execution speed by preventing unnecessary duplication of data, especially
 * when objects are copied and aliased frequently but seldom modified.
 * Basically, a set of objects can share the same data with the condition that
 * the data remains unmodified. In this case, the set of objects is said to be
 * \e referencing a single data item, and all objects in the set are considered
 * as \e aliases of the data. When an alias object tries to modify its
 * referenced data, a new copy is generated on-demand and the object uniquely
 * references the newly generated data, which can then be freely modified. This
 * mechanism works in a completely automatic and transparent way, and
 * %ReferenceCounter just provides a thread-safe counter to keep track of the
 * number of aliased objects that reference the same data structure at a given
 * time, plus a number of useful related functions.
 *
 * \sa AtomicInt
 */
class PCL_CLASS ReferenceCounter
{
public:

   /*!
    * Constructs a new %ReferenceCounter object. The reference counter is
    * initialized to one, which means that only one object (the caller) is
    * referencing the data associated with this object.
    */
   ReferenceCounter() = default;

   /*!
    * Destroys a %ReferenceCounter object.
    */
   ~ReferenceCounter()
   {
   }

   /*!
    * Copy constructor. This constructor is equivalent to the default
    * constructor because reference counters are unique objects. In other
    * words, the reference counter is always initialized to one for newly
    * constructed %ReferenceCounter objects.
    *
    * This constructor exists and is not disabled to allow reference-counted
    * data structures to have copy constructors.
    */
   ReferenceCounter( const ReferenceCounter& )
      : ReferenceCounter()
   {
   }

   /*!
    * Copy assignment. This operator is disabled because reference counters are
    * unique objects.
    */
   ReferenceCounter& operator =( const ReferenceCounter& ) = delete;

   /*!
    * Increments the reference counter by one unit. This happens when a new
    * object references the data being controlled with this counter.
    *
    * \note This attachment operation is thread-safe: the internal increment
    * operation is atomic.
    *
    * \sa Detach()
    */
   void Attach()
   {
      m_count.Increment();
   }

   /*!
    * Decrements the reference counter by one unit. This happens when an object
    * ceases to reference the data being controlled with this counter.
    *
    * Returns true if the reference counter is greater than zero after the
    * decrement; false if the reference counter becomes zero. When the
    * reference counter is zero, the data being controlled with this counter is
    * no longer referenced by any object and thus can be safely destroyed.
    *
    * \note This detachment operation is thread-safe: the internal dereference
    * operation is atomic.
    *
    * \sa Attach()
    */
   bool Detach()
   {
      PCL_CHECK( m_count.Load() > 0 )
      return m_count.Dereference();
   }

   /*!
    * Returns the current value of this reference counter.
    *
    * \sa RefCountAtomic(), IsUnique()
    */
   int RefCount() const
   {
      return m_count;
   }

   /*!
    * Returns the current value of this reference counter.
    *
    * \note This operation is thread-safe: the integer load has been
    * implemented as an atomic operation.
    *
    * \sa RefCount(), IsUniqueAtomic()
    */
   int RefCountAtomic() const
   {
      return m_count.Load();
   }

   /*!
    * Returns true iff the data being controlled with this counter is not being
    * referenced by more than one object.
    *
    * \sa IsUniqueAtomic(), Attach(), Detach()
    */
   bool IsUnique() const
   {
      return RefCount() < 2;
   }

   /*!
    * Returns true iff the data being controlled with this counter is not being
    * referenced by more than one object.
    *
    * \note This operation is thread-safe: the integer comparison has been
    * implemented as an atomic operation.
    *
    * \sa IsUnique(), Attach(), Detach()
    */
   bool IsUniqueAtomic() const
   {
      return RefCountAtomic() < 2;
   }

private:

   mutable AtomicInt m_count = 1;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_ReferenceCounter_h

// ----------------------------------------------------------------------------
// EOF pcl/ReferenceCounter.h - Released 2025-04-07T08:52:44Z
