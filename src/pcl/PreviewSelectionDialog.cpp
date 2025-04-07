//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PreviewSelectionDialog.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/PreviewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PreviewSelectionDialog::PreviewSelectionDialog( const IsoString& id )
   : m_id( id )
{
   SetSizer( Global_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Images_ViewList );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Images_ViewList.GetPreviews();
   Images_ViewList.SelectView( View::ViewById( m_id ) );
   Images_ViewList.SetMinWidth( Font().Width( String( 'M', 40 ) ) );
   Images_ViewList.OnViewSelected( (ViewList::view_event_handler)&PreviewSelectionDialog::ViewSelected, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&PreviewSelectionDialog::ButtonClick, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&PreviewSelectionDialog::ButtonClick, *this );

   SetWindowTitle( "Select Preview" );

   OnShow( (Control::event_handler)&PreviewSelectionDialog::ControlShow, *this );
}

void PreviewSelectionDialog::ViewSelected( ViewList& sender, View& view )
{
   m_id = view.IsNull() ? IsoString() : view.FullId();
}

void PreviewSelectionDialog::ButtonClick( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void PreviewSelectionDialog::ControlShow( Control& sender )
{
   AdjustToContents();
   SetMinWidth();
   SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PreviewSelectionDialog.cpp - Released 2025-04-07T08:53:32Z
