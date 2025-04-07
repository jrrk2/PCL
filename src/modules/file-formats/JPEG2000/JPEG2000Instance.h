//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 1.0.3
// ----------------------------------------------------------------------------
// JPEG2000Instance.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __JPEG2000Instance_h
#define __JPEG2000Instance_h

#include <pcl/FileFormatImplementation.h>

#include "jasper/jasper.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class JPCFormat;
class JP2Format;

/*
 * JPEG2000 Format-Specific Data
 */
struct JPEG2000ProgressionOrder
{
   enum { LRCP, RLCP, RPCL, PCRL, CPRL }; // see JasPer documentation
};

struct JPEG2000ImageOptions
{
   bool  lossyCompression; // use lossy JPEG2000 compression
   float compressionRate;  // compressed/original: 1.0 = no compression
   bool  signedSample;     // signed sample values
   bool  resolutionData;   // include resolution information
   bool  tiledImage;       // partition the image into tiles
   int   tileWidth;        // tile width in pixels
   int   tileHeight;       // tile height in pixels
   int   numberOfLayers;   // > 1 -> progressive JPEG2000
   int   progressionOrder; // use JasPer code

   JPEG2000ImageOptions()
   {
      Reset();
   }

   JPEG2000ImageOptions( const JPEG2000ImageOptions& ) = default;

   void Reset()
   {
      lossyCompression = false;
      compressionRate  = 0.1F;
      signedSample     = false;
      resolutionData   = true;
      tiledImage       = false;
      tileWidth        = 256;
      tileHeight       = 256;
      numberOfLayers   = 1;
      progressionOrder = JPEG2000ProgressionOrder::LRCP;
   }
};

/*
 * JPEG-2000 Code Stream Format Instance.
 */
class JPCInstance : public FileFormatImplementation
{
public:

   JPCInstance( const JPCFormat* );

   virtual ~JPCInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   void* FormatSpecificData() const override;
   String ImageFormatInfo() const override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions ) override;
   void Create( const String& filePath, int numberOfImages, const IsoString& hints ) override;
   void SetOptions( const ImageOptions& options ) override;
   void SetFormatSpecificData( const void* data ) override;

   void WriteImage( const Image& ) override;
   void WriteImage( const DImage& ) override;
   void WriteImage( const UInt8Image& ) override;
   void WriteImage( const UInt16Image& ) override;
   void WriteImage( const UInt32Image& ) override;

   bool WasLossyWrite() const override;

protected:

   // Parameters of the image being read/written
   String               m_path;       // current file path
   ImageOptions         m_options;    // format-independent options
   JPEG2000ImageOptions m_jp2Options; // format-specific options

   // JasPer objects
   jas_stream_t*        m_jp2Stream = nullptr;
   jas_image_t*         m_jp2Image = nullptr;

   // Embedded ICC profile, JP2 format only.
   jas_cmprof_t*        m_jp2CMProfile = nullptr; // JasPer's color management, incl. ICC profile

   // Did us query options to the user?
   bool                 m_queriedOptions = false;

   void CreateImage( const ImageInfo& );

   virtual bool IsCodeStream() const
   {
      return true;
   }
};

/*
 * JPEG-2000 Format Instance.
 */
class JP2Instance : public JPCInstance
{
public:

   JP2Instance( const JP2Format* );

   virtual ~JP2Instance() noexcept( false );

   ICCProfile ReadICCProfile() override;
   void WriteICCProfile( const ICCProfile& ) override;

private:

   bool IsCodeStream() const override
   {
      return false;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __JPEG2000Instance_h

// ----------------------------------------------------------------------------
// EOF JPEG2000Instance.h - Released 2025-04-07T08:53:45Z
