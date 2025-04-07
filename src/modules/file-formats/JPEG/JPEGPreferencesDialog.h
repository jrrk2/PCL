//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// JPEGPreferencesDialog.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEGPreferencesDialog_h
#define __JPEGPreferencesDialog_h

#include "JPEGOptionsDialog.h"
#include "JPEGFormat.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class JPEGPreferencesDialog : public JPEGOptionsDialog
{
public:

   JPEGPreferencesDialog( const JPEGFormat::EmbeddingOverrides&, const JPEGImageOptions& );

   JPEGFormat::EmbeddingOverrides overrides;

private:

   void Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEGPreferencesDialog_h

// ----------------------------------------------------------------------------
// EOF JPEGPreferencesDialog.h - Released 2025-04-07T08:53:45Z
