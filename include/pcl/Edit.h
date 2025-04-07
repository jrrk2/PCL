//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Edit.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Edit_h
#define __PCL_Edit_h

/// \file pcl/Edit.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Frame.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Edit
 * \brief Client-side interface to a PixInsight %Edit control
 *
 * %Edit controls are single-line plain text edit controls.
 */
class PCL_CLASS Edit : public Frame
{
public:

   /*!
    * Constructs an %Edit object with the specified \a text, as a child control
    * of \a parent.
    */
   Edit( const String& text = String(), Control& parent = Control::Null() );

   /*!
    * Destroys an %Edit control.
    */
   ~Edit() override
   {
   }

   /*!
    * Returns the current edit text.
    */
   String Text() const;

   /*!
    * Sets the current button text.
    */
   void SetText( const String& );

   /*!
    * Removes all the text in this %edit control.
    */
   void Clear()
   {
      SetText( String() );
   }

   /*!
    * Returns true iff this %Edit control is in read-only mode.
    */
   bool IsReadOnly() const;

   /*!
    * Enables or disables the read-only mode for this %Edit control.
    */
   void SetReadOnly( bool = true );

   /*!
    * Disables or enables the read-only mode for this %Edit control.
    *
    * This is a convenience member function, equivalent to:
    * SetReadOnly( !rw )
    */
   void SetReadWrite( bool rw = true )
   {
      SetReadOnly( !rw );
   }

   /*!
    * Returns true iff the text in this %Edit control has been modified.
    */
   bool IsModified() const;

   /*!
    * Sets or clears the modified status for this %Edit control.
    */
   void SetModified( bool = true );

   /*!
    * Clears or sets the modified status for this %Edit control.
    *
    * This is a convenience member function, equivalent to:
    * SetModified( !clear )
    */
   void ClearModified( bool clear = true )
   {
      SetModified( !clear );
   }

   /*!
    * Returns true iff this %Edit control is in <em>password display mode</em>.
    *
    * In password mode, the text in an edit control is always displayed as a
    * sequence of asterisks. This does not affect the actual text; the password
    * mode only changes the way characters are displayed.
    *
    * \sa EnablePasswordMode(), DisablePasswordMode()
    */
   bool IsPasswordMode() const;

   /*!
    * Enables the password display mode for this %Edit control.
    *
    * \sa IsPasswordMode(), DisablePasswordMode()
    */
   void EnablePasswordMode( bool = true );

   /*!
    * Disables the password display mode for this %Edit control.
    *
    * This is a convenience member function, equivalent to:
    * EnablePasswordMode( !disable )
    *
    * \sa IsPasswordMode(), EnablePasswordMode()
    */
   void DisablePasswordMode( bool disable = true )
   {
      EnablePasswordMode( !disable );
   }

   /*!
    * Returns the current regular expression used for validation of text input
    * in this %Edit control, or an empty string if no validating regular
    * expression has been defined.
    *
    * See SetValidatingRegExp() for detailed information.
    */
   String ValidatingRegExp() const;

   /*!
    * Returns the current regular expression used for validation of text input
    * in this %Edit control, or an empty string if no validating regular
    * expression has been defined.
    *
    * On exit, the specified \a caseSensitive variable will be \c true if the
    * current regular expression is being used to perform case-sensitive
    * matches; \c false if case-insensitive matches are performed.
    *
    * See SetValidatingRegExp() for detailed information.
    */
   String ValidatingRegExp( bool& caseSensitive ) const;

   /*!
    * Sets a validating regular expression for this %Edit control.
    *
    * The specified regular expression \a rx will be used to accept or reject
    * characters automatically as they are being typed in the control, and also
    * to accept or reject text pasted from the clipboard or dropped on the
    * control by the user.
    *
    * For example, to ensure that a syntactically valid floating point number
    * is entered, the following regular expression can be used:
    *
    * \code
    * Edit velocity_Edit;
    * ...
    * velocity_Edit.SetValidatingRegExp( "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?" );
    * \endcode
    *
    * If the \a caseSensitive parameter is \c true, case-sensitive matches will
    * be performed with the regular expression; otherwise the matches will
    * ignore case. Case-sensitive matches are performed by default.
    *
    * This member function throws an Error exception if the specified regular
    * expression is not an empty string and is not valid or is unsupported.
    *
    * To disable regular expression validation, specify an empty string as the
    * \a rx argument, or call DisableValidatingRegExp().
    *
    * \sa ValidatingRegExp(), IsValid()
    */
   void SetValidatingRegExp( const String& rx, bool caseSensitive = true );

   /*!
    * Disables validation through a regular expression for this %Edit control.
    *
    * Calling this function is equivalent to:
    *
    * \code
    * SetValidatingRegExp( String() );
    * \endcode
    */
   void DisableValidatingRegExp()
   {
      SetValidatingRegExp( String() );
   }

   /*!
    * Returns \c true if the text in this %Edit control matches the current
    * validating regular expression, or if no validating regular expression is
    * currently defined for this control. Returns \c false if a validating
    * regular expression is defined and the current text does not match it.
    *
    * See SetValidatingRegExp() for complete information on validating regular
    * expressions.
    */
   bool IsValid() const;

   /*!
    * Returns the maximum allowed length for the text in this %Edit control.
    */
   int MaxLength() const;

   /*!
    * Sets the maximum allowed length for the text in this %Edit control.
    *
    * The default length limit is \c int_max, which in practice represents no
    * length limit.
    */
   void SetMaxLength( int );

   /*!
    * Enables unlimited length for the text in this %Edit control.
    *
    * This is a convenience member function, equivalent to:
    * SetMaxLength( int_max )
    */
   void SetUnlimitedLength()
   {
      SetMaxLength( int_max );
   }

   /*!
    * Selects all the text in this %edit control.
    */
   void SelectAll( bool = true );

   /*!
    * Clears the existing selection, if any, in this %edit control.
    *
    * \note This member function does not delete the selected text; it only
    * removes the selection.
    */
   void Unselect( bool unsel = true )
   {
      SelectAll( !unsel );
   }

   /*!
    * Gets the limits of the current selection in this %Edit control.
    *
    * \param[out] selStart   The index of the first selected character.
    * \param[out] selEnd     The index of the last selected character plus one.
    *
    * When \a selStart is equal to \a selEnd, there is no selection in this
    * edit control.
    */
   void GetSelection( int& selStart, int& selEnd ) const;

   /*!
    * Sets the limits of the current selection in this %Edit control.
    *
    * \param selStart   The index of the first selected character.
    * \param selEnd     The index of the last selected character plus one.
    *
    * When \a selStart is equal to \a selEnd, there is no selection in this
    * edit control.
    */
   void SetSelection( int selStart, int selEnd );

   /*!
    * Returns true iff there are one or more selected characters in this %Edit
    * control.
    */
   bool HasSelection() const
   {
      int s0, s1;
      GetSelection( s0, s1 );
      return s0 != s1;
   }

   /*!
    * Returns the currently selected text, or an empty string if there is no
    * selection in this %Edit control.
    */
   String SelectedText() const;

   /*!
    * Returns the current caret position in this %Edit control.
    */
   int CaretPosition() const;

   /*!
    * Sets the current caret position in this %Edit control.
    */
   void SetCaretPosition( int );

   /*!
    * Moves the caret to the beginning of the text line in this %Edit control.
    */
   void Home()
   {
      SetCaretPosition( 0 );
   }

   /*!
    * Moves the caret to the end of the text line in this %Edit control.
    */
   void End()
   {
      SetCaretPosition( int_max );
   }

   /*!
    * Returns true iff the text in this %Edit control is right-aligned.
    */
   bool IsRightAligned() const;

   /*!
    * Returns true iff the text in this %Edit control is left-aligned.
    */
   bool IsLeftAligned() const
   {
      return !IsRightAligned();
   }

   /*!
    * Enables or disables right-alignment for the text in this %edit control.
    */
   void SetRightAlignment( bool right = true );

   /*!
    * Disables or enables right-alignment for the text in this %edit control.
    *
    * This is a convenience member function, equivalent to:
    * SetRightAlignment( !left )
    */
   void SetLeftAlignment( bool left = true )
   {
      SetRightAlignment( !left );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnEditCompleted( Edit& sender );
   // void OnReturnPressed( Edit& sender );
   // void OnTextUpdated( Edit& sender, const String& text );
   // void OnCaretPositionUpdated( Edit& sender, int oldPos, int newPos );
   // void OnSelectionUpdated( Edit& sender, int newStart, int newEnd );

   /*! #
    */
   using edit_event_handler = void (Control::*)( Edit& sender );

   /*! #
    */
   using text_event_handler = void (Control::*)( Edit& sender, const String& );

   /*! #
    */
   using caret_event_handler = void (Control::*)( Edit& sender, int oldPos, int newPos );

   /*! #
    */
   using selection_event_handler = void (Control::*)( Edit& sender, int newStart, int newEnd );

   /*! #
    */
   void OnEditCompleted( edit_event_handler, Control& );

   /*! #
    */
   void OnReturnPressed( edit_event_handler, Control& );

   /*! #
    */
   void OnTextUpdated( text_event_handler, Control& );

   /*! #
    */
   void OnCaretPositionUpdated( caret_event_handler, Control& );

   /*! #
    */
   void OnSelectionUpdated( selection_event_handler, Control& );

private:

   struct EventHandlers
   {
      edit_event_handler      onEditCompleted        = nullptr;
      edit_event_handler      onReturnPressed        = nullptr;
      text_event_handler      onTextUpdated          = nullptr;
      caret_event_handler     onCaretPositionUpdated = nullptr;
      selection_event_handler onSelectionUpdated     = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class EditEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_Edit_h

// ----------------------------------------------------------------------------
// EOF pcl/Edit.h - Released 2025-04-07T08:52:44Z
