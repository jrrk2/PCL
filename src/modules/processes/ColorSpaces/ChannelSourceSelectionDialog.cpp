//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelSourceSelectionDialog.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ChannelSourceSelectionDialog.h"

#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelSourceSelectionDialog::ChannelSourceSelectionDialog( const String& channelSuffix,
                                                            const String& channelDesc )
{
   pcl::Font fnt = Font();
   int comboWidth = fnt.Width( String( 'M', 40 ) );
   int buttonWidth = fnt.Width( String( "Cancel" ) + String( 'M', 4 ) );

   Array<ImageWindow> windows = ImageWindow::AllWindows();
   Array<String> targetImages;
   for ( size_type i = 0; i < windows.Length(); ++i )
   {
      View v = windows[i].MainView();
      if ( !v.IsColor() )
         targetImages.Add( v.Id() );
   }

   if ( targetImages.IsEmpty() )
      SourceImages_ComboBox.AddItem( " <* No compatible images *> " );
   else
   {
      size_type currentItem = String::notFound;

      for ( size_type i = 0; i < targetImages.Length(); ++i )
      {
         SourceImages_ComboBox.AddItem( ' ' + targetImages[i] + ' ' );

         if ( currentItem == String::notFound && targetImages[i].EndsWithIC( channelSuffix ) )
            currentItem = i;
      }

      SourceImages_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ChannelSourceSelectionDialog::__SourceImages_ItemSelected, *this );

      if ( currentItem != String::notFound )
         SourceImages_ComboBox.SetCurrentItem( int( currentItem ) );

      sourceId = targetImages[SourceImages_ComboBox.CurrentItem()];
   }

   SourceImages_ComboBox.SetMinWidth( comboWidth );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetFixedWidth( buttonWidth );
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&ChannelSourceSelectionDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetFixedWidth( buttonWidth );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&ChannelSourceSelectionDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( SourceImages_ComboBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Select " + channelDesc + " Source Image" );
}

void ChannelSourceSelectionDialog::__SourceImages_ItemSelected( ComboBox& sender, int itemIndex )
{
   sourceId = sender.ItemText( itemIndex );
   sourceId.Trim();
}

void ChannelSourceSelectionDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelSourceSelectionDialog.cpp - Released 2025-04-07T08:53:55Z
