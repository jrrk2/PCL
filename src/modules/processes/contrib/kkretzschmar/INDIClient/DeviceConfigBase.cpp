//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// DeviceConfigBase.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "DeviceConfigBase.h"
#include "INDIClient.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConfigDialogBase::ConfigDialogBase( const String& deviceName )
   : m_device( deviceName )
{
   SaveConfig_Button.SetText( "Save" );
   SaveConfig_Button.SetToolTip( "<p>Stores the current configuration on the Indigo server.</p>" );
   SaveConfig_Button.SetIcon( ScaledResource( ":/icons/save.png" ) );
   SaveConfig_Button.OnClick( (Button::click_event_handler)&ConfigDialogBase::e_Click, *this );

   Ok_Button.SetText( "Ok" );
   Ok_Button.SetIcon( ScaledResource( ":/icons/ok.png" ) );
   Ok_Button.OnClick( (Button::click_event_handler)&ConfigDialogBase::e_Click, *this );

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&ConfigDialogBase::e_Click, *this );

   ConfigButton_Sizer.SetSpacing( 8 );
   ConfigButton_Sizer.SetMargin( 8 );
   ConfigButton_Sizer.Add( SaveConfig_Button );
   ConfigButton_Sizer.AddStretch();
   ConfigButton_Sizer.Add( Ok_Button );
   ConfigButton_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );

   SetSizer( Global_Sizer );

   SetWindowTitle( "Configuration Dialog" );

   OnShow( (Control::event_handler)&ConfigDialogBase::e_Show, *this );
}

// ----------------------------------------------------------------------------

void ConfigDialogBase::AddBaseControls()
{
   Global_Sizer.Add( ConfigButton_Sizer );
}

// ----------------------------------------------------------------------------

void ConfigDialogBase::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;
      AdjustToContents();
      SetFixedHeight();
      SetMinSize();
   }
}

// ----------------------------------------------------------------------------

void ConfigDialogBase::e_Click( Button& sender, bool checked )
{
   if ( sender == Ok_Button )
   {
      SendUpdatedProperties();
      Ok();
   }

   if ( sender == Cancel_Button )
   {
      Cancel();
   }

   if ( sender == SaveConfig_Button )
   {
      SendUpdatedProperties();
      INDIClient::TheClient()->SendNewPropertyItem( m_device, CONFIG_PROPERTY_NAME, "INDI_SWITCH", CONFIG_SAVE_ITEM_NAME, "ON" );
      Ok();
   }
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF DeviceConfigBase.cpp - Released 2025-04-07T08:53:56Z
