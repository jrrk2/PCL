//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 1.0.5
// ----------------------------------------------------------------------------
// JPEGFormat.cpp - Released 2025-04-07T08:53:45Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "JPEGFormat.h"
#include "JPEGInstance.h"
#include "JPEGPreferencesDialog.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IsoString JPEGFormat::Name() const
{
   return "JPEG";
}

// ----------------------------------------------------------------------------

StringList JPEGFormat::FileExtensions() const
{
   return StringList() << ".jpg" << ".jpeg";
}

// ----------------------------------------------------------------------------

IsoStringList JPEGFormat::MimeTypes() const
{
   // RFC2045,RFC2046
   return IsoStringList() << "image/jpeg" << "image/jpg" << "application/jpeg" << "application/jpg";
}

// ----------------------------------------------------------------------------

uint32 JPEGFormat::Version() const
{
   return 0x103;
}

// ----------------------------------------------------------------------------

String JPEGFormat::Description() const
{
   return
   "<html>"
   "<p>Joint Photographic Experts Group (JPEG) File Interchange Format, JFIF 1.02 Specification.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String JPEGFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>This implementation of the JPEG format is based on the Independent JPEG "
   "Group's (IJG) software library (version 9f), written by "
   "Thomas G. Lane and Guido Vollbeding.</p>"

   "<p>IJG JPEG Software Library (version 9f, released January 14 2024):<br/>"
   "opyright (C) 1991-2024, Thomas G. Lane, Guido Vollbeding.</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2025 Pleiades Astrophoto</p>"

   "<p style=\"white-space:pre;\">"
"\n-------------------------------------------------------------------------------"
"\nJPEG Format Hints             Description"
"\n============================  ================================================="
"\nquality n               ( w)  n is a compression quality level in the range"
"\n                              [0,100]. Lower levels compress more and generate"
"\n                              more artifacts. Higher levels lead to larger"
"\n                              files with less artifacts."
"\n-------------------------------------------------------------------------------"
"\noptimized               ( w)  Use optimized coding tables."
"\n-------------------------------------------------------------------------------"
"\nno-optimized            ( w)  Do not use optimized coding tables."
"\n-------------------------------------------------------------------------------"
"\narithmetic              ( w)  Use arithmetic coding."
"\n-------------------------------------------------------------------------------"
"\nhuffman                 ( w)  Use Huffman coding."
"\n-------------------------------------------------------------------------------"
"\nprogressive             ( w)  Generate a JPEG image that can be loaded as"
"\n                              multiple layers of progressively higher detail."
"\n-------------------------------------------------------------------------------"
"\nno-progressive          ( w)  Generate a baseline JPEG image."
"\n-------------------------------------------------------------------------------"
"\nverbosity n             (rw)  n is a verbosity level in the range [0,3] to"
"\n                              control the amount of generated messages"
"\n                              (default = 1)."
"\n-------------------------------------------------------------------------------"
"\n"
   "</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String JPEGFormat::IconImageSVGFile() const
{
   return "@module_icons_dir/JPEG.svg";
}

// ----------------------------------------------------------------------------

bool JPEGFormat::CanStore16Bit() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool JPEGFormat::CanStoreAlphaChannels() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool JPEGFormat::CanStoreResolution() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool JPEGFormat::CanStoreICCProfiles() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool JPEGFormat::CanEditPreferences() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool JPEGFormat::UsesFormatSpecificData() const
{
   return true; // use format-specific data to manage some special JPEG flags and data
}

// ----------------------------------------------------------------------------

bool JPEGFormat::ValidateFormatSpecificData( const void* data ) const
{
   return FormatOptions::FromGenericDataBlock( data ) != nullptr;
}

// ----------------------------------------------------------------------------

void JPEGFormat::DisposeFormatSpecificData( void* data ) const
{
   if ( FormatOptions* o = FormatOptions::FromGenericDataBlock( data ) )
      delete o;
}

// ----------------------------------------------------------------------------

FileFormatImplementation* JPEGFormat::Create() const
{
   return new JPEGInstance( this );
}

// ----------------------------------------------------------------------------

bool JPEGFormat::EditPreferences() const
{
   JPEGImageOptions options = DefaultOptions();
   EmbeddingOverrides overrides = DefaultEmbeddingOverrides();

   JPEGPreferencesDialog dlg( overrides, options );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      options = dlg.jpegOptions;
      overrides = dlg.overrides;

      Settings::Write( "JPEGQuality",                     options.quality );
      Settings::Write( "JPEGOptimizedCoding",             options.optimizedCoding );
      Settings::Write( "JPEGArithmeticCoding",            options.arithmeticCoding );
      Settings::Write( "JPEGProgressive",                 options.progressive );

      Settings::Write( "JPEGOverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
      Settings::Write( "JPEGEmbedICCProfiles",            overrides.embedICCProfiles );

      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

JPEGImageOptions JPEGFormat::DefaultOptions()
{
   JPEGImageOptions options;
   unsigned u; bool b;

   u = options.quality;
   Settings::Read( "JPEGQuality", u );
   options.quality = uint8( Range( u, 0u, 100u ) );

   b = options.optimizedCoding;
   Settings::Read( "JPEGOptimizedCoding", b );
   options.optimizedCoding = b;

   b = options.arithmeticCoding;
   Settings::Read( "JPEGArithmeticCoding", b );
   options.arithmeticCoding = b;

   b = options.progressive;
   Settings::Read( "JPEGProgressive", b );
   options.progressive = b;

   b = options.JFIFMarker;
   Settings::Read( "JFIFMarker", b );
   options.JFIFMarker = b;

   u = options.JFIFMajorVersion;
   Settings::Read( "JFIFMajorVersion", u );
   options.JFIFMajorVersion = u;

   u = options.JFIFMinorVersion;
   Settings::Read( "JFIFMinorVersion", u );
   options.JFIFMinorVersion = u;

   return options;
}

// ----------------------------------------------------------------------------

JPEGFormat::EmbeddingOverrides JPEGFormat::DefaultEmbeddingOverrides()
{
   EmbeddingOverrides overrides;

   Settings::Read( "JPEGOverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
   Settings::Read( "JPEGEmbedICCProfiles",            overrides.embedICCProfiles );

   return overrides;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define JPEG_SIGNATURE  0x4A504547u

JPEGFormat::FormatOptions::FormatOptions()
   : signature( JPEG_SIGNATURE )
   , options( JPEGFormat::DefaultOptions() )
{
}

JPEGFormat::FormatOptions::FormatOptions( const JPEGFormat::FormatOptions& x )
   : signature( JPEG_SIGNATURE )
   , options( x.options )
{
}

JPEGFormat::FormatOptions* JPEGFormat::FormatOptions::FromGenericDataBlock( const void* data )
{
   if ( data == nullptr )
      return nullptr;
   const JPEGFormat::FormatOptions* o = reinterpret_cast<const JPEGFormat::FormatOptions*>( data );
   if ( o->signature != JPEG_SIGNATURE )
      return nullptr;
   return const_cast<JPEGFormat::FormatOptions*>( o );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEGFormat.cpp - Released 2025-04-07T08:53:45Z
