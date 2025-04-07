//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 1.6.3
// ----------------------------------------------------------------------------
// RawInstance.h - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RawInstance_h
#define __RawInstance_h

#include <pcl/FileFormatImplementation.h>

#include "RawPreferences.h"

class LibRaw;

#define RAW_DEFAULT_VERBOSITY    2
#define RAW_TIMEOFFSET_UNKNOWN   int32_min
#define RAW_GPS_UNKNOWN          -1000

namespace pcl
{

// ----------------------------------------------------------------------------

class RawFormat;
class RawProgress;

class RawInstance : public FileFormatImplementation
{
public:

   RawInstance( const RawFormat* );

   virtual ~RawInstance() noexcept( false );

   ImageDescriptionArray Open( const String& filePath, const IsoString& hints ) override;
   bool IsOpen() const override;
   String FilePath() const override;
   void Close() override;

   String ImageFormatInfo() const override;

   ICCProfile ReadICCProfile() override;

   // We synthesize critical FITS keywords: TIME-OBS, EXPTIME, FOCALLEN, APTDIA
   FITSKeywordArray ReadFITSKeywords() override;

   PropertyDescriptionArray ImagePropertyDescriptions() override;
   Variant ReadImageProperty( const IsoString& property ) override;

   void ReadImage( Image& ) override;
   void ReadImage( DImage& ) override;
   void ReadImage( UInt8Image& ) override;
   void ReadImage( UInt16Image& ) override;
   void ReadImage( UInt32Image& ) override;

   UInt8Image ReadThumbnail() override;

private:

   RawPreferences           m_preferences;
   int                      m_verbosity = RAW_DEFAULT_VERBOSITY;
   String                   m_filePath;
   AutoPointer<LibRaw>      m_raw;
   AutoPointer<RawProgress> m_progress;

   String                   m_description;
   String                   m_author;
   IsoString                m_cameraManufacturer;
   IsoString                m_cameraModel;
   IsoString                m_cfaPattern;
   IsoString                m_rawCFAPattern;
   IsoString                m_cfaPatternName;
   F32Matrix                m_sRGBConversionMatrix;
   TimePoint                m_timestamp;
   int                      m_timeOffset = RAW_TIMEOFFSET_UNKNOWN; // in minutes
   double                   m_gpsLongitude = RAW_GPS_UNKNOWN; // in degrees
   double                   m_gpsLatitude = RAW_GPS_UNKNOWN; // in degrees
   float                    m_gpsAltitude = RAW_GPS_UNKNOWN; // in meters
   float                    m_exposure = 0;
   int                      m_isoSpeed = 0;
   float                    m_focalLength = 0;
   float                    m_aperture = 0;

   String ExposureAsText() const;

   void CheckOpenStream( const String& ) const;

   static void CheckLibRawReturnCode( int );
   static void LibRawEXIFCallback( void*, int, int, int, unsigned, void*, int64 );

   friend class RawReadHints;
   friend class RawProgress;
   friend class RawImageReader;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RawInstance_h

// ----------------------------------------------------------------------------
// EOF RawInstance.h - Released 2025-04-07T08:53:45Z
