//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MuteStatus.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_MuteStatus_h
#define __PCL_MuteStatus_h

/// \file pcl/MuteStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Console.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MuteStatus
 * \brief A status monitoring callback that does not provide any progress
 *        information.
 *
 * %MuteStatus does not provide any information on the ongoing monitored
 * process. However, it does check if either the current thread has been
 * aborted (when the status monitoring process has been initiated from a
 * running Thread object), or if the user has requested to cancel the current
 * process (when the process has been initiated from the main thread). In both
 * cases %MuteStatus attempts to terminate the process being monitored.
 *
 * %MuteStatus is useful as a specialized monitoring callback for processes
 * running in threads, or for fast processes that don't require providing
 * detailed progress information to the user. Compared to other callback
 * objects such as StandardStatus and SpinStatus, %MuteStatus saves the tasks
 * of generating textual information and writing it to GUI controls. These
 * auxiliary tasks can be expensive for fast processes, and in some cases they
 * may require even more computing time than the processes themselves.
 *
 * \sa StatusCallback, StatusMonitor, StandardStatus, SpinStatus,
 * ProgressBarStatus, RealTimeProgressStatus, Console
 */
class PCL_CLASS MuteStatus : public StatusCallback
{
public:

   /*!
    * Constructs a default %MuteStatus object.
    */
   MuteStatus() = default;

   /*!
    * Copy constructor.
    */
   MuteStatus( const MuteStatus& x )
      : StatusCallback( x )
   {
   }

   /*!
    * Move constructor.
    */
   MuteStatus( MuteStatus&& x )
      : StatusCallback( std::move( x ) )
      , m_console( std::move( x.m_console ) )
      , m_thread( x.m_thread )
   {
      x.m_thread = nullptr;
   }

   /*!
    * Destroys a %MuteStatus instance.
    */
   ~MuteStatus() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MuteStatus& operator =( const MuteStatus& x )
   {
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MuteStatus& operator =( MuteStatus&& x )
   {
      if ( this != &x )
      {
         (void)StatusCallback::operator =( std::move( x ) );
         m_console = std::move( x.m_console );
         m_thread = x.m_thread;
         x.m_thread = nullptr;
      }
      return *this;
   }

   /*!
    * This function is called by a status \a monitor object when a new
    * monitored process is about to start.
    */
   int Initialized( const StatusMonitor& monitor ) const override;

   /*!
    * Function called by a status \a monitor object to signal an update of the
    * progress count for the current process.
    */
   int Updated( const StatusMonitor& monitor ) const override;

   /*!
    * Function called by a status \a monitor object to signal that the current
    * process has finished.
    */
   int Completed( const StatusMonitor& monitor ) const override;

   /*!
    * Function called by a status \a monitor object when the progress
    * information for the current process has been changed.
    */
   void InfoUpdated( const StatusMonitor& monitor ) const override;

private:

   mutable pcl::Console m_console;
   mutable void*        m_thread = nullptr;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MuteStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/MuteStatus.h - Released 2025-04-07T08:52:44Z
