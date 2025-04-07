//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 1.0.10
// ----------------------------------------------------------------------------
// TIFFInstance.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __TIFFInstance_h
#define __TIFFInstance_h

#include "TIFF.h"

#include <pcl/FileFormatImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class TIFFFormat;
class TIFFReadHints;

class TIFFInstance : public FileFormatImplementation
{
public:

   TIFFInstance( const TIFFFormat* );

   virtual ~TIFFInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   void* FormatSpecificData() const override;
   String ImageFormatInfo() const override;

   ICCProfile ReadICCProfile() override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions ) override;
   void Create( const String& filePath, int numberOfImages, const IsoString& hints ) override;
   void SetOptions( const ImageOptions& options ) override;
   void SetFormatSpecificData( const void* data ) override;

   void WriteICCProfile( const ICCProfile& ) override;

   void WriteImage( const Image& ) override;
   void WriteImage( const DImage& ) override;
   void WriteImage( const UInt8Image& ) override;
   void WriteImage( const UInt16Image& ) override;
   void WriteImage( const UInt32Image& ) override;

private:

   AutoPointer<TIFFReader>    m_reader;
   AutoPointer<TIFFWriter>    m_writer;
   AutoPointer<TIFFReadHints> m_readHints;
   bool                       m_queriedOptions = false; // did us query options to the user?
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __TIFFInstance_h

// ----------------------------------------------------------------------------
// EOF TIFFInstance.h - Released 2025-04-07T08:53:45Z
