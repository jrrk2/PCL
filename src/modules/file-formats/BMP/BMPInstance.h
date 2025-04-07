//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// BMPInstance.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BMPInstance_h
#define __BMPInstance_h

#include <pcl/AutoPointer.h>
#include <pcl/FileFormatImplementation.h>

#include "BMPFormat.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Bitmap;

class BitmapInstance : public FileFormatImplementation
{
public:

   BitmapInstance( const MetaFileFormat* F )
      : FileFormatImplementation( F )
   {
   }

   virtual ~BitmapInstance() noexcept( false )
   {
      Close();
   }

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Create( const String& filePath, int numberOfImages, const IsoString& hints ) override;
   void Close() override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   void WriteImage( const Image& ) override;
   void WriteImage( const DImage& ) override;
   void WriteImage( const UInt8Image& ) override;
   void WriteImage( const UInt16Image& ) override;
   void WriteImage( const UInt32Image& ) override;

   virtual int DefaultQuality() const
   {
      return -1; // format-dependent default
   }

protected:

   AutoPointer<Bitmap> m_bitmap;
   String              m_path;
   ImageInfo           m_info;
   IsoString           m_hints;
};

// ----------------------------------------------------------------------------

class BMPInstance : public BitmapInstance
{
public:

   BMPInstance( const BMPFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class GIFInstance : public BitmapInstance
{
public:

   GIFInstance( const GIFFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class ICOInstance : public BitmapInstance
{
public:

   ICOInstance( const ICOFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class MNGInstance : public BitmapInstance
{
public:

   MNGInstance( const MNGFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class PBMInstance : public BitmapInstance
{
public:

   PBMInstance( const PBMFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class PGMInstance : public BitmapInstance
{
public:

   PGMInstance( const PGMFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class PNGInstance : public BitmapInstance
{
public:

   PNGInstance( const PNGFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class PPMInstance : public BitmapInstance
{
public:

   PPMInstance( const PPMFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class XBMInstance : public BitmapInstance
{
public:

   XBMInstance( const XBMFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class XPMInstance : public BitmapInstance
{
public:

   XPMInstance( const XPMFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class TGAInstance : public BitmapInstance
{
public:

   TGAInstance( const TGAFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

class WEBPInstance : public BitmapInstance
{
public:

   WEBPInstance( const WEBPFormat* F ) : BitmapInstance( F )
   {
   }

   virtual int DefaultQuality() const
   {
      return 100; // use WebP lossless compression by default
   }
};

// ----------------------------------------------------------------------------

class SVGInstance : public BitmapInstance
{
public:

   SVGInstance( const SVGFormat* F ) : BitmapInstance( F )
   {
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BMPInstance_h

// ----------------------------------------------------------------------------
// EOF BMPInstance.h - Released 2025-04-07T08:53:45Z
