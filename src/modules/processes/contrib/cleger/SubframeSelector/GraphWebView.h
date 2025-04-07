//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// GraphWebView.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __GRAPHWEBVIEW_h
#define __GRAPHWEBVIEW_h

#include <pcl/WebView.h>
#include <pcl/Timer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct DataPoint
{
   double x;
   double data;
   double auxData;
   bool   approved;
   bool   locked;
};

typedef GenericVector<DataPoint> DataPointVector;
typedef Array<DataPoint> DataPointArray;

// ----------------------------------------------------------------------------

class GraphWebView : public WebView
{
public:

   GraphWebView( Control& parent = Control::Null() );

   void SetDataset( const String& dataName, const String& auxDataName, const DataPointVector& dataset );

   typedef void (Control::*approve_event_handler)( GraphWebView& sender, int index );
   typedef void (Control::*unlock_event_handler)( GraphWebView& sender, int index );

   void OnApprove( approve_event_handler handler, Control& receiver );
   void OnUnlock( unlock_event_handler handler, Control& receiver );

   void Cleanup();

private:

   String Header() const;
   String Footer() const;

   void e_MouseEnter( Control& sender );
   void e_MouseLeave( Control& sender );
   void e_Timer( Timer& sender );
   void e_JSResult( WebView& sender, const Variant& result );
   void e_LoadFinished( WebView& sender, bool state );

   bool   m_loaded = false;
   Timer  m_eventCheckTimer;
   bool   m_keepChecking = true;
   String m_htmlFilePath;

   struct EventHandlers
   {
      approve_event_handler   onApprove = nullptr;
      Control*                onApproveReceiver = nullptr;
      unlock_event_handler    onUnlock = nullptr;
      Control*                onUnlockReceiver = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_eventHandlers;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GRAPHWEBVIEW_h

// ----------------------------------------------------------------------------
// EOF GraphWebView.h - Released 2025-04-07T08:53:56Z
