//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Timer.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/AutoLock.h>
#include <pcl/Math.h>
#include <pcl/Timer.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Timer*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class TimerEventDispatcher
{
public:

   static void api_func TimerNotify( timer_handle hSender, control_handle hReceiver )
   {
      ++sender->count;
      if ( sender->onTimer != nullptr )
         (receiver->*sender->onTimer)( *sender );
   }

}; // TimerEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

Timer::Timer()
   : UIObject( (*API->Timer->CreateTimer)( ModuleHandle(), this, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateTimer" );
}

// ----------------------------------------------------------------------------

void Timer::OnTimer( timer_event_handler f, Control& receiver )
{
   onTimer = nullptr;
   count = 0;
   if ( (*API->Timer->SetTimerNotifyEventRoutine)( handle, &receiver,
        (f != nullptr) ? TimerEventDispatcher::TimerNotify : nullptr ) == api_false )
   {
      throw APIFunctionError( "SetTimerNotifyEventRoutine" );
   }
   onTimer = f;
}

// ----------------------------------------------------------------------------

Timer& Timer::Null()
{
   static Timer* nullTimer = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullTimer == nullptr )
      nullTimer = new Timer( reinterpret_cast<void*>( 0 ) );
   return *nullTimer;
}

// ----------------------------------------------------------------------------

double Timer::Interval() const
{
   uint32 msec = 0;
   (*API->Timer->GetTimerInterval)( handle, &msec );
   return 0.001*double( msec );
}

// ----------------------------------------------------------------------------

void Timer::SetInterval( double sec )
{
   count = 0;
   (*API->Timer->SetTimerInterval)( handle, (uint32)(1000*Round( sec, 3 )) );
}

// ----------------------------------------------------------------------------

bool Timer::IsSingleShot() const
{
   return (*API->Timer->GetTimerSingleShot)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Timer::SetSingleShot( bool singleShot )
{
   count = 0;
   (*API->Timer->SetTimerSingleShot)( handle, singleShot );
}

// ----------------------------------------------------------------------------

bool Timer::IsRunning() const
{
   return (*API->Timer->IsTimerActive)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Timer::Start()
{
   count = 0;
   if ( (*API->Timer->StartTimer)( handle ) == api_false )
      throw APIFunctionError( "StartTimer" );
}

// ----------------------------------------------------------------------------

void Timer::Stop()
{
   (*API->Timer->StopTimer)( handle );
}

// ----------------------------------------------------------------------------

void* Timer::CloneHandle() const
{
   throw Error( "Cannot clone a Timer handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Timer.cpp - Released 2025-04-07T08:53:32Z
