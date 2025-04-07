//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SpinStatus.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_SpinStatus_h
#define __PCL_SpinStatus_h

/// \file pcl/SpinStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/Console.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SpinStatus
 * \brief A status monitoring callback that shows a spinning character
 *        animation on the processing console.
 *
 * %SpinStatus is a StatusCallback derived class that generates a <em>spinning
 * character animation</em> on the processing console. The spinning animation
 * consists of successively writing the four - \ | / characters at the same
 * position on the console. This is a classical form of text-based animation
 * that provides a nice feedback to the user while a long process is running.
 *
 * %SpinStatus is suitable for processes where a total <em>status monitoring
 * count</em> cannot be computed in advance. For example, this happens with
 * processes that perform an unpredictable number of iterations while a given
 * contition is not fulfilled. When a status monitoring count is available
 * before running a process, the StandardStatus monitoring class is preferable.
 *
 * %SpinStatus and %StandardStatus are used by all standard PixInsight
 * processes and file formats. Unless a specific module has an extremely good
 * reason to proceed otherwise, we recommend that all modules also use these
 * monitoring classes. This ensures that efficient and coherent feedback is
 * provided to the user on the entire PixInsight platform.
 *
 * As %StandardStatus, %SpinStatus also allows the PixInsight core application
 * to process mouse, keyboard and screen update events. In this way the
 * application's GUI remains responsive in case the user requests aborting
 * or suspending the monitored process.
 *
 * \sa StatusCallback, StatusMonitor, Console, StandardStatus,
 * ProgressBarStatus, MuteStatus
 */
class PCL_CLASS SpinStatus : public StatusCallback
{
public:

   /*!
    * Constructs a default %SpinStatus object.
    */
   SpinStatus() = default;

   /*!
    * Copy constructor.
    */
   SpinStatus( const SpinStatus& x )
      : StatusCallback( x )
      , m_spinCount( x.m_spinCount )
   {
   }

   /*!
    * Move constructor.
    */
   SpinStatus( SpinStatus&& x )
      : StatusCallback( std::move( x ) )
      , m_console( std::move( x.m_console ) )
      , m_spinCount( x.m_spinCount )
      , m_thread( x.m_thread )
   {
      x.m_thread = nullptr;
   }

   /*!
    * Destroys a %SpinStatus instance.
    */
   ~SpinStatus() override
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SpinStatus& operator =( const SpinStatus& x )
   {
      (void)StatusCallback::operator =( x );
      m_spinCount = x.m_spinCount;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SpinStatus& operator =( SpinStatus&& x )
   {
      if ( this != &x )
      {
         (void)StatusCallback::operator =( std::move( x ) );
         m_console = std::move( x.m_console );
         m_spinCount = x.m_spinCount;
         m_thread = x.m_thread;
         x.m_thread = nullptr;
      }
      return *this;
   }

   /*!
    * This function is called by a status monitor object \a m when a new
    * monitored process is about to start.
    */
   int Initialized( const StatusMonitor& m ) const override;

   /*!
    * Function called by a status monitor object \a m to signal an update of
    * the progress count for the current process.
    */
   int Updated( const StatusMonitor& m ) const override;

   /*!
    * Function called by a status monitor object \a m to signal that the
    * current process has finished.
    */
   int Completed( const StatusMonitor& m ) const override;

   /*!
    * Function called by a status monitor object \a m when the progress
    * information for the current process has been changed.
    */
   void InfoUpdated( const StatusMonitor& m ) const override;

private:

   mutable pcl::Console m_console;
   mutable unsigned     m_spinCount = 0;
   mutable void*        m_thread = nullptr;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_SpinStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/SpinStatus.h - Released 2025-04-07T08:52:44Z
