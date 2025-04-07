//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MultiViewSelectionDialog.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/MultiViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MultiViewSelectionDialog::MultiViewSelectionDialog( bool allowPreviews )
   : m_selectedViews()
   , m_allowPreviews( allowPreviews )
{
   SetSizer( Global_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Views_TreeBox );
   Global_Sizer.Add( Row2_Sizer );

   Row2_Sizer.SetSpacing( 8 );
   Row2_Sizer.Add( SelectAll_PushButton );
   Row2_Sizer.Add( UnselectAll_PushButton );
   Row2_Sizer.AddStretch();

   if ( m_allowPreviews )
   {
      Row2_Sizer.Add( IncludePreviews_CheckBox );
      Row2_Sizer.Add( IncludeMainViews_CheckBox );
   }

   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( Buttons_Sizer );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Views_TreeBox.SetNumberOfColumns( 1 );
   Views_TreeBox.HideHeader();
   Views_TreeBox.DisableRootDecoration();
   Views_TreeBox.EnableAlternateRowColor();
   Views_TreeBox.SetScaledMinHeight( 300 );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   UnselectAll_PushButton.SetText( "Unselect All" );
   UnselectAll_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   if ( m_allowPreviews )
   {
      IncludeMainViews_CheckBox.SetText( "Main views" );
      IncludeMainViews_CheckBox.SetToolTip( "<p>Include main views in the list of selectable views.</p>" );
      IncludeMainViews_CheckBox.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::OptionClick, *this );
      IncludeMainViews_CheckBox.SetChecked();

      IncludePreviews_CheckBox.SetText( "Previews" );
      IncludePreviews_CheckBox.SetToolTip( "<p>Include previews in the list of selectable views.</p>" );
      IncludePreviews_CheckBox.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::OptionClick, *this );
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
   OK_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&MultiViewSelectionDialog::ButtonClick, *this );

   SetWindowTitle( "Select Views" );

   OnShow( (Control::event_handler)&MultiViewSelectionDialog::ControlShow, *this );

   Regenerate();
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::Regenerate()
{
   Views_TreeBox.Clear();

   Array<View> views = View::AllViews();

   bool includeMainViews = !m_allowPreviews || IncludeMainViews_CheckBox.IsChecked();
   bool includePreviews = m_allowPreviews && IncludePreviews_CheckBox.IsChecked();

   for ( const View& view : views )
   {
      bool isPreview = view.IsPreview();
      if ( isPreview ? includePreviews : includeMainViews )
      {
         TreeBox::Node* node = new TreeBox::Node( Views_TreeBox );
         node->SetCheckable();
         node->Uncheck();
         node->SetText( 0, view.FullId() );
      }
   }

   Views_TreeBox.AdjustColumnWidthToContents( 0 );
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::OptionClick( Button& sender, bool checked )
{
   Regenerate();
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::ButtonClick( Button& sender, bool checked )
{
   if ( sender == SelectAll_PushButton )
   {
      for ( int i = 0, n = Views_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         TreeBox::Node* node = Views_TreeBox[i];
         if ( node != nullptr )
            node->Check();
      }
   }
   else if ( sender == UnselectAll_PushButton )
   {
      for ( int i = 0, n = Views_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         TreeBox::Node* node = Views_TreeBox[i];
         if ( node != nullptr )
            node->Uncheck();
      }
   }
   else if ( sender == OK_PushButton )
   {
      m_selectedViews.Clear();
      for ( int i = 0, n = Views_TreeBox.NumberOfChildren(); i < n; ++i )
      {
         const TreeBox::Node* node = Views_TreeBox[i];
         if ( node != nullptr && node->IsChecked() )
         {
            View view = View::ViewById( node->Text( 0 ) );
            if ( !view.IsNull() )
               m_selectedViews.Add( view );
         }
      }
      Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      m_selectedViews.Clear();
      Cancel();
   }
}

// ----------------------------------------------------------------------------

void MultiViewSelectionDialog::ControlShow( Control& sender )
{
   AdjustToContents();
   SetMinSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MultiViewSelectionDialog.cpp - Released 2025-04-07T08:53:32Z
