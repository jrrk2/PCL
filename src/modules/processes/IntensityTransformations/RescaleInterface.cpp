//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// RescaleInterface.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RescaleInterface.h"
#include "RescaleProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RescaleInterface* TheRescaleInterface = nullptr;

// ----------------------------------------------------------------------------

RescaleInterface::RescaleInterface()
   : m_instance( TheRescaleProcess )
{
   TheRescaleInterface = this;
}

// ----------------------------------------------------------------------------

RescaleInterface::~RescaleInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString RescaleInterface::Id() const
{
   return "Rescale";
}

// ----------------------------------------------------------------------------

MetaProcess* RescaleInterface::Process() const
{
   return TheRescaleProcess;
}

// ----------------------------------------------------------------------------

String RescaleInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/Rescale.svg";
}

// ----------------------------------------------------------------------------

void RescaleInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void RescaleInterface::ResetInstance()
{
   RescaleInstance defaultInstance( TheRescaleProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool RescaleInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Rescale" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheRescaleProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* RescaleInterface::NewProcess() const
{
   return new RescaleInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool RescaleInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const RescaleInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Rescale instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool RescaleInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RescaleInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void RescaleInterface::UpdateControls()
{
   GUI->RGBK_RadioButton.SetChecked( m_instance.p_mode == RescalingMode::RGBK );
   GUI->RGBK_Individual_RadioButton.SetChecked( m_instance.p_mode == RescalingMode::RGBK_Individual );
   GUI->CIEL_RadioButton.SetChecked( m_instance.p_mode == RescalingMode::CIEL );
   GUI->CIEY_RadioButton.SetChecked( m_instance.p_mode == RescalingMode::CIEY );
}

// ----------------------------------------------------------------------------

void RescaleInterface::RescaleButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->RGBK_RadioButton )
      m_instance.p_mode = RescalingMode::RGBK;
   else if ( sender == GUI->RGBK_Individual_RadioButton )
      m_instance.p_mode = RescalingMode::RGBK_Individual;
   else if ( sender == GUI->CIEL_RadioButton )
      m_instance.p_mode = RescalingMode::CIEL;
   else if ( sender == GUI->CIEY_RadioButton )
      m_instance.p_mode = RescalingMode::CIEY;
}

// ----------------------------------------------------------------------------

RescaleInterface::GUIData::GUIData( RescaleInterface& w )
{
   RGBK_RadioButton.SetText( "RGB/K" );
   RGBK_RadioButton.SetToolTip( "<p>Apply a rescaling operation in the RGB/gray color space.</p>"
      "<p>In this mode the rescaling range is taken as the existing minimum and maximum values "
      "among all nominal channels in the target image, which preserves color balance.</p>" );
   RGBK_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   RGBK_Individual_RadioButton.SetText( "RGB/K, individual channels" );
   RGBK_Individual_RadioButton.SetToolTip( "<p>Rescale each nominal channel separately.</p>"
      "<p>In this mode a different rescaling range is used for each nominal channel, which can modify "
      "color balance.</p>" );
   RGBK_Individual_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   CIEL_RadioButton.SetText( "CIE L* (lightness)" );
   CIEL_RadioButton.SetToolTip( "<p>Rescale the CIE L* component.</p>"
      "<p>The CIE L* component (lightness) is extracted, rescaled, and then reinserted in the target RGB image.</p>"
      "<p><b>Important &mdash</b> If the target image is linear, this rescaling operation will break "
      "linearity, as CIE L* is a nonlinear function of the R, G and B sample values for each pixel.</p>" );
   CIEL_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   CIEY_RadioButton.SetText( "CIE Y (luminance)" );
   CIEY_RadioButton.SetToolTip( "<p>Rescale the CIE Y component.</p>"
      "<p>The CIE Y component (relative luminance) is extracted, rescaled, and then reinserted in the "
      "target RGB image.</p>"
      "<p><b>Important &mdash</b> The CIE Y component is a linear combination of the R, G and B sample "
      "values for each pixel. However, if the target image is linear, a linear RGB working space should "
      "be used (gamma=1) to preserve linearity.</p>" );
   CIEY_RadioButton.OnClick( (pcl::Button::click_event_handler)&RescaleInterface::RescaleButtonClick, w );

   Rescale_Sizer.SetMargin( 6 );
   Rescale_Sizer.SetSpacing( 4 );
   Rescale_Sizer.Add( RGBK_RadioButton );
   Rescale_Sizer.Add( RGBK_Individual_RadioButton );
   Rescale_Sizer.Add( CIEL_RadioButton );
   Rescale_Sizer.Add( CIEY_RadioButton );

   Rescale_GroupBox.SetTitle( "Rescaling Mode" );
   Rescale_GroupBox.SetSizer( Rescale_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Rescale_GroupBox );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleInterface.cpp - Released 2025-04-07T08:53:56Z
