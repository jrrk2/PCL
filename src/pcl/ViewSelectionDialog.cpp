//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ViewSelectionDialog.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ViewSelectionDialog::ViewSelectionDialog( const IsoString& id, bool allowPreviews )
   : m_id( id )
   , m_allowPreviews( allowPreviews )
{
   SetSizer( Global_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Images_ViewList );

   if ( m_allowPreviews )
   {
      Global_Sizer.Add( IncludeMainViews_CheckBox );
      Global_Sizer.Add( IncludePreviews_CheckBox );
   }

   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   if ( m_allowPreviews )
      Images_ViewList.GetAll();
   else
      Images_ViewList.GetMainViews();

   Images_ViewList.SelectView( View::ViewById( m_id ) );
   Images_ViewList.SetMinWidth( Font().Width( String( 'M', 40 ) ) );
   Images_ViewList.OnViewSelected( (ViewList::view_event_handler)&ViewSelectionDialog::ViewSelected, *this );

   if ( m_allowPreviews )
   {
      IncludeMainViews_CheckBox.SetText( "Include main views" );
      IncludeMainViews_CheckBox.OnClick( (Button::click_event_handler)&ViewSelectionDialog::OptionClick, *this );
      IncludeMainViews_CheckBox.SetChecked();

      IncludePreviews_CheckBox.SetText( "Include previews" );
      IncludePreviews_CheckBox.OnClick( (Button::click_event_handler)&ViewSelectionDialog::OptionClick, *this );
      IncludePreviews_CheckBox.SetChecked();
   }
   else
   {
      IncludeMainViews_CheckBox.Hide();
      IncludePreviews_CheckBox.Hide();
   }

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&ViewSelectionDialog::ButtonClick, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&ViewSelectionDialog::ButtonClick, *this );

   SetWindowTitle( "Select View" );

   OnShow( (Control::event_handler)&ViewSelectionDialog::ControlShow, *this );
}

void ViewSelectionDialog::ViewSelected( ViewList& sender, View& view )
{
   m_id = view.IsNull() ? IsoString() : view.FullId();
}

void ViewSelectionDialog::OptionClick( Button& sender, bool checked )
{
   bool includeMainViews = !m_allowPreviews || IncludeMainViews_CheckBox.IsChecked();
   bool includePreviews = m_allowPreviews && IncludePreviews_CheckBox.IsChecked();
   Images_ViewList.Regenerate( includeMainViews, includePreviews );
   Images_ViewList.SelectView( View::ViewById( m_id ) );
}

void ViewSelectionDialog::ButtonClick( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void ViewSelectionDialog::ControlShow( Control& sender )
{
   AdjustToContents();
   SetMinWidth();
   SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ViewSelectionDialog.cpp - Released 2025-04-07T08:53:32Z
