//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// JPEGPreferencesDialog.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "JPEGPreferencesDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

static ImageOptions options;

JPEGPreferencesDialog::JPEGPreferencesDialog( const JPEGFormat::EmbeddingOverrides& e, const JPEGImageOptions& o )
   : JPEGOptionsDialog( options, o )
   , overrides( e )
{
   ICCProfile_CheckBox.SetTristateMode();
   ICCProfile_CheckBox.SetToolTip( "Override global core application settings for embedded ICC profiles." );
   ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
      (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );

   SetWindowTitle( "JPEG Format Preferences" );

   OnReturn( (Dialog::return_event_handler)&JPEGPreferencesDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void JPEGPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   JPEGOptionsDialog::Dialog_Return( sender, retVal );

   if ( retVal == StdDialogCode::Ok )
   {
      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEGPreferencesDialog.cpp - Released 2025-04-07T08:53:45Z
