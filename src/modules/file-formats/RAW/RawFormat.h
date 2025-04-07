//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawFormat.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RawFormat_h
#define __RawFormat_h

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class RawFormat : public MetaFileFormat
{
public:

   RawFormat() = default;

   IsoString Name() const override;

   StringList FileExtensions() const override;

   uint32 Version() const override;

   String Description() const override;
   String Implementation() const override;

   String IconImageSVGFile() const override;

   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStoreAlphaChannels() const override;
   bool CanStoreResolution() const override;
   bool CanStoreKeywords() const override;
   bool CanStoreICCProfiles() const override;
   bool CanStoreThumbnails() const override;
   bool CanStoreImageProperties() const override;
   bool SupportsViewProperties() const override;
   bool CanEditPreferences() const override;

   FileFormatImplementation* Create() const override;

   bool EditPreferences() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RawFormat_h

// ----------------------------------------------------------------------------
// EOF RawFormat.h - Released 2025-04-07T08:53:45Z
