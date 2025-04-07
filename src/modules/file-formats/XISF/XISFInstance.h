//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 1.1.3
// ----------------------------------------------------------------------------
// XISFInstance.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __XISFInstance_h
#define __XISFInstance_h

#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/FileFormatImplementation.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFFormat;
class XISFStreamHints;

class XISFInstance : public FileFormatImplementation
{
public:

   XISFInstance( const XISFFormat* );

   virtual ~XISFInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   void SelectImage( int index ) override;
   int SelectedImageIndex() const override;

   void* FormatSpecificData() const override;

   ICCProfile ReadICCProfile() override;
   RGBColorSystem ReadRGBWorkingSpace() override;
   DisplayFunction ReadDisplayFunction() override;
   ColorFilterArray ReadColorFilterArray() override;
   UInt8Image ReadThumbnail() override;
   FITSKeywordArray ReadFITSKeywords() override;
   PropertyDescriptionArray PropertyDescriptions() override;
   Variant ReadProperty( const IsoString& property ) override;
   PropertyDescriptionArray ImagePropertyDescriptions() override;
   Variant ReadImageProperty( const IsoString& property ) override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   bool CanReadIncrementally() const override;

   void ReadSamples( Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel ) override;

   bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions ) override;
   void Create( const String& filePath, int numberOfImages, const IsoString& hints ) override;
   void SetId( const IsoString& ) override;
   void SetOptions( const ImageOptions& ) override;
   void SetFormatSpecificData( const void* ) override;

   void WriteICCProfile( const ICCProfile& ) override;
   void WriteRGBWorkingSpace( const RGBColorSystem& rgbws ) override;
   void WriteDisplayFunction( const DisplayFunction& df ) override;
   void WriteColorFilterArray( const ColorFilterArray& cfa ) override;
   void WriteThumbnail( const UInt8Image& ) override;
   void WriteFITSKeywords( const FITSKeywordArray& ) override;
   void WriteProperty( const IsoString& property, const Variant& value ) override;
   void WriteImageProperty( const IsoString& property, const Variant& value ) override;

   void WriteImage( const Image& ) override;
   void WriteImage( const DImage& ) override;
   void WriteImage( const UInt8Image& ) override;
   void WriteImage( const UInt16Image& ) override;
   void WriteImage( const UInt32Image& ) override;

   void CreateImage( const ImageInfo& ) override;
   void CloseImage() override;

   void WriteSamples( const Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel ) override;
   void WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel ) override;

private:

   AutoPointer<XISFReader>      m_reader;
   AutoPointer<XISFWriter>      m_writer;
   AutoPointer<XISFStreamHints> m_readHints;
   AutoPointer<XISFStreamHints> m_writeHints;
   bool                         m_queriedOptions = false; // did us query options to the user?

   class LogHandler : public XISFLogHandler
   {
   public:

      LogHandler() = default;

      void Init( const String& filePath, bool writing ) override;
      void Log( const String& text, message_type type ) override;
      void Close() override;

   private:

      Console m_console;
   };
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __XISFInstance_h

// ----------------------------------------------------------------------------
// EOF XISFInstance.h - Released 2025-04-07T08:53:45Z
