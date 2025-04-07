//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/TextBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/TextBox.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TextBox::TextBox( const String& text, Control& parent )
   : Frame( (*API->TextBox->CreateTextBox)( ModuleHandle(), this, text.c_str(), parent.handle, 0/*flags*/ ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTextBox" );
}

// ----------------------------------------------------------------------------

String TextBox::Text() const
{
   size_type len = 0;
   (*API->TextBox->GetTextBoxText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->TextBox->GetTextBoxText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTextBoxText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void TextBox::SetText( const String& text )
{
   (*API->TextBox->SetTextBoxText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

bool TextBox::IsReadOnly() const
{
   return (*API->TextBox->GetTextBoxReadOnly)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TextBox::SetReadOnly( bool readOnly )
{
   (*API->TextBox->SetTextBoxReadOnly)( handle, readOnly );
}

// ----------------------------------------------------------------------------

void TextBox::SelectAll( bool select )
{
   (*API->TextBox->SetTextBoxSelected)( handle, select );
}

// ----------------------------------------------------------------------------

void TextBox::GetSelection( int& selStart, int& selEnd ) const
{
   selStart = selEnd = 0;
   (*API->TextBox->GetTextBoxSelection)( handle, &selStart, &selEnd );
}

// ----------------------------------------------------------------------------

void TextBox::SetSelection( int selStart, int selEnd )
{
   (*API->TextBox->SetTextBoxSelection)( handle, selStart, selEnd );
}

// ----------------------------------------------------------------------------

String TextBox::SelectedText() const
{
   size_type len = 0;
   (*API->TextBox->GetTextBoxSelectedText)( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->TextBox->GetTextBoxSelectedText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTextBoxSelectedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

int TextBox::CaretPosition() const
{
   return (*API->TextBox->GetTextBoxCaretPosition)( handle );
}

// ----------------------------------------------------------------------------

void TextBox::SetCaretPosition( int pos )
{
   (*API->TextBox->SetTextBoxCaretPosition)( handle, pos );
}

// ----------------------------------------------------------------------------

void TextBox::Insert( const String& text )
{
   (*API->TextBox->InsertTextBoxText)( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

void TextBox::Delete()
{
   (*API->TextBox->DeleteTextBoxText)( handle );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<TextBox*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class TextBoxEventDispatcher
{
public:

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
}; // TextBoxEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void TextBox::OnTextUpdated( unicode_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TextBox->SetTextBoxUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TextBoxEventDispatcher::TextUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextBoxUpdatedEventRoutine" );
   m_handlers->onTextUpdated = f;
}

void TextBox::OnCaretPositionUpdated( caret_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TextBox->SetTextBoxCaretPositionUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TextBoxEventDispatcher::CaretPositionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextBoxCaretPositionUpdatedEventRoutine" );
   m_handlers->onCaretPositionUpdated = f;
}

void TextBox::OnSelectionUpdated( selection_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TextBox->SetTextBoxSelectionUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TextBoxEventDispatcher::SelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTextBoxSelectionUpdatedEventRoutine" );
   m_handlers->onSelectionUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/TextBox.cpp - Released 2025-04-07T08:53:32Z
