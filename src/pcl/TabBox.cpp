//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/TabBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/TabBox.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TabBox::TabBox( Control& parent )
   : Control( API_TabBox_CreateTabBox( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTabBox" );
}

// ----------------------------------------------------------------------------

int TabBox::NumberOfPages() const
{
   return API_TabBox_GetTabBoxLength( handle );
}

// ----------------------------------------------------------------------------

int TabBox::CurrentPageIndex() const
{
   return API_TabBox_GetTabBoxCurrentPageIndex( handle );
}

// ----------------------------------------------------------------------------

void TabBox::SetCurrentPageIndex( int idx )
{
   API_TabBox_SetTabBoxCurrentPageIndex( handle, idx );
}

// ----------------------------------------------------------------------------

Control& TabBox::CurrentPageControl() const
{
   int i = CurrentPageIndex();
   return (i >= 0) ? PageControlByIndex( i ) : Control::Null();
}

// ----------------------------------------------------------------------------

Control& TabBox::PageControlByIndex( int idx ) const
{
   Control* w = reinterpret_cast<Control*>( API_TabBox_GetTabBoxPageByIndex( handle, idx ) );
   return (w != nullptr) ? *w : Control::Null();
}

// ----------------------------------------------------------------------------

void TabBox::InsertPage( int idx, Control& page, const String& label, const Bitmap& icon )
{
   API_TabBox_InsertTabBoxPage( handle, idx, page.handle, label.c_str(), icon.handle );
}

// ----------------------------------------------------------------------------

void TabBox::RemovePage( int idx )
{
   API_TabBox_RemoveTabBoxPage( handle, idx );
}

// ----------------------------------------------------------------------------

TabBox::tab_position TabBox::TabPosition() const
{
   return tab_position( API_TabBox_GetTabBoxPosition( handle ) );
}

// ----------------------------------------------------------------------------

void TabBox::SetTabPosition( TabBox::tab_position pos )
{
   API_TabBox_SetTabBoxPosition( handle, pos );
}

// ----------------------------------------------------------------------------

bool TabBox::IsPageEnabled( int idx ) const
{
   return API_TabBox_GetTabBoxPageEnabled( handle, idx ) != api_false;
}

// ----------------------------------------------------------------------------

void TabBox::EnablePage( int idx, bool enable )
{
   API_TabBox_SetTabBoxPageEnabled( handle, idx, enable );
}

// ----------------------------------------------------------------------------

String TabBox::PageLabel( int idx ) const
{
   size_type len = 0;
   API_TabBox_GetTabBoxPageLabel( handle, idx, 0, &len );

   String label;
   if ( len > 0 )
   {
      label.SetLength( len );
      if ( API_TabBox_GetTabBoxPageLabel( handle, idx, label.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTabBoxPageLabel" );
      label.ResizeToNullTerminated();
   }
   return label;
}

// ----------------------------------------------------------------------------

void TabBox::SetPageLabel( int idx, const String& label )
{
   API_TabBox_SetTabBoxPageLabel( handle, idx, label.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap TabBox::PageIcon( int idx ) const
{
   return Bitmap( API_TabBox_GetTabBoxPageIcon( handle, idx ) );
}

// ----------------------------------------------------------------------------

void TabBox::SetPageIcon( int idx, const Bitmap& pm )
{
   API_TabBox_SetTabBoxPageIcon( handle, idx, pm.handle );
}

// ----------------------------------------------------------------------------

String TabBox::PageToolTip( int idx ) const
{
   size_type len = 0;
   API_TabBox_GetTabBoxPageToolTip( handle, idx, 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( API_TabBox_GetTabBoxPageToolTip( handle, idx, tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTabBoxPageToolTip" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

void TabBox::SetPageToolTip( int idx, const String& toolTip )
{
   API_TabBox_SetTabBoxPageToolTip( handle, idx, toolTip.c_str() );
}

// ----------------------------------------------------------------------------

Control& TabBox::LeftControl() const
{
   Control* w = reinterpret_cast<Control*>( API_TabBox_GetTabBoxLeftControl( handle ) );
   return (w != nullptr) ? *w : Control::Null();
}

// ----------------------------------------------------------------------------

Control& TabBox::RightControl() const
{
   Control* w = reinterpret_cast<Control*>( API_TabBox_GetTabBoxRightControl( handle ) );
   return (w != nullptr) ? *w : Control::Null();
}

// ----------------------------------------------------------------------------

void TabBox::SetControls( Control& left, Control& right )
{
   API_TabBox_SetTabBoxControls( handle, left.handle, right.handle );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<TabBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class TabBoxEventDispatcher
{
public:

   static void api_func PageSelected( control_handle hSender, control_handle hReceiver, int32 pageIndex )
   {
      if ( handlers->onPageSelected != nullptr )
         (receiver->*handlers->onPageSelected)( *sender, pageIndex );
   }

}; // TabBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void TabBox::OnPageSelected( page_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_TabBox_SetTabBoxPageSelectedEventRoutine( handle, &receiver,
                  (f != nullptr) ? TabBoxEventDispatcher::PageSelected : nullptr ) == api_false )
      throw APIFunctionError( "SetTabBoxPageSelectedEventRoutine" );
   m_handlers->onPageSelected = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/TabBox.cpp - Released 2025-04-07T08:53:32Z
