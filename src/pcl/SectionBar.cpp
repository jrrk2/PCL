//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SectionBar.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/GlobalSettings.h>
#include <pcl/MetaModule.h> // MetaModule::ProcessEvents()
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>

#define contract_icon ":/process-interface/contract-vert.png"
#define expand_icon   ":/process-interface/expand-vert.png"

namespace pcl
{

// ----------------------------------------------------------------------------

SectionBar::SectionBar( Control& parent )
   : Control( parent )
{
   SetObjectId( "IWSectionBar" );

   SetFocusStyle( FocusStyle::NoFocus );

   SetSizer( Global_Sizer );

   Global_Sizer.AddSpacing( 1 );
   Global_Sizer.Add( Title_Sizer );
   Global_Sizer.AddSpacing( 1 );

   Title_Sizer.AddSpacing( 4 );
   Title_Sizer.Add( Title_Label );
   Title_Sizer.AddStretch();
   Title_Sizer.Add( Title_ToolButton );
   Title_Sizer.AddSpacing( 4 );

   Title_Label.SetText( String( 'M' ) );
   Title_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   Title_Label.SetText( String() );

   Title_ToolButton.SetIcon( Bitmap( ScaledResource( contract_icon ) ) );
   Title_ToolButton.SetScaledFixedSize( 17, 17 );
   Title_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Title_ToolButton.OnClick( (Button::click_event_handler)&SectionBar::ButtonClick, *this );

   AdjustToContents();
   SetFixedHeight();

   OnMousePress( (Control::mouse_button_event_handler)&SectionBar::MousePress, *this );
   OnShow( (Control::event_handler)&SectionBar::ControlShow, *this );
}

// ----------------------------------------------------------------------------

SectionBar::~SectionBar()
{
   if ( m_section != nullptr )
   {
      m_section->OnShow( nullptr, Control::Null() );
      m_section->OnHide( nullptr, Control::Null() );
      m_section = nullptr;
   }
}

// ----------------------------------------------------------------------------

void SectionBar::SetSection( Control& section )
{
   if ( m_section != nullptr )
   {
      m_section->OnShow( nullptr, Control::Null() );
      m_section->OnHide( nullptr, Control::Null() );
      m_section = nullptr;
   }

   if ( section.IsNull() )
      return;

   m_section = &section;
   m_section->OnShow( (Control::event_handler)&SectionBar::ControlShow, *this );
   m_section->OnHide( (Control::event_handler)&SectionBar::ControlHide, *this );

   Title_ToolButton.SetIcon( Bitmap( ScaledResource( m_section->IsVisible() ? contract_icon : expand_icon ) ) );
}

// ----------------------------------------------------------------------------

void SectionBar::EnableTitleCheckBox( bool enable )
{
   if ( Title_CheckBox.IsNull() == enable )
   {
      if ( Title_CheckBox.IsNull() )
      {
         Title_CheckBox = new CheckBox;
         Title_CheckBox->SetFocusStyle( FocusStyle::NoFocus );
         Title_CheckBox->SetChecked();
         Title_CheckBox->OnClick( (Button::click_event_handler)&SectionBar::ButtonClick, *this );
         Title_Sizer.Insert( 1, *Title_CheckBox );
         Title_Sizer.InsertSpacing( 2, 2 );
      }
      else
      {
         if ( !m_handlers.IsNull() )
         {
            m_handlers->onCheck = nullptr;
            m_handlers->onCheckReceiver = nullptr;
         }
         Title_CheckBox.Destroy();
      }

      if ( m_section != nullptr )
         m_section->Enable();
   }
}

// ----------------------------------------------------------------------------

void SectionBar::SetChecked( bool checked )
{
   if ( !Title_CheckBox.IsNull() )
   {
      if ( m_section != nullptr )
         m_section->Enable( checked );
      Title_CheckBox->SetChecked( checked );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::Enable( bool enabled )
{
   Title_Label.Enable( enabled );

   if ( !Title_CheckBox.IsNull() )
      Title_CheckBox->Enable( enabled );

   if ( m_section != nullptr )
      m_section->Enable( enabled && (Title_CheckBox.IsNull() || Title_CheckBox->IsChecked()) );
}

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void SectionBar::OnToggleSection( section_event_handler handler, Control& receiver )
{
   if ( handler == nullptr || receiver.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onToggleSection = nullptr;
         m_handlers->onToggleSectionReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onToggleSection = handler;
      m_handlers->onToggleSectionReceiver = &receiver;
   }
}

void SectionBar::OnCheck( check_event_handler handler, Control& receiver )
{
   if ( Title_CheckBox.IsNull() || handler == nullptr || receiver.IsNull() )
   {
      if ( !m_handlers.IsNull() )
      {
         m_handlers->onCheck = nullptr;
         m_handlers->onCheckReceiver = nullptr;
      }
   }
   else
   {
      INIT_EVENT_HANDLERS();
      m_handlers->onCheck = handler;
      m_handlers->onCheckReceiver = &receiver;
   }
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

void SectionBar::SetSectionVisible( bool visible )
{
   if ( m_section != nullptr )
      if ( visible != m_section->IsVisible() )
      {
         Control* parent = &m_section->Parent();
         if ( !parent->IsNull() )
         {
            while ( !parent->Parent().IsNull() )
               parent = &parent->Parent();
            parent->DisableUpdates();
         }

         m_section->SetVisible( visible );

         /*
          * On macOS, forcing event processing here causes a lot of flickering.
          * On X11 and Windows, it's just the opposite... go figure!
          */
#ifndef __PCL_MACOSX
         Module->ProcessEvents();
#endif
         if ( !parent->IsNull() )
         {
            bool fixedWidth = parent->IsFixedWidth();
            if ( !fixedWidth )
               parent->SetFixedWidth();

            bool fixedHeight = parent->IsFixedHeight();
            if ( fixedHeight )
               parent->SetVariableHeight();

            parent->AdjustToContents();

            Module->ProcessEvents();

            if ( !fixedWidth )
               parent->SetVariableWidth();
            if ( fixedHeight )
               parent->SetFixedHeight();

            parent->EnableUpdates();
         }
      }
}

// ----------------------------------------------------------------------------

void SectionBar::ButtonClick( Button& sender, bool checked )
{
   if ( m_section != nullptr && sender == Title_ToolButton )
   {
      if ( !m_handlers.IsNull() )
         if ( m_handlers->onToggleSection != nullptr )
            (m_handlers->onToggleSectionReceiver->*m_handlers->onToggleSection)( *this, *m_section, true );

      SetSectionVisible( !m_section->IsVisible() );

      if ( !m_handlers.IsNull() )
         if ( m_handlers->onToggleSection != nullptr )
            (m_handlers->onToggleSectionReceiver->*m_handlers->onToggleSection)( *this, *m_section, false );
   }
   else if ( !Title_CheckBox.IsNull() && sender == *Title_CheckBox )
   {
      if ( m_section != nullptr )
         m_section->Enable( checked );

      if ( !m_handlers.IsNull() )
         if ( m_handlers->onCheck != nullptr )
            (m_handlers->onCheckReceiver->*m_handlers->onCheck)( *this, checked );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::MousePress( Control& /*sender*/, const pcl::Point& /*pos*/,
                             int mouseButton, unsigned /*buttons*/, unsigned /*modifiers*/ )
{
   if ( mouseButton == MouseButton::Left )
      ButtonClick( Title_ToolButton, false );
}

// ----------------------------------------------------------------------------

void SectionBar::ControlShow( Control& sender )
{
   if ( m_section != nullptr )
   {
      const char* iconResource = nullptr;
      if ( sender == *m_section )
         iconResource = contract_icon;
      else if ( sender == *this )
      {
         if ( m_section->IsVisible() )
            iconResource = contract_icon;
         else
            iconResource = expand_icon;
      }
      if ( iconResource != nullptr )
         Title_ToolButton.SetIcon( Bitmap( ScaledResource( iconResource ) ) );
   }
}

// ----------------------------------------------------------------------------

void SectionBar::ControlHide( Control& sender )
{
   if ( m_section != nullptr )
      if ( sender == *m_section )
         Title_ToolButton.SetIcon( Bitmap( ScaledResource( expand_icon ) ) );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SectionBar.cpp - Released 2025-04-07T08:53:32Z
