//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/GroupBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/GroupBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

GroupBox::GroupBox( const String& title, Control& parent )
   : Control( (*API->GroupBox->CreateGroupBox)( ModuleHandle(), this, title.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateGroupBox" );
}

// ----------------------------------------------------------------------------

String GroupBox::Title() const
{
   size_type len = 0;
   (*API->GroupBox->GetGroupBoxTitle)( handle, 0, &len );

   String title;
   if ( len > 0 )
   {
      title.SetLength( len );
      if ( (*API->GroupBox->GetGroupBoxTitle)( handle, title.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetGroupBoxTitle" );
      title.ResizeToNullTerminated();
   }
   return title;
}

// ----------------------------------------------------------------------------

void GroupBox::SetTitle( const String& title )
{
   (*API->GroupBox->SetGroupBoxTitle)( handle, title.c_str() );
}

// ----------------------------------------------------------------------------

bool GroupBox::HasTitleCheckBox() const
{
   return (*API->GroupBox->GetGroupBoxCheckable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GroupBox::EnableTitleCheckBox( bool enabled )
{
   (*API->GroupBox->SetGroupBoxCheckable)( handle, enabled );
}

// ----------------------------------------------------------------------------

bool GroupBox::IsChecked() const
{
   return (*API->GroupBox->GetGroupBoxChecked)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void GroupBox::SetChecked( bool checked )
{
   (*API->GroupBox->SetGroupBoxChecked)( handle, checked );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<GroupBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class GroupBoxEventDispatcher
{
public:

   static void api_func Check( control_handle hSender, control_handle hReceiver, int32 state )
   {
      if ( handlers->onCheck != nullptr )
         (receiver->*handlers->onCheck)( *sender, state != 0 );
   }
}; // GroupBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void GroupBox::OnCheck( check_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->GroupBox->SetGroupBoxCheckEventRoutine)( handle, &receiver,
                  (f != nullptr) ? GroupBoxEventDispatcher::Check : nullptr ) == api_false )
      throw APIFunctionError( "SetGroupBoxCheckEventRoutine" );
   m_handlers->onCheck = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/GroupBox.cpp - Released 2025-04-07T08:53:32Z
