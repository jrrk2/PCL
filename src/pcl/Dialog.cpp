//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Dialog.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Dialog.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Dialog::Dialog( Control& parent )
   : Control( (*API->Dialog->CreateDialog)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateDialog" );
}

// ----------------------------------------------------------------------------

int Dialog::Execute()
{
   return (*API->Dialog->ExecuteDialog)( handle );
}

// ----------------------------------------------------------------------------

void Dialog::Open()
{
   (*API->Dialog->OpenDialog)( handle );
}

// ----------------------------------------------------------------------------

void Dialog::Return( int retVal )
{
   (*API->Dialog->ReturnDialog)( handle, retVal );
}

// ----------------------------------------------------------------------------

bool Dialog::IsUserResizable() const
{
   return (*API->Dialog->GetDialogResizable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Dialog::EnableUserResizing( bool enable )
{
   (*API->Dialog->SetDialogResizable)( handle, enable );
}

// ----------------------------------------------------------------------------

void Dialog::ProcessEvents( bool excludeUserInputEvents )
{
   if ( Module != nullptr )
      Module->ProcessEvents( excludeUserInputEvents );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Dialog*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class DialogEventDispatcher
{
public:

   static void Executed( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onExecute != nullptr )
         (receiver->*handlers->onExecute)( *sender );
   }

   static void Returned( control_handle hSender, control_handle hReceiver, int32 retVal )
   {
      if ( handlers->onReturn != nullptr )
         (receiver->*handlers->onReturn)( *sender, retVal );
   }
}; // DialogEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void Dialog::OnExecute( execute_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Dialog->SetExecuteDialogEventRoutine)( handle, &receiver,
                  (f != nullptr) ? DialogEventDispatcher::Executed : nullptr ) == api_false )
      throw APIFunctionError( "SetExecuteDialogEventRoutine" );
   m_handlers->onExecute = f;
}

void Dialog::OnReturn( return_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->Dialog->SetReturnDialogEventRoutine)( handle, &receiver,
                  (f != nullptr) ? DialogEventDispatcher::Returned : nullptr ) == api_false )
      throw APIFunctionError( "SetReturnDialogEventRoutine" );
   m_handlers->onReturn = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Dialog.cpp - Released 2025-04-07T08:53:32Z
