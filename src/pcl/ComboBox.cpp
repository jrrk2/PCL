//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ComboBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ComboBox.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ComboBox::ComboBox( Control& parent )
   : Control( API_ComboBox_CreateComboBox( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( handle == nullptr )
      throw APIFunctionError( "CreateComboBox" );
}

// ----------------------------------------------------------------------------

int ComboBox::NumberOfItems() const
{
   return API_ComboBox_GetComboBoxLength( handle );
}

// ----------------------------------------------------------------------------

int ComboBox::CurrentItem() const
{
   return API_ComboBox_GetComboBoxCurrentItem( handle );
}

// ----------------------------------------------------------------------------

void ComboBox::SetCurrentItem( int idx )
{
   API_ComboBox_SetComboBoxCurrentItem( handle, idx );
}

// ----------------------------------------------------------------------------

int ComboBox::FindItem( const String& text, int fromIdx, bool exactMatch, bool caseSensitive ) const
{
   return API_ComboBox_FindComboBoxItem( handle, text.c_str(), fromIdx, exactMatch, caseSensitive );
}

// ----------------------------------------------------------------------------

void ComboBox::InsertItem( int idx, const String& text, const Bitmap& icon )
{
   API_ComboBox_InsertComboBoxItem( handle, idx, text.c_str(), icon.handle );
}

// ----------------------------------------------------------------------------

void ComboBox::RemoveItem( int idx )
{
   API_ComboBox_RemoveComboBoxItem( handle, idx );
}

// ----------------------------------------------------------------------------

void ComboBox::Clear()
{
   API_ComboBox_ClearComboBox( handle );
}

// ----------------------------------------------------------------------------

String ComboBox::ItemText( int idx ) const
{
   size_type len = 0;
   API_ComboBox_GetComboBoxItemText( handle, idx, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_ComboBox_GetComboBoxItemText( handle, idx, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetComboBoxItemText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void ComboBox::SetItemText( int idx, const String& text )
{
   API_ComboBox_SetComboBoxItemText( handle, idx, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap ComboBox::ItemIcon( int idx ) const
{
   return Bitmap( API_ComboBox_GetComboBoxItemIcon( handle, idx ) );
}

// ----------------------------------------------------------------------------

void ComboBox::SetItemIcon( int idx, const Bitmap& icon )
{
   API_ComboBox_SetComboBoxItemIcon( handle, idx, icon.handle );
}

// ----------------------------------------------------------------------------

bool ComboBox::IsEditEnabled() const
{
   return API_ComboBox_GetComboBoxEditEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ComboBox::EnableEdit( bool enable )
{
   API_ComboBox_SetComboBoxEditEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

String ComboBox::EditText() const
{
   size_type len = 0;
   API_ComboBox_GetComboBoxEditText( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_ComboBox_GetComboBoxEditText( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetComboBoxEditText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void ComboBox::SetEditText( const String& text )
{
   API_ComboBox_SetComboBoxEditText( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool ComboBox::IsAutoCompletionEnabled() const
{
   return API_ComboBox_GetComboBoxAutoCompletionEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ComboBox::EnableAutoCompletion( bool enable )
{
   API_ComboBox_SetComboBoxAutoCompletionEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

void ComboBox::GetIconSize( int& width, int& height ) const
{
   API_ComboBox_GetComboBoxIconSize( handle, &width, &height );
}

// ----------------------------------------------------------------------------

void ComboBox::SetIconSize( int width, int height )
{
   API_ComboBox_SetComboBoxIconSize( handle, width, height );
}

// ----------------------------------------------------------------------------

int ComboBox::MaxVisibleItemCount() const
{
   return API_ComboBox_GetComboBoxMaxVisibleItemCount( handle );
}

// ----------------------------------------------------------------------------

void ComboBox::SetMaxVisibleItemCount( int n )
{
   API_ComboBox_SetComboBoxMaxVisibleItemCount( handle, n );
}

// ----------------------------------------------------------------------------

int ComboBox::MinItemCharWidth() const
{
   return API_ComboBox_GetComboBoxMinItemCharWidth( handle );
}

// ----------------------------------------------------------------------------

void ComboBox::SetMinItemCharWidth( int n )
{
   API_ComboBox_SetComboBoxMinItemCharWidth( handle, n );
}

// ----------------------------------------------------------------------------

void ComboBox::ShowList()
{
   API_ComboBox_SetComboBoxListVisible( handle, api_true );
}

// ----------------------------------------------------------------------------

void ComboBox::HideList()
{
   API_ComboBox_SetComboBoxListVisible( handle, api_false );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ComboBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ComboBoxEventDispatcher
{
public:

   static void api_func ItemSelected( control_handle hSender, control_handle hReceiver, int32 itemIndex )
   {
      if ( handlers->onItemSelected != nullptr )
         (receiver->*handlers->onItemSelected)( *sender, itemIndex );
   }

   static void api_func ItemHighlighted( control_handle hSender, control_handle hReceiver, int32 itemIndex )
   {
      if ( handlers->onItemHighlighted != nullptr )
         (receiver->*handlers->onItemHighlighted)( *sender, itemIndex );
   }

   static void api_func EditTextUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onEditTextUpdated != nullptr )
         (receiver->*handlers->onEditTextUpdated)( *sender );
   }
}; // ComboBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ComboBox::OnItemSelected( item_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ComboBox_SetComboBoxItemSelectedEventRoutine( handle, &receiver,
                  (f != nullptr) ? ComboBoxEventDispatcher::ItemSelected : nullptr ) == api_false )
      throw APIFunctionError( "SetComboBoxItemSelectedEventRoutine" );
   m_handlers->onItemSelected = f;
}

void ComboBox::OnItemHighlighted( item_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ComboBox_SetComboBoxItemHighlightedEventRoutine( handle, &receiver,
                  (f != nullptr) ? ComboBoxEventDispatcher::ItemHighlighted : nullptr ) == api_false )
      throw APIFunctionError( "SetComboBoxItemHighlightedEventRoutine" );
   m_handlers->onItemHighlighted = f;
}

void ComboBox::OnEditTextUpdated( edit_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ComboBox_SetComboBoxEditTextUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? ComboBoxEventDispatcher::EditTextUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetComboBoxEditTextUpdatedEventRoutine" );
   m_handlers->onEditTextUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ComboBox.cpp - Released 2025-04-07T08:53:32Z
