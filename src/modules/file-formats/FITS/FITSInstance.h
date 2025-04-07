//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 1.2.3
// ----------------------------------------------------------------------------
// FITSInstance.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FITSInstance_h
#define __FITSInstance_h

#include "FITS.h"

#include <pcl/AutoPointer.h>
#include <pcl/FileFormatImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSFormat;
class FITSReadHints;

class FITSInstance : public FileFormatImplementation
{
public:

   FITSInstance( const FITSFormat* );

   virtual ~FITSInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   void SelectImage( int index ) override;
   int SelectedImageIndex() const override;

   void* FormatSpecificData() const override;

   FITSKeywordArray ReadFITSKeywords() override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   void ReadSamples( Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel ) override;

   bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions ) override;
   void Create( const String& filePath, int numberOfImages, const IsoString& hints ) override;
   void SetId( const IsoString& id ) override;
   void SetOptions( const ImageOptions& options ) override;
   void SetFormatSpecificData( const void* data ) override;

   void WriteFITSKeywords( const FITSKeywordArray& ) override;

   void WriteImage( const Image& ) override;
   void WriteImage( const DImage& ) override;
   void WriteImage( const UInt8Image& ) override;
   void WriteImage( const UInt16Image& ) override;
   void WriteImage( const UInt32Image& ) override;

   void CreateImage( const ImageInfo& info ) override;

   void WriteSamples( const Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel ) override;

   void CloseImage() override;

private:

   AutoPointer<FITSReader>    m_reader;
   AutoPointer<FITSWriter>    m_writer;
   AutoPointer<FITSReadHints> m_readHints;
   bool                       m_queriedOptions = false;
   bool                       m_shownExtractedKeywordsInfo = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FITSInstance_h

// ----------------------------------------------------------------------------
// EOF FITSInstance.h - Released 2025-04-07T08:53:45Z
