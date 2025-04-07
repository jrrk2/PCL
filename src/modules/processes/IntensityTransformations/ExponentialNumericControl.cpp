//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialNumericControl.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ExponentialNumericControl.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialNumericControl::ExponentialNumericControl( Control& parent ) : NumericEdit( parent )
{
   SetReal();
   SetPrecision( 8 );
   SetRange( 0.0, 1.0 );

   m_coefficient.label.Hide();
   m_coefficient.slider.SetRange( 0, 180 ); // step size = 0.05
   m_coefficient.slider.SetScaledMinWidth( 250 );
   m_coefficient.SetReal();
   m_coefficient.SetRange( 1.00, 10.00 );
   m_coefficient.SetPrecision( 2 );
   m_coefficient.SetToolTip( "<p>Coefficient value in the range [1.0,9.99]</p>" );
   m_coefficient.OnValueUpdated( (NumericEdit::value_event_handler)&ExponentialNumericControl::__CoefficientValueUpdated, *this );

   m_exponent.SetRange( -8, 0 );
   m_exponent.SetToolTip( "<p>Exponent in the range [-8,0]</p>" );
   m_exponent.OnValueUpdated( (SpinBox::value_event_handler)&ExponentialNumericControl::__ExponentValueUpdated, *this );

   sizer.Add( m_coefficient, 100 );
   sizer.Add( m_exponent );

   AdjustToContents();
}

void ExponentialNumericControl::__CoefficientValueUpdated( NumericEdit& sender, double value )
{
   SetValue( Range( value, 1.0, 9.99 ) * Pow10I<double>()( m_exponent.Value() ) );
   if ( m_handlers != nullptr )
      if ( m_handlers->onValueUpdated != nullptr )
         (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, Value() );
}

void ExponentialNumericControl::__ExponentValueUpdated( SpinBox& sender, int value )
{
   SetValue( m_coefficient.Value() * Pow10I<double>()( value ) );
   if ( m_handlers != nullptr )
      if ( m_handlers->onValueUpdated != nullptr )
         (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, Value() );
}

void ExponentialNumericControl::UpdateControls()
{
   NumericEdit::UpdateControls();

   double x = Abs( Value() );
   if ( 1 + x == 1 )
   {
      SetValue( 1.0e-08 );
      if ( m_handlers != nullptr )
         if ( m_handlers->onValueUpdated != nullptr )
            (m_handlers->onValueUpdatedReceiver->*m_handlers->onValueUpdated)( *this, Value() );
      return;
   }

   int exponent = int( Floor( Log( x ) ) );

   double coefficient = Round( x/Pow10I<double>()( exponent ), 2 );
   if ( Value() < 0 )
      coefficient = -coefficient;

   m_coefficient.SetValue( coefficient );
   m_exponent.SetValue( exponent );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialNumericControl.cpp - Released 2025-04-07T08:53:56Z
