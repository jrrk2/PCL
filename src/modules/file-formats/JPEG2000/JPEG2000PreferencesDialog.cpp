//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// JPEG2000PreferencesDialog.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "JPEG2000PreferencesDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

static ImageOptions options;

JPEG2000PreferencesDialog::JPEG2000PreferencesDialog( const JP2Format::EmbeddingOverrides& e,
                                                      const JPEG2000ImageOptions& o, bool isJPC )
   : JPEG2000OptionsDialog( options, o, isJPC )
   , overrides( e )
{
   if ( isJPC )
   {
      SetVariableSize();

      EmbeddedData_GroupBox.Disable();
      EmbeddedData_GroupBox.Hide();

      AdjustToContents();
      SetFixedSize();
   }
   else
   {
      ICCProfile_CheckBox.SetTristateMode();
      ICCProfile_CheckBox.SetToolTip( "Override global core application settings for embedded ICC profiles." );
      ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
         (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

      EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );
   }

   SetWindowTitle( isJPC ? "JPEG2000 Code Stream Preferences" : "JPEG2000 JP2 Format Preferences" );

   OnReturn( (Dialog::return_event_handler)&JPEG2000PreferencesDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void JPEG2000PreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   JPEG2000OptionsDialog::Dialog_Return( sender, retVal );

   if ( retVal == StdDialogCode::Ok && EmbeddedData_GroupBox.IsVisible() )
   {
      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEG2000PreferencesDialog.cpp - Released 2025-04-07T08:53:45Z
