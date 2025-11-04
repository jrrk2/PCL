//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CodeEditor.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/CodeEditor.h>
#include <pcl/MetaModule.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CodeEditor::CodeEditor( Control& parent )
   : Control( API_CodeEditor_CreateCodeEditor( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
   , m_lineNumbers( nullptr )
{
   if ( handle == nullptr )
      throw APIFunctionError( "CreateCodeEditor" );
}

// ----------------------------------------------------------------------------

Control& CodeEditor::LineNumbersControl()
{
   if ( m_lineNumbers.IsNull() )
   {
      m_lineNumbers.TransferHandle( API_CodeEditor_CreateEditorLineNumbersControl( handle, &m_lineNumbers, handle, 0/*flags*/ ) );
      if ( m_lineNumbers.IsNull() )
         throw APIFunctionError( "CreateEditorLineNumbersControl" );
   }
   return m_lineNumbers;
}

// ----------------------------------------------------------------------------

String CodeEditor::FilePath() const
{
   size_type len = 0;
   API_CodeEditor_GetEditorFilePath( handle, 0, &len );

   String path;
   if ( len > 0 )
   {
      path.SetLength( len );
      if ( API_CodeEditor_GetEditorFilePath( handle, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetEditorFilePath" );
      path.ResizeToNullTerminated();
   }
   return path;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetFilePath( const String& filePath )
{
   API_CodeEditor_SetEditorFilePath( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

String CodeEditor::Text() const
{
   size_type len = 0;
   API_CodeEditor_GetEditorText( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_CodeEditor_GetEditorText( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetEditorText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetText( const String& text )
{
   API_CodeEditor_SetEditorText( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

IsoString CodeEditor::EncodedText( const IsoString& encoding ) const
{
   size_type len = 0;
   API_CodeEditor_GetEditorEncodedText( handle, 0, &len, encoding.c_str() );

   IsoString text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_CodeEditor_GetEditorEncodedText( handle, text.Begin(), &len, encoding.c_str() ) == api_false )
         throw APIFunctionError( "GetEditorEncodedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetEncodedText( const IsoString& text, const IsoString& encoding )
{
   if ( API_CodeEditor_SetEditorEncodedText( handle, text.c_str(), encoding.c_str() ) == api_false )
      throw APIFunctionError( "SetEditorEncodedText" );
}

// ----------------------------------------------------------------------------

void CodeEditor::ClearText()
{
   API_CodeEditor_ClearEditorText( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsReadOnly() const
{
   return API_CodeEditor_GetEditorReadOnly( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetReadOnly( bool readOnly )
{
   API_CodeEditor_SetEditorReadOnly( handle, readOnly );
}

// ----------------------------------------------------------------------------

bool CodeEditor::Save( const String& filePath, const IsoString& encoding )
{
   return API_CodeEditor_SaveEditorText( handle, filePath.c_str(), encoding.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool CodeEditor::Load( const String& filePath, const IsoString& encoding )
{
   return API_CodeEditor_LoadEditorText( handle, filePath.c_str(), encoding.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

int CodeEditor::NumberOfLines() const
{
   return API_CodeEditor_GetEditorLineCount( handle );
}

// ----------------------------------------------------------------------------

int CodeEditor::NumberOfCharacters() const
{
   return API_CodeEditor_GetEditorCharacterCount( handle );
}

// ----------------------------------------------------------------------------

Point CodeEditor::CursorPosition() const
{
   Point pos;
   API_CodeEditor_GetEditorCursorCoordinates( handle, &pos.y, &pos.x );
   return pos;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetCursorPosition( int line, int column )
{
   API_CodeEditor_SetEditorCursorCoordinates( handle, line, column );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsInsertMode() const
{
   return API_CodeEditor_GetEditorInsertMode( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetInsertMode( bool insert )
{
   API_CodeEditor_SetEditorInsertMode( handle, insert );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsBlockSelectionMode() const
{
   return API_CodeEditor_GetEditorBlockSelectionMode( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetBlockSelectionMode( bool blockMode )
{
   API_CodeEditor_SetEditorBlockSelectionMode( handle, blockMode );
}

// ----------------------------------------------------------------------------

bool CodeEditor::IsDynamicWordWrapMode() const
{
   return API_CodeEditor_GetEditorDynamicWordWrapMode( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetDynamicWordWrapMode( bool wrapMode )
{
   API_CodeEditor_SetEditorDynamicWordWrapMode( handle, wrapMode );
}

// ----------------------------------------------------------------------------

int CodeEditor::UndoSteps() const
{
   return API_CodeEditor_GetEditorUndoSteps( handle );
}

// ----------------------------------------------------------------------------

int CodeEditor::RedoSteps() const
{
   return API_CodeEditor_GetEditorRedoSteps( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::HasSelection() const
{
   return API_CodeEditor_GetEditorHasSelection( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String CodeEditor::SelectedText() const
{
   size_type len = 0;
   API_CodeEditor_GetEditorSelectedText( handle, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( API_CodeEditor_GetEditorSelectedText( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetEditorSelectedText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

Rect CodeEditor::Selection() const
{
   Rect r;
   API_CodeEditor_GetEditorSelectionCoordinates( handle, &r.y0, &r.x0, &r.y1, &r.x1 );
   return r;
}

// ----------------------------------------------------------------------------

void CodeEditor::SetSelection( int fromLine, int fromCol, int toLine, int toCol )
{
   API_CodeEditor_SetEditorSelectionCoordinates( handle, fromLine, fromCol, toLine, toCol );
}

// ----------------------------------------------------------------------------

void CodeEditor::InsertText( const String& text )
{
   API_CodeEditor_InsertEditorText( handle, text.c_str() );
}

// ----------------------------------------------------------------------------

void CodeEditor::Undo()
{
   API_CodeEditor_EditorUndo( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Redo()
{
   API_CodeEditor_EditorRedo( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Cut()
{
   API_CodeEditor_EditorCut( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Copy()
{
   API_CodeEditor_EditorCopy( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Paste()
{
   API_CodeEditor_EditorPaste( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Delete()
{
   API_CodeEditor_EditorDelete( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::SelectAll()
{
   API_CodeEditor_EditorSelectAll( handle );
}

// ----------------------------------------------------------------------------

void CodeEditor::Unselect()
{
   API_CodeEditor_EditorUnselect( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::GotoMatchedParenthesis()
{
   return API_CodeEditor_EditorGotoMatchedParenthesis( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int CodeEditor::HighlightAllMatches( const String& toFind, FindModes mode )
{
   return API_CodeEditor_EditorHighlightAllMatches( handle, toFind.c_str(), unsigned( mode ) );
}

// ----------------------------------------------------------------------------

void CodeEditor::ClearMatches()
{
   API_CodeEditor_EditorClearMatches( handle );
}

// ----------------------------------------------------------------------------

bool CodeEditor::Find( const String& toFind, FindModes mode )
{
   return API_CodeEditor_EditorFind( handle, toFind.c_str(), unsigned( mode ) ) != api_false;
}

// ----------------------------------------------------------------------------

bool CodeEditor::ReplaceSelection( const String& replaceWith )
{
   return API_CodeEditor_EditorReplace( handle, replaceWith.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

int CodeEditor::ReplaceAll( const String& toFind, const String& replaceWith, FindModes mode )
{
   return API_CodeEditor_EditorReplaceAll( handle, toFind.c_str(), replaceWith.c_str(), unsigned( mode ) );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<CodeEditor*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class CodeEditorEventDispatcher
{
public:

   static void TextUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onTextUpdated != nullptr )
         (receiver->*handlers->onTextUpdated)( *sender );
   }

   static void CursorPositionUpdated( control_handle hSender, control_handle hReceiver, int32 line, int32 col )
   {
      if ( handlers->onCursorPositionUpdated != nullptr )
         (receiver->*handlers->onCursorPositionUpdated)( *sender, line, col );
   }

   static void SelectionUpdated( control_handle hSender, control_handle hReceiver, int32 fromLine, int32 fromCol, int32 toLine, int32 toCol )
   {
      if ( handlers->onSelectionUpdated != nullptr )
         (receiver->*handlers->onSelectionUpdated)( *sender, fromLine, fromCol, toLine, toCol );
   }

   static void OverwriteModeUpdated( control_handle hSender, control_handle hReceiver, api_bool overwrite )
   {
      if ( handlers->onOverwriteModeUpdated != nullptr )
         (receiver->*handlers->onOverwriteModeUpdated)( *sender, overwrite != api_false );
   }

   static void SelectionModeUpdated( control_handle hSender, control_handle hReceiver, api_bool blockMode )
   {
      if ( handlers->onSelectionModeUpdated != nullptr )
         (receiver->*handlers->onSelectionModeUpdated)( *sender, blockMode != api_false );
   }

   static void DynamicWordWrapModeUpdated( control_handle hSender, control_handle hReceiver, api_bool wordWrap )
   {
      if ( handlers->onDynamicWordWrapModeUpdated != nullptr )
         (receiver->*handlers->onDynamicWordWrapModeUpdated)( *sender, wordWrap != api_false );
   }

}; // CodeEditorEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void CodeEditor::OnTextUpdated( editor_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_CodeEditor_SetEditorTextUpdatedEventRoutine( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::TextUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorTextUpdatedEventRoutine" );
   m_handlers->onTextUpdated = handler;
}

void CodeEditor::OnCursorPositionUpdated( cursor_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_CodeEditor_SetEditorCursorPositionUpdatedEventRoutine( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::CursorPositionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorCursorPositionUpdatedEventRoutine" );
   m_handlers->onCursorPositionUpdated = handler;
}

void CodeEditor::OnSelectionUpdated( selection_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_CodeEditor_SetEditorSelectionUpdatedEventRoutine( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::SelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorSelectionUpdatedEventRoutine" );
   m_handlers->onSelectionUpdated = handler;
}

void CodeEditor::OnOverwriteModeUpdated( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_CodeEditor_SetEditorOverwriteModeUpdatedEventRoutine( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::OverwriteModeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorOverwriteModeUpdatedEventRoutine" );
   m_handlers->onOverwriteModeUpdated = handler;
}

void CodeEditor::OnSelectionModeUpdated( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_CodeEditor_SetEditorSelectionModeUpdatedEventRoutine( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::SelectionModeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorSelectionModeUpdatedEventRoutine" );
   m_handlers->onSelectionModeUpdated = handler;
}

void CodeEditor::OnDynamicWordWrapModeUpdated( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_CodeEditor_SetEditorDynamicWordWrapModeUpdatedEventRoutine( handle, &receiver,
                  (handler != nullptr) ? CodeEditorEventDispatcher::DynamicWordWrapModeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetEditorDynamicWordWrapModeUpdatedEventRoutine" );
   m_handlers->onDynamicWordWrapModeUpdated = handler;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CodeEditor.cpp - Released 2025-04-07T08:53:32Z
