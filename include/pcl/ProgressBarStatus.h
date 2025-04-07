//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ProgressBarStatus.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ProgressBarStatus_h
#define __PCL_ProgressBarStatus_h

/// \file pcl/ProgressBarStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS ProgressDialog;

/*!
 * \class ProgressBarStatus
 * \brief A status monitoring callback that shows a modal progress dialog.
 *
 * %ProgressBarStatus is a StatusCallback derived class that opens a modal
 * ProgressDialog to provide visual feedback about a running process, without
 * freezing the graphical user interface dugin potentially long tasks. It
 * allows the user to interrupt the process by activating a standard Cancel
 * button on the progress dialog. This class supports both bounded and
 * unbounded status monitors.
 *
 * \sa StatusCallback, StatusMonitor, StandardStatus, SpinStatus, MuteStatus
 */
class PCL_CLASS ProgressBarStatus : public StatusCallback
{
public:

   /*!
    * Constructs a %ProgressBarStatus object.
    *
    * \param title   The window title for the modal progress bar dialog box.
    *
    * \param parent  The parent control of the modal progress bar dialog box.
    *                The default value is Control::Null(), which creates a
    *                child top-level window of the current workspace.
    */
   ProgressBarStatus( const String& title, Control& parent = Control::Null() );

   /*!
    * Destroys a %ProgressBarStatus object.
    */
   ~ProgressBarStatus() override;

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

   mutable AutoPointer<ProgressDialog> m_progressDialog;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ProgressBarStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/ProgressBarStatus.h - Released 2025-04-07T08:52:44Z
