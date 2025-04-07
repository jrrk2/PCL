//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierTransformInterface.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FourierTransformInterface.h"
#include "FourierTransformProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FourierTransformInterface* TheFourierTransformInterface = nullptr;

// ----------------------------------------------------------------------------

FourierTransformInterface::FourierTransformInterface()
   : instance( TheFourierTransformProcess )
{
   TheFourierTransformInterface = this;
}

// ----------------------------------------------------------------------------

FourierTransformInterface::~FourierTransformInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString FourierTransformInterface::Id() const
{
   return "FourierTransform";
}

// ----------------------------------------------------------------------------

MetaProcess* FourierTransformInterface::Process() const
{
   return TheFourierTransformProcess;
}

// ----------------------------------------------------------------------------

String FourierTransformInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/FourierTransform.svg";
}

// ----------------------------------------------------------------------------

void FourierTransformInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void FourierTransformInterface::ResetInstance()
{
   FourierTransformInstance defaultInstance( TheFourierTransformProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FourierTransformInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FourierTransform" );
      ImportProcess( instance );
   }

   dynamic = false;
   return &P == TheFourierTransformProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FourierTransformInterface::NewProcess() const
{
   return new FourierTransformInstance( instance );
}

// ----------------------------------------------------------------------------

bool FourierTransformInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const FourierTransformInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FourierTransform instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool FourierTransformInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FourierTransformInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void FourierTransformInterface::UpdateControls()
{
   GUI->Shift_CheckBox.SetChecked( instance.centered );
   GUI->Radial_CheckBox.SetChecked( instance.radialCoordinates );
}

// ----------------------------------------------------------------------------

void FourierTransformInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->Shift_CheckBox )
      instance.centered = checked;
   else if ( sender == GUI->Radial_CheckBox )
      instance.radialCoordinates = checked;
   UpdateControls();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FourierTransformInterface::GUIData::GUIData( FourierTransformInterface& w )
{
   Shift_CheckBox.SetText( "Centered" );
   Shift_CheckBox.SetToolTip(
      "<p>When checked, the origin of the DFT (the DC component) is centered in the transform matrix.</p>"
      "<p>When unchecked, the origin is at the top left corner of the transform." );
   Shift_CheckBox.OnClick( (Button::click_event_handler)&FourierTransformInterface::__Click, w );

   Radial_CheckBox.SetText( "Power Spectrum" );
   Radial_CheckBox.SetToolTip(
      "<p>When checked, magnitude and phase components are computed instead of the real and imaginary components.</p>" );
   Radial_CheckBox.OnClick( (Button::click_event_handler)&FourierTransformInterface::__Click, w );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Shift_CheckBox );
   Global_Sizer.Add( Radial_CheckBox );

   w.SetSizer( Global_Sizer );

   w.SetScaledMinWidth( 250 );
   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FourierTransformInterface.cpp - Released 2025-04-07T08:53:55Z
