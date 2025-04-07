//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationInterface.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FastRotationInterface.h"
#include "FastRotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FastRotationInterface* TheFastRotationInterface = nullptr;

// ----------------------------------------------------------------------------

FastRotationInterface::FastRotationInterface()
   : m_instance( TheFastRotationProcess )
{
   TheFastRotationInterface = this;
}

// ----------------------------------------------------------------------------

FastRotationInterface::~FastRotationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString FastRotationInterface::Id() const
{
   return "FastRotation";
}

// ----------------------------------------------------------------------------

MetaProcess* FastRotationInterface::Process() const
{
   return TheFastRotationProcess;
}

// ----------------------------------------------------------------------------

String FastRotationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/FastRotation.svg";
}

// ----------------------------------------------------------------------------

void FastRotationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentWindow();
}

// ----------------------------------------------------------------------------

void FastRotationInterface::ResetInstance()
{
   FastRotationInstance defaultInstance( TheFastRotationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "FastRotation" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheFastRotationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* FastRotationInterface::NewProcess() const
{
   return new FastRotationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const FastRotationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a FastRotation instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool FastRotationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void FastRotationInterface::UpdateControls()
{
   switch ( m_instance.p_mode )
   {
   default:
   case FRMode::Rotate180:
      GUI->Rotate180_RadioButton.Check();
      break;
   case FRMode::Rotate90CW:
      GUI->Rotate90CW_RadioButton.Check();
      break;
   case FRMode::Rotate90CCW:
      GUI->Rotate90CCW_RadioButton.Check();
      break;
   case FRMode::HorizontalMirror:
      GUI->HorizontalMirror_RadioButton.Check();
      break;
   case FRMode::VerticalMirror:
      GUI->VerticalMirror_RadioButton.Check();
      break;
   }
}

// ----------------------------------------------------------------------------

void FastRotationInterface::e_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Rotate180_RadioButton )
      m_instance.p_mode = FRMode::Rotate180;
   else if ( sender == GUI->Rotate90CW_RadioButton )
      m_instance.p_mode = FRMode::Rotate90CW;
   else if ( sender == GUI->Rotate90CCW_RadioButton )
      m_instance.p_mode = FRMode::Rotate90CCW;
   else if ( sender == GUI->HorizontalMirror_RadioButton )
      m_instance.p_mode = FRMode::HorizontalMirror;
   else if ( sender == GUI->VerticalMirror_RadioButton )
      m_instance.p_mode = FRMode::VerticalMirror;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FastRotationInterface::GUIData::GUIData( FastRotationInterface& w )
{
   Rotate180_RadioButton.SetText( "Rotate 180\xb0" );
   Rotate180_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::e_ButtonClick, w );

   Rotate90CW_RadioButton.SetText( "Rotate 90\xb0 Clockwise" );
   Rotate90CW_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::e_ButtonClick, w );

   Rotate90CCW_RadioButton.SetText( "Rotate 90\xb0 Counter-clockwise" );
   Rotate90CCW_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::e_ButtonClick, w );

   HorizontalMirror_RadioButton.SetText( "Horizontal Mirror" );
   HorizontalMirror_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::e_ButtonClick, w );

   VerticalMirror_RadioButton.SetText( "Vertical Mirror" );
   VerticalMirror_RadioButton.OnClick( (Button::click_event_handler)&FastRotationInterface::e_ButtonClick, w );

   Transform_Sizer.SetMargin( 6 );
   Transform_Sizer.SetSpacing( 4 );
   Transform_Sizer.Add( Rotate180_RadioButton );
   Transform_Sizer.Add( Rotate90CW_RadioButton );
   Transform_Sizer.Add( Rotate90CCW_RadioButton );
   Transform_Sizer.Add( HorizontalMirror_RadioButton );
   Transform_Sizer.Add( VerticalMirror_RadioButton );

   Transform_GroupBox.SetTitle( "Transformation" );
   Transform_GroupBox.SetSizer( Transform_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Transform_GroupBox );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationInterface.cpp - Released 2025-04-07T08:53:55Z
