//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Edit.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Edit.h>
#include <pcl/TextAlign.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Edit::Edit( const String& text, Control& parent )
   : Frame( API_Edit_CreateEdit( ModuleHandle(), this, text.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateEdit" );
}

// ----------------------------------------------------------------------------

String Edit::Text() const
{
   size_type len = 0;
   API_Edit_GetEditText( handle, nullptr/*text*/, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_Edit_GetEditText( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetEditText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void Edit::SetText( const String& text )
{
   API_Edit_SetEditText( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool Edit::IsReadOnly() const
{
   return API_Edit_GetEditReadOnly( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Edit::SetReadOnly( bool readOnly )
{
   API_Edit_SetEditReadOnly( handle, readOnly );
}

// ----------------------------------------------------------------------------

bool Edit::IsModified() const
{
   return API_Edit_GetEditModified( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Edit::SetModified( bool modified )
{
   API_Edit_SetEditModified( handle, modified );
}

// ----------------------------------------------------------------------------

bool Edit::IsPasswordMode() const
{
   return API_Edit_GetEditPasswordEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Edit::EnablePasswordMode( bool enable )
{
   API_Edit_SetEditPasswordEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

String Edit::ValidatingRegExp() const
{
   size_type len = 0;
   API_Edit_GetEditValidatingRegExp( handle, nullptr, &len, nullptr/*caseSensitive*/ );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_Edit_GetEditValidatingRegExp( handle, text.Begin(), &len, nullptr/*caseSensitive*/ ) == api_false )
         throw APIFunctionError( "GetEditValidatingRegExp" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

String Edit::ValidatingRegExp( bool& caseSensitive ) const
{
   size_type len = 0;
   API_Edit_GetEditValidatingRegExp( handle, nullptr, &len, nullptr/*caseSensitive*/ );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      api_bool cs = api_true;
      if ( API_Edit_GetEditValidatingRegExp( handle, text.Begin(), &len, &cs ) == api_false )
         throw APIFunctionError( "GetEditValidatingRegExp" );
      text.ResizeToNullTerminated();
      caseSensitive = cs != api_false;
   }
   return text;
}

// ----------------------------------------------------------------------------

void Edit::SetValidatingRegExp( const String& rx, bool caseSensitive )
{
   if ( API_Edit_SetEditValidatingRegExp( handle, rx.c_str(), api_bool( caseSensitive ) ) == api_false )
      throw APIFunctionError( "SetEditValidatingRegExp" );
}

// ----------------------------------------------------------------------------

bool Edit::IsValid() const
{
   return API_Edit_GetEditValid( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int Edit::MaxLength() const
{
   return API_Edit_GetEditMaxLength( handle );
}

// ----------------------------------------------------------------------------

void Edit::SetMaxLength( int n )
{
   API_Edit_SetEditMaxLength( handle, n );
}

// ----------------------------------------------------------------------------
/*
String Edit::Mask() const
{
   size_type len = 0;
   API_Edit_GetEditMask( handle, 0, &len );

   String mask;
   if ( len > 0 )
   {
      mask.SetLength( len );
      if ( API_Edit_GetEditMask( handle, mask.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetEditMask" );
      mask.ResizeToNullTerminated();
   }
   return mask;
}

// ----------------------------------------------------------------------------

void Edit::SetMask( const String& mask )
{
   API_Edit_SetEditMask( handle, mask.c_str() );
}

// ----------------------------------------------------------------------------

bool Edit::IsValid() const
{
   return true; // ### TODO
}
*/
// ----------------------------------------------------------------------------

void Edit::SelectAll( bool select )
{
   API_Edit_SetEditSelected( handle, select );
}

// ----------------------------------------------------------------------------

void Edit::GetSelection( int& selStart, int& selEnd ) const
{
   selStart = selEnd = 0;
   API_Edit_GetEditSelection( handle, &selStart, &selEnd );
}

// ----------------------------------------------------------------------------

void Edit::SetSelection( int selStart, int selEnd )
{
   API_Edit_SetEditSelection( handle, selStart, selEnd );
}

// ----------------------------------------------------------------------------

String Edit::SelectedText() const
{
   size_type len = 0;
   API_Edit_GetEditSelectedText( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_Edit_GetEditSelectedText( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetEditSelectedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

int Edit::CaretPosition() const
{
   return API_Edit_GetEditCaretPosition( handle );
}

// ----------------------------------------------------------------------------

void Edit::SetCaretPosition( int pos )
{
   API_Edit_SetEditCaretPosition( handle, pos );
}

// ----------------------------------------------------------------------------

bool Edit::IsRightAligned() const
{
   return API_Edit_GetEditAlignment( handle ) == TextAlign::Right;
}

// ----------------------------------------------------------------------------

void Edit::SetRightAlignment( bool right )
{
   API_Edit_SetEditAlignment( handle, right ? TextAlign::Right : TextAlign::Left );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Edit*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class EditEventDispatcher
{
public:

   static void api_func EditCompleted( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onEditCompleted != nullptr )
         (receiver->*handlers->onEditCompleted)( *sender );
   }

   static void api_func ReturnPressed( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onReturnPressed != nullptr )
         (receiver->*handlers->onReturnPressed)( *sender );
   }

   static void api_func TextUpdated( control_handle hSender, control_handle hReceiver, const char16_type* text )
   {
      if ( handlers->onTextUpdated != nullptr )
      {
         String s( text );
         (receiver->*handlers->onTextUpdated)( *sender, s );
      }
   }

   static void api_func CaretPositionUpdated( control_handle hSender, control_handle hReceiver, int32 oldPos, int32 newPos )
   {
      if ( handlers->onCaretPositionUpdated != nullptr )
         (receiver->*handlers->onCaretPositionUpdated)( *sender, oldPos, newPos );
   }

   static void api_func SelectionUpdated( control_handle hSender, control_handle hReceiver, int32 newStart, int32 newEnd )
   {
      if ( handlers->onSelectionUpdated != nullptr )
         (receiver->*handlers->onSelectionUpdated)( *sender, newStart, newEnd );
   }
}; // EditEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void Edit::OnEditCompleted( edit_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Edit_SetEditCompletedEventRoutine( handle, &receiver,
                  (f != nullptr) ? EditEventDispatcher::EditCompleted : nullptr ) == api_false )
      throw APIFunctionError( "SetEditCompletedEventRoutine" );
   m_handlers->onEditCompleted = f;
}

void Edit::OnReturnPressed( edit_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Edit_SetReturnPressedEventRoutine( handle, &receiver,
                  (f != nullptr) ? EditEventDispatcher::ReturnPressed : nullptr ) == api_false )
      throw APIFunctionError( "SetReturnPressedEventRoutine" );
   m_handlers->onReturnPressed = f;
}

void Edit::OnTextUpdated( text_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Edit_SetTextUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? EditEventDispatcher::TextUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextUpdatedEventRoutine" );
   m_handlers->onTextUpdated = f;
}

void Edit::OnCaretPositionUpdated( caret_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Edit_SetCaretPositionUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? EditEventDispatcher::CaretPositionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetCaretPositionUpdatedEventRoutine" );
   m_handlers->onCaretPositionUpdated = f;
}

void Edit::OnSelectionUpdated( selection_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_Edit_SetSelectionUpdatedEventRoutine( handle, &receiver,
                  (f != nullptr) ? EditEventDispatcher::SelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetSelectionUpdatedEventRoutine" );
   m_handlers->onSelectionUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Edit.cpp - Released 2025-04-07T08:53:32Z
