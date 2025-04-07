//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ReadWriteMutex.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ReadWriteMutex_h
#define __PCL_ReadWriteMutex_h

/// \file pcl/ReadWriteMutex.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/UIObject.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ReadWriteMutex
 * \brief Client-side interface to a PixInsight read/write mutex object
 *
 * %ReadWriteMutex provides separate thread synchronization for read and write
 * operations. This can be more efficient than plain Mutex synchronization
 * because multiple threads can access a data item for read-only operations
 * simultaneously, which in turn improves concurrency if write accesses are
 * relatively infrequent.
 *
 * \sa Mutex
 */
class PCL_CLASS ReadWriteMutex : public UIObject
{
public:

   /*!
    * Constructs a %ReadWriteMutex object.
    */
   ReadWriteMutex();

   /*!
    * Destroys a %ReadWriteMutex object.
    *
    * \warning    Destroying a locked %ReadWriteMutex object invokes undefined
    * (mostly catastrophic) behavior. Always ensure that a read/write mutex has
    * been unlocked before destroying it.
    */
   ~ReadWriteMutex() override
   {
   }

   /*!
    * Copy constructor. This constructor is disabled because %ReadWriteMutex
    * represents unique server-side objects.
    */
   ReadWriteMutex( const ReadWriteMutex& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %ReadWriteMutex
    * represents unique server-side objects.
    */
   ReadWriteMutex& operator =( const ReadWriteMutex& ) = delete;

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since mutexes are unique objects by definition, calling this member
    * function has no effect.
    */
   void EnsureUnique() override
   {
      // ReadWriteMutex instances are unique objects
   }

   /*!
    * Returns a reference to a null %ReadWriteMutex instance. A null
    * %ReadWriteMutex does not correspond to an existing read/write mutex in
    * the PixInsight core application.
    */
   static ReadWriteMutex& Null();

   /*!
    * Locks this read/write mutex object for reading.
    *
    * The calling thread will block its execution if other thread has locked
    * this read/write mutex for writing.
    */
   void LockForRead();

   /*!
    * Locks this read/write mutex object for writing.
    *
    * The calling thread will block its execution if other thread has locked
    * this read/write mutex for reading or writing.
    */
   void LockForWrite();

   /*!
    * Attempts locking this read/write mutex object for reading. Returns true
    * if this mutex has been successfully locked by the calling thread, false
    * otherwise.
    *
    * Locking for reading will only fail if other thread has locked this object
    * for writing. If other threads have locked this object for reading only,
    * the calling thread will also lock it and this function will return true.
    *
    * Unlike LockForRead(), this function does not block execution of the
    * calling thread if this %ReadWriteMutex cannot be locked for reading.
    */
   bool TryLockForRead();

   /*!
    * Attempts locking this read/write mutex object for writing. Returns true if
    * this mutex has been successfully locked by the calling thread, false
    * otherwise.
    *
    * Locking for writing will fail if other thread has locked this object for
    * reading or writing.
    *
    * Unlike LockForWrite(), this function does not block execution of the
    * calling thread if this %ReadWriteMutex cannot be locked for writing.
    */
   bool TryLockForWrite();

   /*!
    * Unlocks this %ReadWriteMutex object.
    */
   void Unlock();

private:

   ReadWriteMutex( void* h ) : UIObject( h )
   {
   }

   void* CloneHandle() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ReadWriteMutex_h

// ----------------------------------------------------------------------------
// EOF pcl/ReadWriteMutex.h - Released 2025-04-07T08:52:44Z
