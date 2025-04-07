//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// JPEG2000Format.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEG2000Format_h
#define __JPEG2000Format_h

#include "JPEG2000Instance.h"

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * JPEG-2000 Code Stream Format
 */
class JPCFormat : public MetaFileFormat
{
public:

   JPCFormat() = default;

   IsoString Name() const override;

   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;

   uint32 Version() const override;

   String Description() const override;
   String Implementation() const override;

   String IconImageSVGFile() const override;

   bool CanEditPreferences() const override;
   bool UsesFormatSpecificData() const override;

   bool ValidateFormatSpecificData( const void* data ) const override;
   void DisposeFormatSpecificData( void* data ) const override;

   FileFormatImplementation* Create() const override;

   bool EditPreferences() const override;

   static JPEG2000ImageOptions DefaultOptions();
};

// ----------------------------------------------------------------------------

/*
 * JPEG-2000 JP2 Format
 */
class JP2Format : public JPCFormat
{
public:

   JP2Format() = default;

   IsoString Name() const override;

   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;

   String Description() const override;

   String IconImageSVGFile() const override;

   bool CanStoreICCProfiles() const override;
   bool CanStoreResolution() const override;

   FileFormatImplementation* Create() const override;

   bool EditPreferences() const override;

   static JPEG2000ImageOptions DefaultOptions();

   struct EmbeddingOverrides
   {
      bool overrideICCProfileEmbedding = false;
      bool embedICCProfiles            = false;
   };

   static EmbeddingOverrides DefaultEmbeddingOverrides();
};

// ----------------------------------------------------------------------------

/*
 * JPEG-2000 code stream format-specific data
 */
struct JPEG2000FormatOptions
{
   uint32               signature;
   JPEG2000ImageOptions options;

   JPEG2000FormatOptions( bool isCodeStream );
   JPEG2000FormatOptions( const JPEG2000FormatOptions& );

   static JPEG2000FormatOptions* FromGenericDataBlock( const void* );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEG2000Format_h

// ----------------------------------------------------------------------------
// EOF JPEG2000Format.h - Released 2025-04-07T08:53:45Z
