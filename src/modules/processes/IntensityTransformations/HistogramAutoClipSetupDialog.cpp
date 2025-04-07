//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// HistogramAutoClipSetupDialog.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "HistogramAutoClipSetupDialog.h"
#include "HistogramTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramAutoClipSetupDialog::HistogramAutoClipSetupDialog()
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( '0', 10 ) );

   m_shadowsAutoClipping = TheHistogramTransformationInterface->m_shadowsAutoClipping;
   m_highlightsAutoClipping = TheHistogramTransformationInterface->m_highlightsAutoClipping;

   ShadowsAmount_NumericControl.label.SetText( "Shadows:" );
   ShadowsAmount_NumericControl.label.SetFixedWidth( labelWidth );
   ShadowsAmount_NumericControl.slider.SetScaledFixedWidth( 250 );
   ShadowsAmount_NumericControl.slider.SetRange( 0, 200 );
   ShadowsAmount_NumericControl.SetReal();
   ShadowsAmount_NumericControl.SetRange( 0.0, 100.0 );
   ShadowsAmount_NumericControl.SetPrecision( 3 );
   ShadowsAmount_NumericControl.SetToolTip( "Shadows auto clipping amount" );
   ShadowsAmount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramAutoClipSetupDialog::__ClippingAmount_ValueUpdated, *this );

   HighlightsAmount_NumericControl.label.SetText( "Highlights:" );
   HighlightsAmount_NumericControl.label.SetFixedWidth( labelWidth );
   HighlightsAmount_NumericControl.slider.SetScaledFixedWidth( 250 );
   HighlightsAmount_NumericControl.slider.SetRange( 0, 200 );
   HighlightsAmount_NumericControl.SetReal();
   HighlightsAmount_NumericControl.SetRange( 0.0, 100.0 );
   HighlightsAmount_NumericControl.SetPrecision( 3 );
   HighlightsAmount_NumericControl.SetToolTip( "Highlights auto clipping amount" );
   HighlightsAmount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramAutoClipSetupDialog::__ClippingAmount_ValueUpdated, *this );

   ClippingParameters_Sizer.SetMargin( 8 );
   ClippingParameters_Sizer.SetSpacing( 6 );
   ClippingParameters_Sizer.Add( ShadowsAmount_NumericControl );
   ClippingParameters_Sizer.Add( HighlightsAmount_NumericControl );

   ClippingParameters_GroupBox.SetTitle( "Clipping Amounts (Percentage of total pixels)" );
   ClippingParameters_GroupBox.SetSizer( ClippingParameters_Sizer );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&HistogramAutoClipSetupDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&HistogramAutoClipSetupDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ClippingParameters_GroupBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   EnsureLayoutUpdated();
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Histogram Auto Clipping Setup" );

   OnReturn( (Dialog::return_event_handler)&HistogramAutoClipSetupDialog::__Dialog_Return, *this );

   ShadowsAmount_NumericControl.SetValue( 100*m_shadowsAutoClipping );
   HighlightsAmount_NumericControl.SetValue( 100*m_highlightsAutoClipping );

   Update();
}

// ----------------------------------------------------------------------------

void HistogramAutoClipSetupDialog::__ClippingAmount_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == ShadowsAmount_NumericControl )
      m_shadowsAutoClipping = value/100;
   else if ( sender == HighlightsAmount_NumericControl )
      m_highlightsAutoClipping = value/100;
}

// ----------------------------------------------------------------------------

void HistogramAutoClipSetupDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

void HistogramAutoClipSetupDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      TheHistogramTransformationInterface->m_shadowsAutoClipping = m_shadowsAutoClipping;
      TheHistogramTransformationInterface->m_highlightsAutoClipping = m_highlightsAutoClipping;
      TheHistogramTransformationInterface->UpdateAutoClippingToolTips();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramAutoClipSetupDialog.cpp - Released 2025-04-07T08:53:56Z
