//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// DynamicCropPreferencesDialog.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "DynamicCropInterface.h"
#include "DynamicCropPreferencesDialog.h"

namespace pcl
{

#define m_fillColor  TheDynamicCropInterface->m_fillColor

// ----------------------------------------------------------------------------

DynamicCropPreferencesDialog::DynamicCropPreferencesDialog()
{
   savedColor = m_fillColor;

   pcl::Font fnt = Font();
   int buttonWidth = fnt.Width( String( "Cancel" ) + String( 'M', 4 ) );

   Black_RadioButton.SetText( "Black" );
   Black_RadioButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::__Click, *this );

   White_RadioButton.SetText( "White" );
   White_RadioButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::__Click, *this );

   Alpha_Label.SetText( "Alpha Blend:" );
   Alpha_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Alpha_Slider.SetRange( 0, 255 );
   Alpha_Slider.SetStepSize( 10 );
   Alpha_Slider.SetTickFrequency( 10 );
   Alpha_Slider.SetTickStyle( TickStyle::NoTicks );
   Alpha_Slider.SetScaledMinWidth( 265 );
   Alpha_Slider.OnValueUpdated( (Slider::value_event_handler)&DynamicCropPreferencesDialog::__ValueUpdated, *this );

   FillColor_Sizer.SetMargin( 8 );
   FillColor_Sizer.SetSpacing( 6 );
   FillColor_Sizer.Add( Black_RadioButton );
   FillColor_Sizer.Add( White_RadioButton );
   FillColor_Sizer.AddSpacing( 10 );
   FillColor_Sizer.Add( Alpha_Label );
   FillColor_Sizer.Add( Alpha_Slider );

   FillColor_GroupBox.SetTitle( "Fill Color" );
   FillColor_GroupBox.SetSizer( FillColor_Sizer );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetMinWidth( buttonWidth );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::__Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetMinWidth( buttonWidth );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::__Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( FillColor_GroupBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "DynamicCrop Interface Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&DynamicCropPreferencesDialog::__Return, *this );

   Black_RadioButton.SetChecked( Red( m_fillColor ) == 0 );
   White_RadioButton.SetChecked( Red( m_fillColor ) != 0 );

   Alpha_Slider.SetValue( Alpha( m_fillColor ) );

   Update();
}

void DynamicCropPreferencesDialog::Update()
{
   int alpha = Alpha_Slider.Value();

   Alpha_Slider.SetToolTip( (alpha == 0) ? "Transparent" :
                  ((alpha == 255) ? "Opaque" : String( Alpha_Slider.Value() )) );

   uint8 base = White_RadioButton.IsChecked() ? 0xFF : 0x00;

   m_fillColor = RGBAColor( base, base, base, alpha );

   if ( !TheDynamicCropInterface->m_view.IsNull() )
      TheDynamicCropInterface->UpdateView();
}

void DynamicCropPreferencesDialog::__ValueUpdated( Slider& /*sender*/, int /*value*/ )
{
   Update();
}

void DynamicCropPreferencesDialog::__Click( Button& sender, bool /*checked*/ )
{
   if ( sender == Black_RadioButton || sender == White_RadioButton )
      Update();
   else if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void DynamicCropPreferencesDialog::__Return( Dialog& sender, int retVal )
{
   if ( retVal != StdDialogCode::Ok )
   {
      m_fillColor = savedColor;
      if ( !TheDynamicCropInterface->m_view.IsNull() )
         TheDynamicCropInterface->UpdateView();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicCropPreferencesDialog.cpp - Released 2025-04-07T08:53:55Z
