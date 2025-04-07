//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// BMPFormat.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BMPFormat_h
#define __BMPFormat_h

#include <pcl/MetaFileFormat.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BMPFormat : public MetaFileFormat
{
public:

   BMPFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class GIFFormat : public MetaFileFormat
{
public:

   GIFFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class ICOFormat : public MetaFileFormat
{
public:

   ICOFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class MNGFormat : public MetaFileFormat
{
public:

   MNGFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class PBMFormat : public MetaFileFormat
{
public:

   PBMFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class PGMFormat : public MetaFileFormat
{
public:

   PGMFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class PNGFormat : public MetaFileFormat
{
public:

   PNGFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class PPMFormat : public MetaFileFormat
{
public:

   PPMFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class XBMFormat : public MetaFileFormat
{
public:

   XBMFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class XPMFormat : public MetaFileFormat
{
public:

   XPMFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class TGAFormat : public MetaFileFormat
{
public:

   TGAFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class WEBPFormat : public MetaFileFormat
{
public:

   WEBPFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

class SVGFormat : public MetaFileFormat
{
public:

   SVGFormat() = default;

   IsoString Name() const override;
   StringList FileExtensions() const override;
   IsoStringList MimeTypes() const override;
   uint32 Version() const override;
   String Description() const override;
   String Implementation() const override;
   String IconImageSVGFile() const override;
   bool CanRead() const override;
   bool CanWrite() const override;
   bool CanStore8Bit() const override;
   bool CanStore16Bit() const override;
   FileFormatImplementation* Create() const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BMPFormat_h

// ----------------------------------------------------------------------------
// EOF BMPFormat.h - Released 2025-04-07T08:53:45Z
