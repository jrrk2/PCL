//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// FITSFormat.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSFormat_h
#define __FITSFormat_h

#include "FITS.h"

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSFormat : public MetaFileFormat
{
public:

   FITSFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String Status() const override;
   String IconImageSVGFile() const override;
   bool CanReadIncrementally() const override;
   bool CanWriteIncrementally() const override;
   bool CanStore32Bit() const override;
   bool CanStore64Bit() const override;
   bool CanStoreFloat() const override;
   bool CanStoreDouble() const override;
   bool CanStoreResolution() const override;
   bool CanStoreKeywords() const override;
   bool CanStoreICCProfiles() const override;
   bool CanStoreThumbnails() const override;
   bool CanStoreImageProperties() const override;
   bool SupportsMultipleImages() const override;
   bool CanEditPreferences() const override;
   bool UsesFormatSpecificData() const override;
   bool IsDeprecated() const override;
   bool ValidateFormatSpecificData( const void* data ) const override;
   void DisposeFormatSpecificData( void* data ) const override;
   FileFormatImplementation* Create() const override;
   bool EditPreferences() const override;

   // FITS format-specific data

   struct FormatOptions
   {
      uint32            signature;
      FITSImageOptions  options;

      FormatOptions();
      FormatOptions( const FormatOptions& );

      static FormatOptions* FromGenericDataBlock( const void* );
   };

   // Default FITS format options.
   static FITSImageOptions DefaultOptions();

   // Defines how to proceed when there are out of range values in a just-read
   // floating point FITS image.
   enum out_of_range_policy
   {
      OutOfRangePolicy_SetRange,    // use LowerRange() & UpperRange() values
      OutOfRangePolicy_AskIfOut,    // if there are out of range values, ask range and action
      OutOfRangePolicy_AskAlways,   // always ask for range and action (truncate, rescale)
      OutOfRangePolicy_Default = OutOfRangePolicy_SetRange
   };

   // Defines how to fix out of range pixel values in a just-read floating
   // point FITS image.
   enum out_of_range_fix_mode
   {
      OutOfRangeFix_Rescale,        // rescale existing values to [lowerRange,upperRange]
      OutOfRangeFix_Truncate,       // truncate existing values to [lowerRange,upperRange]
      OutOfRangeFix_Ignore,         // allow (do not fix) out-of-range values
      OutOfRangeFix_Default = OutOfRangeFix_Rescale
   };

   struct OutOfRangePolicyOptions
   {
      double                lowerRange        = 0;
      double                upperRange        = 1;
      out_of_range_policy   outOfRangePolicy  = OutOfRangePolicy_Default;
      out_of_range_fix_mode outOfRangeFixMode = OutOfRangeFix_Default;
   };

   static OutOfRangePolicyOptions DefaultOutOfRangePolicyOptions();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSFormat_h

// ----------------------------------------------------------------------------
// EOF FITSFormat.h - Released 2025-04-07T08:53:45Z
