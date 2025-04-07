//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationInterface.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ExponentialTransformationInterface.h"
#include "ExponentialTransformationProcess.h"

#include <pcl/ImageWindow.h>
#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExponentialTransformationInterface* TheExponentialTransformationInterface = nullptr;

// ----------------------------------------------------------------------------

ExponentialTransformationInterface::ExponentialTransformationInterface()
   : m_instance( TheExponentialTransformationProcess )
{
   TheExponentialTransformationInterface = this;
}

// ----------------------------------------------------------------------------

ExponentialTransformationInterface::~ExponentialTransformationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString ExponentialTransformationInterface::Id() const
{
   return "ExponentialTransformation";
}

// ----------------------------------------------------------------------------

MetaProcess* ExponentialTransformationInterface::Process() const
{
   return TheExponentialTransformationProcess;
}

// ----------------------------------------------------------------------------

String ExponentialTransformationInterface::IconImageSVGFile() const
{
   return "@module_icons_dir/ExponentialTransformation.svg";
}

// ----------------------------------------------------------------------------

InterfaceFeatures ExponentialTransformationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void ExponentialTransformationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void ExponentialTransformationInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void ExponentialTransformationInterface::ResetInstance()
{
   ExponentialTransformationInstance defaultInstance( TheExponentialTransformationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ExponentialTransformation" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheExponentialTransformationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ExponentialTransformationInterface::NewProcess() const
{
   return new ExponentialTransformationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ExponentialTransformationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an ExponentialTransformation instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );

   UpdateControls();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool ExponentialTransformationInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int ) const
{
   return true;
}

// ----------------------------------------------------------------------------

ExponentialTransformationInterface::RealTimeThread::RealTimeThread()
   : m_instance( TheExponentialTransformationProcess )
{
}

void ExponentialTransformationInterface::RealTimeThread::Reset( const UInt16Image& image,
                                                                const ExponentialTransformationInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void ExponentialTransformationInterface::RealTimeThread::Run()
{
   MuteStatus status;
   m_image.SetStatusCallback( &status );
   m_instance.Preview( m_image );
   m_image.ResetSelections();
}

bool ExponentialTransformationInterface::GenerateRealTimePreview( UInt16Image& image, const View& view, const Rect&, int zoomLevel, String& ) const
{
   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      ExponentialTransformationInstance previewInstance( m_instance );
      if ( zoomLevel < 0 )
         previewInstance.p_sigma /= -zoomLevel;

      m_realTimeThread->Reset( image, previewInstance );
      m_realTimeThread->Start();

      while ( m_realTimeThread->IsActive() )
      {
         ProcessEvents();

         if ( !IsRealTimePreviewActive() )
         {
            m_realTimeThread->Abort();
            m_realTimeThread->Wait();

            delete m_realTimeThread;
            m_realTimeThread = nullptr;
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );

         delete m_realTimeThread;
         m_realTimeThread = nullptr;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ExponentialTransformationInterface::UpdateControls()
{
   GUI->Function_ComboBox.SetCurrentItem( m_instance.p_type );
   GUI->Order_NumericControl.SetValue( m_instance.p_order );
   GUI->Smoothing_NumericControl.SetValue( m_instance.p_sigma );
   GUI->LightnessMask_CheckBox.SetChecked( m_instance.p_useLightnessMask );
}

void ExponentialTransformationInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != nullptr )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ExponentialTransformationInterface::__Function_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   m_instance.p_type = itemIndex;
   UpdateRealTimePreview();
}

void ExponentialTransformationInterface::__FunctionParameter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Order_NumericControl )
      m_instance.p_order = value;
   else if ( sender == GUI->Smoothing_NumericControl )
      m_instance.p_sigma = value;

   UpdateRealTimePreview();
}

void ExponentialTransformationInterface::__LightnessMask_ButtonClick( Button& /*sender*/, bool checked )
{
   m_instance.p_useLightnessMask = checked;
   UpdateRealTimePreview();
}

void ExponentialTransformationInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != nullptr )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------

ExponentialTransformationInterface::GUIData::GUIData( ExponentialTransformationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth = fnt.Width( String( "Smoothing:" ) + 'M' );

   //

   Function_Label.SetText( "Function:" );
   Function_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Function_Label.SetFixedWidth( labelWidth );

   Function_ComboBox.AddItem( "PIP - Power of Inverted Pixels" );
   Function_ComboBox.AddItem( "SMI - Screen/Mask/Invert" );
   Function_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ExponentialTransformationInterface::__Function_ItemSelected, w );

   Function_Sizer.SetSpacing( 4 );
   Function_Sizer.Add( Function_Label );
   Function_Sizer.Add( Function_ComboBox );

   //

   Smoothing_NumericControl.label.SetText( "Smoothing:" );
   Smoothing_NumericControl.label.SetFixedWidth( labelWidth );
   Smoothing_NumericControl.slider.SetScaledMinWidth( 250 );
   Smoothing_NumericControl.slider.SetRange( 0, 200 );
   Smoothing_NumericControl.SetReal();
   Smoothing_NumericControl.SetRange( TheExponentialFunctionSmoothingParameter->MinimumValue(), TheExponentialFunctionSmoothingParameter->MaximumValue() );
   Smoothing_NumericControl.SetPrecision( TheExponentialFunctionSmoothingParameter->Precision() );
   //Smoothing_NumericControl.SetToolTip( "" );
   Smoothing_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ExponentialTransformationInterface::__FunctionParameter_ValueUpdated, w );

   //

   Order_NumericControl.label.SetText( "Order:" );
   Order_NumericControl.label.SetFixedWidth( labelWidth );
   Order_NumericControl.slider.SetScaledMinWidth( 250 );
   Order_NumericControl.slider.SetRange( 1, 600 );
   Order_NumericControl.SetReal();
   Order_NumericControl.SetRange( TheExponentialFunctionOrderParameter->MinimumValue(), TheExponentialFunctionOrderParameter->MaximumValue() );
   Order_NumericControl.SetPrecision( TheExponentialFunctionOrderParameter->Precision() );
   Order_NumericControl.edit.SetMinWidth( Smoothing_NumericControl.edit.MinWidth() );
   //Order_NumericControl.SetToolTip( "" );
   Order_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ExponentialTransformationInterface::__FunctionParameter_ValueUpdated, w );

   //

   LightnessMask_CheckBox.SetText( "Lightness Mask" );
   LightnessMask_CheckBox.OnClick( (Button::click_event_handler)&ExponentialTransformationInterface::__LightnessMask_ButtonClick, w );

   LightnessMask_Sizer.AddUnscaledSpacing( labelWidth + w.LogicalPixelsToPhysical( 4 ) );
   LightnessMask_Sizer.Add( LightnessMask_CheckBox );

   //

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Function_Sizer );
   Global_Sizer.Add( Order_NumericControl );
   Global_Sizer.Add( Smoothing_NumericControl );
   Global_Sizer.Add( LightnessMask_Sizer );

   w.SetSizer( Global_Sizer );

   w.EnsureLayoutUpdated();
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&ExponentialTransformationInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationInterface.cpp - Released 2025-04-07T08:53:56Z
