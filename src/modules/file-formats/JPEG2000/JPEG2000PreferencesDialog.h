//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// JPEG2000PreferencesDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEG2000PreferencesDialog_h
#define __JPEG2000PreferencesDialog_h

#include "JPEG2000OptionsDialog.h"
#include "JPEG2000Format.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class JPEG2000PreferencesDialog: public JPEG2000OptionsDialog
{
public:

   JPEG2000PreferencesDialog( const JP2Format::EmbeddingOverrides&, const JPEG2000ImageOptions&, bool isJPC );

   JP2Format::EmbeddingOverrides overrides;

private:

   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEG2000PreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF JPEG2000PreferencesDialog.h - Released 2025-04-07T08:53:45Z
