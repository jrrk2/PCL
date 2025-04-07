//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// JPEGFormat.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEGFormat_h
#define __JPEGFormat_h

#include "JPEG.h"

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class JPEGFormat : public MetaFileFormat
{
public:

   JPEGFormat() = default;

   IsoString Name() const override;

   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;

   uint32 Version() const override;

   String Description() const override;
   String Implementation() const override;

   String IconImageSVGFile() const override;

   bool CanStore16Bit() const override;
   bool CanStoreAlphaChannels() const override;
   bool CanStoreResolution() const override;
   bool CanStoreICCProfiles() const override;
   bool CanEditPreferences() const override;
   bool UsesFormatSpecificData() const override;

   bool ValidateFormatSpecificData( const void* data ) const override;
   void DisposeFormatSpecificData( void* data ) const override;

   FileFormatImplementation* Create() const override;

   bool EditPreferences() const override;

   // JPEG format-specific data

   struct FormatOptions
   {
      uint32           signature;
      JPEGImageOptions options;

      FormatOptions();
      FormatOptions( const FormatOptions& );

      static FormatOptions* FromGenericDataBlock( const void* );
   };

   // Default JPEG format options
   static JPEGImageOptions DefaultOptions();

   struct EmbeddingOverrides
   {
      bool overrideICCProfileEmbedding = false;
      bool embedICCProfiles            = false;
   };

   static EmbeddingOverrides DefaultEmbeddingOverrides();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEGFormat_h

// ----------------------------------------------------------------------------
// EOF JPEGFormat.h - Released 2025-04-07T08:53:45Z
