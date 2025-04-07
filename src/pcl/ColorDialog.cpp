//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ColorDialog.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ColorDialog.h>
#include <pcl/Graphics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SimpleColorDialog::SimpleColorDialog( RGBA* color, uint32 flags )
   : m_color( color )
   , m_workingColor( (color != nullptr) ? *color : 0u )
   , m_flags( flags )
{
   SetSizer( Global_Sizer );

   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.SetMargin( 6 );
   Global_Sizer.Add( Color_Sizer );
   Global_Sizer.Add( Buttons_Sizer );

   Color_Sizer.SetSpacing( 4 );
   Color_Sizer.Add( Sliders_Sizer, 100 );
   Color_Sizer.Add( ColorSample_Control );

   Sliders_Sizer.SetSpacing( 4 );
   Sliders_Sizer.Add( V0_NumericControl );
   Sliders_Sizer.Add( V1_NumericControl );
   Sliders_Sizer.Add( V2_NumericControl );
   Sliders_Sizer.Add( V3_NumericControl );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   if ( IsGrayscale() )
   {
      SetGreen( m_workingColor, Red( m_workingColor ) );
      SetBlue( m_workingColor, Red( m_workingColor ) );
   }

   if ( !IsAlphaEnabled() )
      SetAlpha( m_workingColor, 255 );

   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "Alpha:" ) + 'T' );
   int buttonWidth = fnt.Width( String( "Cancel" ) + String( 'M', 4 ) );

   V0_NumericControl.label.SetText( IsGrayscale() ? "Gray:" : "Red:" );
   V0_NumericControl.label.SetFixedWidth( labelWidth );
   V0_NumericControl.slider.SetRange( 0, 255 );
   V0_NumericControl.slider.SetMinWidth( 300 );
   V0_NumericControl.SetInteger();
   V0_NumericControl.SetRange( 0, 255 );
   V0_NumericControl.SetValue( Red( m_workingColor ) );
   V0_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::Color_ValueUpdated, *this );

   V1_NumericControl.label.SetText( "Green:" );
   V1_NumericControl.label.SetFixedWidth( labelWidth );
   V1_NumericControl.slider.SetRange( 0, 255 );
   V1_NumericControl.slider.SetMinWidth( 300 );
   V1_NumericControl.SetInteger();
   V1_NumericControl.SetRange( 0, 255 );
   V1_NumericControl.SetValue( Green( m_workingColor ) );
   V1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::Color_ValueUpdated, *this );

   V2_NumericControl.label.SetText( "Blue:" );
   V2_NumericControl.label.SetFixedWidth( labelWidth );
   V2_NumericControl.slider.SetRange( 0, 255 );
   V2_NumericControl.slider.SetMinWidth( 300 );
   V2_NumericControl.SetInteger();
   V2_NumericControl.SetRange( 0, 255 );
   V2_NumericControl.SetValue( Blue( m_workingColor ) );
   V2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::Color_ValueUpdated, *this );

   V3_NumericControl.label.SetText( "Alpha:" );
   V3_NumericControl.label.SetFixedWidth( labelWidth );
   V3_NumericControl.slider.SetRange( 0, 255 );
   V3_NumericControl.slider.SetMinWidth( 300 );
   V3_NumericControl.SetInteger();
   V3_NumericControl.SetRange( 0, 255 );
   V3_NumericControl.SetValue( Alpha( m_workingColor ) );
   V3_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::Color_ValueUpdated, *this );

   ColorSample_Control.SetFixedWidth( 4*fnt.Width( 'm' ) );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&SimpleColorDialog::ColorSample_Paint, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetFixedWidth( buttonWidth );
   OK_PushButton.SetCursor( pcl::Cursor( StdCursor::Checkmark ) );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&SimpleColorDialog::Done_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetFixedWidth( buttonWidth );
   Cancel_PushButton.SetCursor( pcl::Cursor( StdCursor::Crossmark ) );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&SimpleColorDialog::Done_Click, *this );

   if ( IsGrayscale() )
   {
      V1_NumericControl.Hide();
      V2_NumericControl.Hide();
   }

   if ( !IsAlphaEnabled() )
      V3_NumericControl.Hide();

   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Define Custom Color" );

   OnReturn( (Dialog::return_event_handler)&SimpleColorDialog::Dialog_Return, *this );
}

void SimpleColorDialog::SetColor( RGBA rgba )
{
   m_workingColor = rgba;

   if ( IsGrayscale() )
   {
      V0_NumericControl.SetValue( Red( m_workingColor ) );
      V1_NumericControl.SetValue( Red( m_workingColor ) );
      V2_NumericControl.SetValue( Red( m_workingColor ) );
   }
   else
   {
      V0_NumericControl.SetValue( Red( m_workingColor ) );
      V1_NumericControl.SetValue( Green( m_workingColor ) );
      V2_NumericControl.SetValue( Blue( m_workingColor ) );
   }

   if ( IsAlphaEnabled() )
      V3_NumericControl.SetValue( Alpha( m_workingColor ) );
}

void SimpleColorDialog::Color_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == V0_NumericControl )
      SetRed( m_workingColor, int( value ) );

   if ( IsGrayscale() )
   {
      SetGreen( m_workingColor, int( value ) );
      SetBlue( m_workingColor, int( value ) );
   }
   else
   {
      if ( sender == V1_NumericControl )
         SetGreen( m_workingColor, int( value ) );
      else if ( sender == V2_NumericControl )
         SetBlue( m_workingColor, int( value ) );
   }

   if ( IsAlphaEnabled() )
      if ( sender == V3_NumericControl )
         SetAlpha( m_workingColor, int( value ) );

   ColorSample_Control.Update();
}

void SimpleColorDialog::ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   double f = sender.DisplayPixelRatio();
   Graphics g( sender );
   g.EnableAntialiasing();
   Rect r = sender.BoundsRect();
   if ( Alpha( m_workingColor ) != 0xff )
      g.DrawTiledBitmap( r, Bitmap( UIScaledResource( f, ":/images/transparent_small.png" ) ) );
   g.SetBrush( m_workingColor );
   g.SetPen( 0xff000000, f );
   g.DrawRect( r );
}

void SimpleColorDialog::Done_Click( Button& sender, bool/*checked*/ )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void SimpleColorDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
      if ( m_color != nullptr )
         *m_color = m_workingColor;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ColorDialog.cpp - Released 2025-04-07T08:53:32Z
