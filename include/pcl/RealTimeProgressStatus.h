//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/RealTimeProgressStatus.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_RealTimeProgressStatus_h
#define __PCL_RealTimeProgressStatus_h

/// \file pcl/RealTimeProgressStatus.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class RealTimeProgressStatus
 * \brief A status monitoring callback to provide progress feedback during long
 * real-time preview generation procedures.
 *
 * %RealTimeProgressStatus allows you to drive the Real-Time Preview progress
 * dialog during long real-time preview generation tasks. Using this class is
 * recommended for real-time renditions consistently taking more than a couple
 * of seconds to complete on contemporary hardware.
 *
 * For detailed information on the Real-Time Preview system and its associated
 * modal progress dialog functionality, please refer to the RealTimePreview
 * class and the RealTimePreview::ShowProgressDialog() static member function.
 *
 * \sa RealTimePreview, StatusCallback, StatusMonitor, MuteStatus
 */
class PCL_CLASS RealTimeProgressStatus : public MuteStatus
{
public:

   /*!
    * Constructs a %RealTimeProgressStatus instance.
    *
    * \param title   The text that will be shown as the tile of the Real-Time
    *                Preview progress dialog window. Typically this text will
    *                be shown centered on the dialog's title bar.
    */
   RealTimeProgressStatus( const String& title )
      : m_title( title )
   {
   }

   /*!
    * Copy constructor.
    */
   RealTimeProgressStatus( const RealTimeProgressStatus& x )
      : MuteStatus( x )
      , m_title( x.m_title )
   {
   }

   /*!
    * This function is called by a status \a monitor object when a new
    * monitored process is about to start.
    *
    * As reimplemented in %RealTimeProgressStatus, this function opens the
    * Real-Time Preview progress dialog window and initializes it with the
    * monitor's total count and information text.
    */
   int Initialized( const StatusMonitor& monitor ) const override
   {
      if ( MuteStatus::Initialized( monitor ) == 0 )
      {
         RealTimePreview::ShowProgressDialog( m_title, monitor.Info(), monitor.Total() );
         return 0;
      }
      return 1;
   }

   /*!
    * Function called by a status \a monitor object to signal an update of the
    * progress count for the current process.
    *
    * As reimplemented in %RealTimeProgressStatus, this function updates the
    * Real-Time Preview progress dialog with the current monitor's count.
    */
   int Updated( const StatusMonitor& monitor ) const override
   {
      if ( MuteStatus::Updated( monitor ) == 0 )
      {
         RealTimePreview::SetProgressCount( monitor.Count() );
         return 0;
      }
      return 1;
   }

   /*!
    * Function called by a status \a monitor object to signal that the current
    * process has finished.
    *
    * As reimplemented in %RealTimeProgressStatus, this function closes the
    * Real-Time Preview progress dialog window.
    */
   int Completed( const StatusMonitor& monitor ) const override
   {
      if ( MuteStatus::Completed( monitor ) == 0 )
      {
         RealTimePreview::CloseProgressDialog();
         return 0;
      }
      return 1;
   }

   /*!
    * Function called by a status \a monitor object when the progress
    * information for the current process has been changed.
    *
    * As reimplemented in %RealTimeProgressStatus, this function updates the
    * single-line label text on the Real-Time Preview progress dialog.
    */
   void InfoUpdated( const StatusMonitor& monitor ) const override
   {
      RealTimePreview::SetProgressText( monitor.Info() );
   }

private:

   String m_title;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_RealTimeProgressStatus_h

// ----------------------------------------------------------------------------
// EOF pcl/RealTimeProgressStatus.h - Released 2025-04-07T08:52:44Z
