//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.1.3
// ----------------------------------------------------------------------------
// XISFFormat.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __XISFFormat_h
#define __XISFFormat_h

#include <pcl/MetaFileFormat.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFFormat : public MetaFileFormat
{
public:

   XISFFormat();

   IsoString Name() const override;

   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;

   uint32 Version() const override;

   String Description() const override;
   String Implementation() const override;

   String IconImageSVGFile() const override;

   bool CanReadIncrementally() const override;
   bool CanWriteIncrementally() const override;
   bool CanStore32Bit() const override;
   bool CanStore64Bit() const override;
   bool CanStoreFloat() const override;
   bool CanStoreDouble() const override;
   bool CanStoreComplex() const override;
   bool CanStoreDComplex() const override;
   bool CanStoreResolution() const override;
   bool CanStoreKeywords() const override;
   bool CanStoreICCProfiles() const override;
   bool CanStoreThumbnails() const override;
   bool CanStoreProperties() const override;
   bool CanStoreImageProperties() const override;
   bool CanStoreRGBWS() const override;
   bool CanStoreDisplayFunctions() const override;
   bool CanStoreColorFilterArrays() const override;
   bool SupportsCompression() const override;
   bool SupportsMultipleImages() const override;
   bool SupportsViewProperties() const override;
   bool CanEditPreferences() const override;
   bool UsesFormatSpecificData() const override;

   FileFormatImplementation* Create() const override;

   bool EditPreferences() const override;

   bool ValidateFormatSpecificData( const void* data ) const override;
   void DisposeFormatSpecificData( void* data ) const override;

   struct FormatOptions
   {
      XISFOptions options;

      FormatOptions();
      FormatOptions( const FormatOptions& ) = default;

      static FormatOptions* FromGenericDataBlock( const void* );

   private:

      uint32 signature;
   };

   static XISFOptions DefaultOptions();

   struct EmbeddingOverrides
   {
      bool overridePropertyEmbedding            = false;
      bool embedProperties                      = true;
      bool overrideICCProfileEmbedding          = false;
      bool embedICCProfiles                     = false;
      bool overrideDisplayFunctionEmbedding     = false;
      bool embedDisplayFunctions                = false;
      bool overrideRGBWorkingSpaceEmbedding     = false;
      bool embedRGBWorkingSpaces                = false;
      bool overrideThumbnailEmbedding           = false;
      bool embedThumbnails                      = false;
      bool overrideProcessingHistoriesEmbedding = false;
      bool embedProcessingHistories             = false;
      bool overridePreviewRectsEmbedding        = false;
      bool embedPreviewRects                    = false;
   };

   static EmbeddingOverrides DefaultEmbeddingOverrides();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __XISFFormat_h

// ----------------------------------------------------------------------------
// EOF XISFFormat.h - Released 2025-04-07T08:53:45Z
