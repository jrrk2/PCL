//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/AutoViewLock.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_AutoViewLock_h
#define __PCL_AutoViewLock_h

/// \file pcl/AutoViewLock.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Atomic.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class AutoViewLock
 * \brief Automatic view lock/unlock.
 *
 * %AutoViewLock simplifies locking and unlocking View objects accessed from
 * process execution routines.
 *
 * An %AutoViewLock object locks a %View upon construction and unlocks it upon
 * destruction. This ensures that a view will never be left locked, even in
 * critical situations involving multiple function return points and
 * exceptions.
 *
 * %AutoViewLock works for %View just as AutoLock does for Mutex. The main
 * difference is that a view can be locked and unlocked for read and write
 * operations separately, while a mutex cannot.
 *
 * Typically %AutoViewLock is used within a reimplementation of the
 * ProcessImplementation::ExecuteOn() virtual member function for a process
 * instance class. Consider the following example:
 *
 * \code
 * bool FooProcessInstance::ExecuteOn( View& view )
 * {
 *    try
 *    {
 *       view.Lock();
 *
 *       ImageVariant image = view.Image();
 *
 *       if ( image.IsComplexSample() )
 *       {
 *          view.Unlock();
 *          return false;
 *       }
 *
 *       Console().EnableAbort();
 *
 *       StandardStatus status;
 *       image->SetStatusCallback( &status );
 *
 *       FooEngine::Apply( image, *this );
 *
 *       view.Unlock();
 *       return true;
 *   }
 *   catch ( ... )
 *   {
 *      view.Unlock();
 *      throw;
 *   }
 * }
 * \endcode
 *
 * With %AutoViewLock the above code can be simplified considerably:
 *
 * \code
 * bool FooProcessInstance::ExecuteOn( View& view )
 * {
 *    AutoViewLock lock( view );
 *
 *    ImageVariant image = view.Image();
 *
 *    if ( image.IsComplexSample() )
 *       return false;
 *
 *    Console().EnableAbort();
 *
 *    StandardStatus status;
 *    image->SetStatusCallback( &status );
 *
 *    FooEngine::Apply( image, *this );
 *
 *    return true;
 * }
 * \endcode
 *
 * Note that the try-catch blocks are now unnecessary. As soon as the \c lock
 * variable gets out of scope, the \c view object will be unlocked
 * automatically, including both normal function returns and uncaught
 * exceptions within the ExecuteOn() function. Keep in mind that all exceptions
 * will always be caught by internal PCL routines.
 *
 * \sa View
 */
class PCL_CLASS AutoViewLock
{
public:

   /*!
    * Constructs an %AutoViewLock object to monitor the specified \a view.
    *
    * \param view    A %View object that will be monitored by this
    *                %AutoViewLock instance. The object must remain valid for
    *                the whole lifetime of this %AutoViewLock instance.
    *
    * \param lock    Whether the specified \a view should be locked for read
    *                and write operations by this constructor. The default
    *                value is true.
    *
    * If the \a lock argument is true, the specified \a view will be locked for
    * read and write operations immediately by this constructor. It will be
    * unlocked automatically when this %AutoViewLock object gets out of scope,
    * or if it is destroyed explicitly.
    *
    * By specifying \a lock=false, you can create an %AutoViewLock object to
    * monitor a view and lock it for write operations exclusively:
    *
    * \code
    * void foo( View& view )
    * {
    *    AutoViewLock lock( view, false ); // do not lock for read-write ops.
    *    lock.LockForWrite();
    *    ...
    * }
    * \endcode
    *
    * See AutoViewWriteLock for a convenience class to implements this
    * functionality in a cleaner way.
    */
   explicit AutoViewLock( View& view, bool lock = true )
      : m_view( view )
      , m_readLock( 0 )
      , m_writeLock( 0 )
   {
      if ( lock )
         Lock();
   }

   /*!
    * Destroys this %AutoViewLock object.
    *
    * If the monitored view (that was specified in the constructor) is locked,
    * it is unlocked by this destructor.
    */
   ~AutoViewLock()
   {
      Unlock();
      m_view = View::Null();
   }

   /*!
    * Copy constructor. This constructor is disabled because views and view
    * locks are unique objects.
    */
   AutoViewLock( const AutoViewLock& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because views and view locks
    * are unique objects.
    */
   AutoViewLock& operator =( const AutoViewLock& ) = delete;

   /*!
    * Locks the monitored view for read and write operations, if it has not
    * been previously locked.
    *
    * For more information, see the documentation for View::Lock().
    */
   void Lock()
   {
      if ( !m_view.IsNull() )
      {
         int unlocked = 0;
         if ( m_readLock.TestAndSet( 0, 1 ) )
            ++unlocked;
         if ( m_writeLock.TestAndSet( 0, 1 ) )
            ++unlocked;
         if ( unlocked )
            m_view.Lock();
      }
   }

   /*!
    * Unlocks the monitored view for read and write operations, if it has been
    * previously locked.
    *
    * For more information, see the documentation for View::Unlock().
    */
   void Unlock()
   {
      if ( !m_view.IsNull() )
      {
         int locked = 0;
         if ( m_readLock.TestAndSet( 1, 0 ) )
            ++locked;
         if ( m_writeLock.TestAndSet( 1, 0 ) )
            ++locked;
         if ( locked )
            m_view.Unlock();
      }
   }

   /*!
    * Locks the monitored view for write operations, if it has not already been
    * write-locked.
    *
    * For more information, see the documentation for View::LockForWrite().
    */
   void LockForWrite()
   {
      if ( !m_view.IsNull() )
         if ( m_writeLock.TestAndSet( 0, 1 ) )
            m_view.LockForWrite();
   }

   /*!
    * Unlocks the monitored view for read operations, if it has already been
    * read-locked.
    *
    * For more information, see the documentation for View::UnlockForRead().
    */
   void UnlockForRead()
   {
      if ( !m_view.IsNull() )
         if ( m_readLock.TestAndSet( 1, 0 ) )
            m_view.UnlockForRead();
   }

   /*!
    * Unlocks the monitored view for read operations only, if it has not
    * already been unlocked.
    *
    * For more information, see the documentation for View::RelockForRead().
    */
   void RelockForRead()
   {
      if ( !m_view.IsNull() )
         if ( m_readLock.TestAndSet( 0, 1 ) )
            m_view.RelockForRead();
   }

private:

   View      m_view;
   AtomicInt m_readLock;
   AtomicInt m_writeLock;
};

/*!
 * \class AutoViewWriteLock
 * \brief Automatic write-only view lock/unlock.
 *
 * %AutoViewWriteLock performs automatic write-only locking/unlocking of View
 * objects. See the documentation for the base class, AutoViewLock, for
 * complete information.
 *
 * \sa View, AutoViewLock
 */
class PCL_CLASS AutoViewWriteLock : public AutoViewLock
{
public:

   /*!
    * Constructs an %AutoViewWriteLock object to monitor the specified \a view
    * and lock it for write operations exclusively.
    *
    * This constructor is equivalent to the following:
    *
    * \code
    * AutoViewLock lock( view, false ); // do not lock for read-write ops.
    * lock.LockForWrite();
    * \endcode
    */
   explicit AutoViewWriteLock( View& view )
      : AutoViewLock( view, false/*lock*/ )
   {
      LockForWrite();
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_AutoViewLock_h

// ----------------------------------------------------------------------------
// EOF pcl/AutoViewLock.h - Released 2025-04-07T08:52:44Z
