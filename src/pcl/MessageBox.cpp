//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MessageBox.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Exception.h>
#include <pcl/MessageBox.h>

#include <pcl/api/APIInterface.h>

#ifdef __PCL_WINDOWS
#  include <windows.h>
#else
#  include <stdlib.h> // for system()
#  include <unistd.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class MessageBoxPrivate
{
   MessageBoxPrivate( const String&           aText,
                      const String&           aCaption,
                      MessageBox::std_icon    aIcon,
                      MessageBox::std_button  aButton0,
                      MessageBox::std_button  aButton1,
                      MessageBox::std_button  aButton2,
                      int                     aDefaultButton,
                      int                     aEscapeButton )
      : text( aText )
      , caption( aCaption )
      , icon( aIcon )
      , button0( aButton0 )
      , button1( aButton1 )
      , button2( aButton2 )
      , defaultButton( aDefaultButton )
      , escapeButton( aEscapeButton )
      , result( StdButton::NoButton )
   {
   }

   String                  text;
   String                  caption;
   MessageBox::std_icon    icon;
   MessageBox::std_button  button0, button1, button2;
   int                     defaultButton, escapeButton;
   MessageBox::std_button  result;

   friend class MessageBox;
};

// ----------------------------------------------------------------------------

MessageBox::MessageBox( const String& text,
                        const String& caption,
                        std_icon      icon,
                        std_button    button0,
                        std_button    button1,
                        std_button    button2,
                        int           defaultButton,
                        int           escapeButton )
{
   p = new MessageBoxPrivate( text, caption, icon, button0, button1, button2, defaultButton, escapeButton );
}

MessageBox::~MessageBox()
{
}

// ----------------------------------------------------------------------------

MessageBox::std_button MessageBox::Execute()
{
   if ( API == nullptr )
   {
      // Errors occurring before API initialization.

#ifdef __PCL_WINDOWS
      ::MessageBoxW( 0, (LPCWSTR)p->text.c_str(), (LPCWSTR)p->caption.c_str(), MB_OK|MB_ICONERROR|MB_TASKMODAL );
#else

      IsoString cmd( "xmessage -buttons Ok:0 -default Ok -center " );

      cmd.Append( '\"' );

      if ( !p->caption.IsEmpty() )
      {
         cmd.Append( IsoString( p->caption ) );
         cmd.Append( ": " );
      }

      if ( !p->text.IsEmpty() )
         cmd.Append( IsoString( p->text ) );

      cmd.Append( '\"' );

      PCL_UNUSED( system( cmd.c_str() ) );

#endif   // __PCL_WINDOWS

      return StdButton::Ok;
   }

   // Request a PixInsight MessageBox primitive.

   return std_button( (*API->Global->MessageBox)( p->text.c_str(), p->caption.c_str(),
                                                  p->button0, p->button1, p->button2,
                                                  p->defaultButton, p->escapeButton,
                                                  p->icon ) );
}

// ----------------------------------------------------------------------------

MessageBox::std_button MessageBox::Result() const
{
   return p->result;
}

// ----------------------------------------------------------------------------

String MessageBox::Text() const
{
   return p->text;
}

void MessageBox::SetText( const String& text )
{
   p->text = text;
}

String MessageBox::Caption() const
{
   return p->caption;
}

void MessageBox::SetCaption( const String& caption )
{
   p->caption = caption;
}

MessageBox::std_icon MessageBox::Icon() const
{
   return p->icon;
}

void MessageBox::SetIcon( MessageBox::std_icon icon )
{
   p->icon = icon;
}

MessageBox::std_button MessageBox::FirstButton() const
{
   return p->button0;
}

void MessageBox::SetFirstButton( MessageBox::std_button button )
{
   p->button0 = button;
}

MessageBox::std_button MessageBox::SecondButton() const
{
   return p->button1;
}

void MessageBox::SetSecondButton( MessageBox::std_button button )
{
   p->button1 = button;
}

MessageBox::std_button MessageBox::ThirdButton() const
{
   return p->button2;
}

void MessageBox::SetThirdButton( MessageBox::std_button button )
{
   p->button2 = button;
}

int MessageBox::DefaultButtonIndex() const
{
   return p->defaultButton;
}

void MessageBox::SetDefaultButtonIndex( int index )
{
   p->defaultButton = index;
}

int MessageBox::EscapeButtonIndex() const
{
   return p->escapeButton;
}

void MessageBox::SetEscapeButtonIndex( int index )
{
   p->escapeButton = index;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MessageBox.cpp - Released 2025-04-07T08:53:32Z
