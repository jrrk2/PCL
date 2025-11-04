//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/FileFormatInstance.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/AutoLock.h>
#include <pcl/ColorFilterArray.h>
#include <pcl/DisplayFunction.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ICCProfile.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void APIHackingAttempt( const String& routineId )
{
   throw Error( "Possible hacking attempt detected in low-level API call: " + String( "FileFormat->" ) + routineId );
}

// ----------------------------------------------------------------------------

class FileFormatInstancePrivate
{
public:

   template <class I>
   static bool ReadImage( FileFormatInstance* instance, I& image )
   {
      if ( !image.IsShared() )
      {
         I tmp( (void*)0, 0, 0 );
         if ( !ReadImage( instance, tmp ) )
            return false;
         image.Assign( tmp );
         return true;
      }

      if ( API_FileFormat_ReadImage( instance->handle, image.Allocator().Handle() ) == api_false )
         return false;
      image.Synchronize();
      return true;
   }

   template <class I>
   static bool WriteImage( FileFormatInstance* instance, const I& image )
   {
      if ( !image.IsShared() || !image.IsCompletelySelected() )
      {
         I tmp( (void*)0, 0, 0 );
         tmp.Assign( image );
         return WriteImage( instance, tmp );
      }

      image.PushSelections();
      image.ResetSelections();

      ImageInfo info1( image );

      image.PopSelections();

      bool ok = API_FileFormat_WriteImage( instance->handle, image.Allocator().Handle() ) != api_false;

      image.PushSelections();
      image.ResetSelections();

      ImageInfo info2( image );

      image.PopSelections();

      if ( info1 != info2 )
         APIHackingAttempt( "WriteImage" );

      return ok;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FileFormatInstance::FileFormatInstance( const FileFormat& fmt )
   : UIObject( API_FileFormat_CreateFileFormatInstance( ModuleHandle(), fmt.Handle() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateFileFormatInstance" );
}

// ----------------------------------------------------------------------------

FileFormatInstance& FileFormatInstance::Null()
{
   static FileFormatInstance* nullInstance = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullInstance == nullptr )
      nullInstance = new FileFormatInstance( nullptr );
   return *nullInstance;
}

// ----------------------------------------------------------------------------

FileFormat FileFormatInstance::Format() const
{
   return FileFormat( (const void*)API_FileFormat_GetFileFormatInstanceFormat( handle ) );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Close()
{
   return API_FileFormat_CloseImageFile( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::IsOpen() const
{
   return API_FileFormat_IsImageFileOpen( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String FileFormatInstance::FilePath() const
{
   size_type len = 0;
   API_FileFormat_GetImageFilePath( handle, 0, &len );

   String path;
   if ( len > 0 )
   {
      path.SetLength( len );
      if ( API_FileFormat_GetImageFilePath( handle, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageFilePath" );
      path.ResizeToNullTerminated();
   }
   return path;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Open( ImageDescriptionArray& images,
                               const String& filePath, const IsoString& hints )
{
   images.Clear();

   if ( API_FileFormat_OpenImageFileEx( handle, filePath.c_str(), hints.c_str(), 0/*flags*/ ) == api_false )
      return false;

   for ( uint32 i = 0, n = API_FileFormat_GetImageCount( handle ); i < n; ++i )
   {
      IsoString id;
      size_type len = 0;
      API_FileFormat_GetImageId( handle, 0, &len, i );
      if ( len > 0 )
      {
         id.SetLength( len );
         if ( API_FileFormat_GetImageId( handle, id.Begin(), &len, i ) == api_false )
            throw APIFunctionError( "GetImageId" );
         id.ResizeToNullTerminated();
      }

      api_image_info info;
      api_image_options options;
      if ( API_FileFormat_GetImageDescription( handle, &info, &options, i ) == api_false )
         throw APIFunctionError( "GetImageDescription" );

      ImageDescription d;
      d.id = id;
      APIImageInfoToPCL( d.info, info );
      APIImageOptionsToPCL( d.options, options );
      images.Add( d );
   }

   return true;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SelectImage( int index )
{
   return API_FileFormat_SelectImage( handle, uint32( index ) ) != api_false;
}

// ----------------------------------------------------------------------------

int FileFormatInstance::SelectedImageIndex() const
{
   return int( API_FileFormat_GetSelectedImageIndex( handle ) );
}

// ----------------------------------------------------------------------------

const void* FileFormatInstance::FormatSpecificData() const
{
   return API_FileFormat_GetFormatSpecificData( handle );
}

// ----------------------------------------------------------------------------

String FileFormatInstance::ImageFormatInfo() const
{
   size_type len = 0;
   API_FileFormat_GetImageFormatInfo( handle, 0, &len );

   String properties;
   if ( len > 0 )
   {
      properties.SetLength( len );
      if ( API_FileFormat_GetImageFormatInfo( handle, properties.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageFormatInfo" );
      properties.ResizeToNullTerminated();
   }
   return properties;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadICCProfile( ICCProfile& icc )
{
   try
   {
      icc.Clear();

      if ( API_FileFormat_BeginICCProfileExtraction( handle ) == api_false )
         return false;

      const void* iccData = API_FileFormat_GetICCProfile( handle );
      if ( iccData != 0 )
         icc.Set( iccData );

      API_FileFormat_EndICCProfileExtraction( handle );
      return true;
   }
   catch ( ... )
   {
      API_FileFormat_EndICCProfileExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadRGBWorkingSpace( RGBColorSystem& rgbws )
{
   try
   {
      rgbws = RGBColorSystem();

      if ( API_FileFormat_BeginRGBWSExtraction( handle ) == api_false )
         return false;

      float gamma = rgbws.Gamma();
      api_bool issRGB = rgbws.IsSRGB();
      FVector x = rgbws.ChromaticityXCoordinates();
      FVector y = rgbws.ChromaticityYCoordinates();
      FVector Y = rgbws.LuminanceCoefficients();

      bool ok = API_FileFormat_GetImageRGBWS( handle, &gamma, &issRGB, x.Begin(), y.Begin(), Y.Begin() ) != api_false;
      if ( ok )
         rgbws = RGBColorSystem( gamma, issRGB, x, y, Y );

      API_FileFormat_EndRGBWSExtraction( handle );
      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndRGBWSExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool  FileFormatInstance::ReadDisplayFunction( DisplayFunction& df )
{
   try
   {
      df = DisplayFunction();

      if ( API_FileFormat_BeginDisplayFunctionExtraction( handle ) == api_false )
         return false;

      DVector m, s, h, l, r;
      df.GetDisplayFunctionParameters( m, s, h, l, r );

      bool ok = API_FileFormat_GetImageDisplayFunction( handle, m.Begin(), s.Begin(), h.Begin(), l.Begin(), r.Begin() ) != api_false;
      if ( ok )
         df = DisplayFunction( m, s, h, l, r );

      API_FileFormat_EndDisplayFunctionExtraction( handle );
      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndDisplayFunctionExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadColorFilterArray( ColorFilterArray& cfa )
{
   try
   {
      cfa = ColorFilterArray();

      if ( API_FileFormat_BeginColorFilterArrayExtraction( handle ) == api_false )
         return false;

      size_type patternLen = 0, nameLen = 0;
      API_FileFormat_GetImageColorFilterArray( handle, 0, &patternLen, 0, 0, 0, &nameLen );

      bool ok = true;
      if ( patternLen > 0 )
      {
         IsoString pattern;
         pattern.SetLength( patternLen );
         String name;
         if ( nameLen > 0 )
            name.SetLength( nameLen );
         int32 width, height;
         ok = API_FileFormat_GetImageColorFilterArray( handle,
                              pattern.Begin(), &patternLen, &width, &height, name.Begin(), &nameLen ) != api_false;
         if ( ok )
         {
            pattern.ResizeToNullTerminated();
            name.ResizeToNullTerminated();
            cfa = ColorFilterArray( pattern, width, height, name );
         }
      }

      API_FileFormat_EndColorFilterArrayExtraction( handle );
      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndColorFilterArrayExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadThumbnail( pcl::UInt8Image& thumbnail )
{
   try
   {
      thumbnail.FreeData();

      if ( API_FileFormat_BeginThumbnailExtraction( handle ) == api_false )
         return false;

      if ( thumbnail.IsShared() )
      {
         API_FileFormat_GetThumbnail( handle, thumbnail.Allocator().Handle() );
         thumbnail.Synchronize();
      }
      else
      {
         UInt8Image img( (void*)0, 0, 0 );
         API_FileFormat_GetThumbnail( handle, img.Allocator().Handle() );
         img.Synchronize();
         thumbnail.Assign( img );
      }

      API_FileFormat_EndThumbnailExtraction( handle );
      return true;
   }
   catch ( ... )
   {
      API_FileFormat_EndThumbnailExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadFITSKeywords( FITSKeywordArray& keywords )
{
   try
   {
      keywords.Clear();

      if ( API_FileFormat_BeginKeywordExtraction( handle ) == api_false )
         return false;

      for ( size_type i = 0, count = API_FileFormat_GetKeywordCount( handle ); i < count; ++i )
      {
         IsoString name, value, comment;
         name.Reserve( 256 );
         value.Reserve( 256 );
         comment.Reserve( 256 );

         // N.B. The passed maximum length *includes* an ending null character.
         if ( API_FileFormat_GetNextKeyword( handle,
                  name.Begin(), value.Begin(), comment.Begin(), 256 ) == api_false )
            throw APIFunctionError( "GetNextKeyword" );

         name.ResizeToNullTerminated();
         value.ResizeToNullTerminated();
         comment.ResizeToNullTerminated();

         keywords.Add( FITSHeaderKeyword( name, value, comment ) );
      }

      API_FileFormat_EndKeywordExtraction( handle );
      return true;
   }
   catch ( ... )
   {
      API_FileFormat_EndKeywordExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

static api_bool APIPropertyEnumerationCallback( const char* id, uint64 type, void* data )
{
   reinterpret_cast<PropertyDescriptionArray*>( data )->Append(
         PropertyDescription( id, VariantTypeFromAPIPropertyType( type ) ) );
   return api_true;
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray FileFormatInstance::PropertyDescriptions()
{
   PropertyDescriptionArray properties;
   IsoString id;
   size_type len = 0;
   API_FileFormat_EnumerateProperties( handle, 0, 0, &len, 0 ); // 1st call to get max identifier length
   if ( len > 0 )
   {
      id.Reserve( len );
      if ( API_FileFormat_EnumerateProperties( handle, APIPropertyEnumerationCallback,
                                                    id.Begin(), &len, &properties ) == api_false )
         throw APIFunctionError( "EnumerateProperties" );
   }
   return properties;
}

// ----------------------------------------------------------------------------

Variant FileFormatInstance::ReadProperty( const IsoString& property )
{
   try
   {
      if ( API_FileFormat_BeginPropertyExtraction( handle ) == api_false )
         return Variant();

      api_property_value apiValue;
      if ( API_FileFormat_GetProperty( handle, property.c_str(), &apiValue ) == api_false )
      {
         apiValue.data.blockValue = nullptr;
         apiValue.type = VTYPE_INVALID;
      }

      API_FileFormat_EndPropertyExtraction( handle );

      return VariantFromAPIPropertyValue( apiValue );
   }
   catch ( ... )
   {
      API_FileFormat_EndPropertyExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

PropertyArray FileFormatInstance::ReadProperties()
{
   PropertyDescriptionArray descriptions = PropertyDescriptions();
   try
   {
      PropertyArray properties;
      if ( API_FileFormat_BeginPropertyExtraction( handle ) != api_false )
      {
         for ( const pcl::PropertyDescription& description : descriptions )
         {
            api_property_value apiValue;
            if ( API_FileFormat_GetProperty( handle, description.id.c_str(), &apiValue ) != api_false )
               properties << Property( description.id, VariantFromAPIPropertyValue( apiValue ) );
         }

         API_FileFormat_EndPropertyExtraction( handle );
      }
      return properties;
   }
   catch ( ... )
   {
      API_FileFormat_EndPropertyExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray FileFormatInstance::ImagePropertyDescriptions()
{
   PropertyDescriptionArray properties;
   IsoString id;
   size_type len = 0;
   API_FileFormat_EnumerateImageProperties( handle, 0, 0, &len, 0 ); // 1st call to get max identifier length
   if ( len > 0 )
   {
      id.Reserve( len );
      if ( API_FileFormat_EnumerateImageProperties( handle, APIPropertyEnumerationCallback,
                                                         id.Begin(), &len, &properties ) == api_false )
         throw APIFunctionError( "EnumerateImageProperties" );
   }
   return properties;
}

// ----------------------------------------------------------------------------

Variant FileFormatInstance::ReadImageProperty( const IsoString& property )
{
   try
   {
      if ( API_FileFormat_BeginImagePropertyExtraction( handle ) == api_false )
         return Variant();

      api_property_value apiValue;
      if ( API_FileFormat_GetImageProperty( handle, property.c_str(), &apiValue ) == api_false )
      {
         apiValue.data.blockValue = nullptr;
         apiValue.type = VTYPE_INVALID;
      }

      API_FileFormat_EndImagePropertyExtraction( handle );

      return VariantFromAPIPropertyValue( apiValue );
   }
   catch ( ... )
   {
      API_FileFormat_EndImagePropertyExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

PropertyArray FileFormatInstance::ReadImageProperties()
{
   PropertyDescriptionArray descriptions = ImagePropertyDescriptions();
   try
   {
      PropertyArray properties;
      if ( API_FileFormat_BeginImagePropertyExtraction( handle ) != api_false )
      {
         for ( const pcl::PropertyDescription& description : descriptions )
         {
            api_property_value apiValue;
            if ( API_FileFormat_GetImageProperty( handle, description.id.c_str(), &apiValue ) != api_false )
               properties << Property( description.id, VariantFromAPIPropertyValue( apiValue ) );
         }

         API_FileFormat_EndImagePropertyExtraction( handle );
      }
      return properties;
   }
   catch ( ... )
   {
      API_FileFormat_EndImagePropertyExtraction( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadImage( pcl::Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( pcl::DImage& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt8Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt16Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt32Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CanReadIncrementally() const
{
   return API_FileFormat_CanReadIncrementally( handle ) != api_false;
}

// ----------------------------------------------------------------------------

static bool ReadSamples( file_format_handle handle,
      void* buffer, int startRow, int rowCount, int channel, int bitsPerSample, bool floatSample )
{
   return API_FileFormat_ReadSamples( handle, buffer, startRow, rowCount, channel,
                                          bitsPerSample, floatSample, false ) != api_false;
}

bool FileFormatInstance::ReadSamples( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 32, true );
}

bool FileFormatInstance::ReadSamples( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 64, true );
}

bool FileFormatInstance::ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 8, false );
}

bool FileFormatInstance::ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 16, false );
}

bool FileFormatInstance::ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::ReadSamples( handle, buffer, startRow, rowCount, channel, 32, false );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WasInexactRead() const
{
   return API_FileFormat_WasInexactRead( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::QueryOptions( Array<ImageOptions>& options, Array<const void*>& formatOptions )
{
   uint32 n = uint32( options.Length() );

   Array<api_image_options> o( (size_type)n );
   for ( uint32 i = 0; i < n; ++i )
      PCLImageOptionsToAPI( o[i], options[i] );

   if ( API_FileFormat_QueryImageFileOptions( handle, o.Begin(), formatOptions.Begin(), n ) == api_false )
      return false;

   for ( uint32 i = 0; i < n; ++i )
      APIImageOptionsToPCL( options[i], o[i] );
   return true;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Create( const String& filePath, const IsoString& hints, int count )
{
   return API_FileFormat_CreateImageFileEx( handle, filePath.c_str(), uint32( count ),
                                                 hints.c_str(), 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetId( const IsoString& id )
{
   return API_FileFormat_SetImageId( handle, id.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetOptions( const ImageOptions& options )
{
   api_image_options o;
   PCLImageOptionsToAPI( o, options );
   return API_FileFormat_SetImageOptions( handle, &o ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetFormatSpecificData( const void* data )
{
   return API_FileFormat_SetFormatSpecificData( handle, data ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteICCProfile( const ICCProfile& icc )
{
   try
   {
      if ( API_FileFormat_BeginICCProfileEmbedding( handle ) == api_false )
         return false;

      bool ok = true;

      if ( icc.IsProfile() ) // ### should allow embedding empty profiles ?
      {
         ICCProfile safeCopy = icc;
         ok = API_FileFormat_SetICCProfile( handle, safeCopy.ProfileData().Begin() ) != api_false;

         if ( !safeCopy.IsSameProfile( icc ) || safeCopy.FilePath() != icc.FilePath() )
            APIHackingAttempt( "SetICCProfile" );
      }

      API_FileFormat_EndICCProfileEmbedding( handle );

      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndICCProfileEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteRGBWorkingSpace( const RGBColorSystem& rgbws )
{
   try
   {
      if ( API_FileFormat_BeginRGBWSEmbedding( handle ) == api_false )
         return false;

      float gamma = rgbws.Gamma();
      api_bool issRGB = rgbws.IsSRGB();
      FVector x = rgbws.ChromaticityXCoordinates();
      FVector y = rgbws.ChromaticityYCoordinates();
      FVector Y = rgbws.LuminanceCoefficients();

      bool ok = API_FileFormat_SetImageRGBWS( handle, gamma, issRGB, x.Begin(), y.Begin(), Y.Begin() ) != api_false;

      if ( rgbws.Gamma() != gamma ||
           rgbws.IsSRGB() != (issRGB != api_false) ||
           rgbws.ChromaticityXCoordinates() != x ||
           rgbws.ChromaticityYCoordinates() != y ||
           rgbws.LuminanceCoefficients() != Y )
      {
         APIHackingAttempt( "WriteRGBWorkingSpace" );
      }

      API_FileFormat_EndRGBWSEmbedding( handle );

      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndRGBWSEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteDisplayFunction( const DisplayFunction& df )
{
   try
   {
      if ( API_FileFormat_BeginDisplayFunctionEmbedding( handle ) == api_false )
         return false;

      DVector m, s, h, l, r;
      df.GetDisplayFunctionParameters( m, s, h, l, r );

      bool ok = API_FileFormat_SetImageDisplayFunction( handle, m.Begin(), s.Begin(), h.Begin(), l.Begin(), r.Begin() ) != api_false;

      DVector m1, s1, h1, l1, r1;
      df.GetDisplayFunctionParameters( m1, s1, h1, l1, r1 );
      if ( m1 != m || s1 != s || h1 != h || l1 != l || r1 != r )
         APIHackingAttempt( "WriteDisplayFunction" );

      API_FileFormat_EndDisplayFunctionEmbedding( handle );
      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndDisplayFunctionEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteColorFilterArray( const ColorFilterArray& cfa )
{
   try
   {
      if ( API_FileFormat_BeginColorFilterArrayEmbedding( handle ) == api_false )
         return false;

      IsoString pattern = cfa.Pattern();
      pattern.EnsureUnique();
      String name = cfa.Name();
      name.EnsureUnique();

      bool ok = API_FileFormat_SetImageColorFilterArray( handle,
                           pattern.c_str(), cfa.Width(), cfa.Height(), name.c_str() ) != api_false;

      if ( cfa.Pattern() != pattern || cfa.Name() != name )
         APIHackingAttempt( "WriteColorFilterArray" );

      API_FileFormat_EndColorFilterArrayEmbedding( handle );
      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndColorFilterArrayEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteThumbnail( const pcl::UInt8Image& thumbnail )
{
   try
   {
      if ( API_FileFormat_BeginThumbnailEmbedding( handle ) == api_false )
         return false;

      thumbnail.PushSelections();
      thumbnail.ResetSelections();

      ImageInfo info( thumbnail );

      thumbnail.PopSelections();

      bool ok;

      if ( thumbnail.IsShared() )
         ok = API_FileFormat_SetThumbnail( handle, thumbnail.Allocator().Handle() ) != api_false;
      else
      {
         UInt8Image tmp( (void*)0, 0, 0 );
         tmp.Assign( thumbnail );
         ok = API_FileFormat_SetThumbnail( handle, tmp.Allocator().Handle() ) != api_false;
      }

      thumbnail.PushSelections();
      thumbnail.ResetSelections();

      if ( info != ImageInfo( thumbnail ) )
         APIHackingAttempt( "SetThumbnail" );

      thumbnail.PopSelections();

      API_FileFormat_EndThumbnailEmbedding( handle );

      return ok;
   }
   catch ( ... )
   {
      if ( thumbnail.CanPopSelections() )
         thumbnail.PopSelections();
      API_FileFormat_EndThumbnailEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteFITSKeywords( const FITSKeywordArray& keywords )
{
   try
   {
      if ( API_FileFormat_BeginKeywordEmbedding( handle ) == api_false )
         return false;

      bool ok = true;

      for ( const FITSHeaderKeyword& k : keywords )
         if ( API_FileFormat_AddKeyword( handle, k.name.c_str(), k.value.c_str(), k.comment.c_str() ) == api_false )
         {
            ok = false;
            break;
         }

      API_FileFormat_EndKeywordEmbedding( handle );

      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndKeywordEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteProperty( const IsoString& property, const Variant& value )
{
   try
   {
      if ( API_FileFormat_BeginPropertyEmbedding( handle ) == api_false )
         return false;

      bool ok = true;

      if ( value.IsValid() )
      {
         api_property_value apiValue;
         APIPropertyValueFromVariant( apiValue, value );
         api_property_value safeCopy = apiValue;
         ok = API_FileFormat_SetProperty( handle, property.c_str(), &safeCopy ) != api_false;

         if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
              safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
              safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
         {
            APIHackingAttempt( "SetProperty" );
         }
      }

      API_FileFormat_EndPropertyEmbedding( handle );

      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndPropertyEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteProperties( const PropertyArray& properties )
{
   try
   {
      if ( API_FileFormat_BeginPropertyEmbedding( handle ) == api_false )
         return false;

      for ( const Property& property : properties )
         if ( property.IsValid() )
         {
            api_property_value apiValue;
            APIPropertyValueFromVariant( apiValue, property.Value() );
            api_property_value safeCopy = apiValue;
            bool ok = API_FileFormat_SetProperty( handle, property.Id().c_str(), &safeCopy ) != api_false;

            if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
                 safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
                 safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
            {
               APIHackingAttempt( "SetProperty" );
            }

            if ( !ok )
            {
               API_FileFormat_EndPropertyEmbedding( handle );
               return false;
            }
         }

      API_FileFormat_EndPropertyEmbedding( handle );
      return true;
   }
   catch ( ... )
   {
      API_FileFormat_EndPropertyEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImageProperty( const IsoString& property, const Variant& value )
{
   try
   {
      if ( API_FileFormat_BeginImagePropertyEmbedding( handle ) == api_false )
         return false;

      bool ok = true;

      if ( value.IsValid() )
      {
         api_property_value apiValue;
         APIPropertyValueFromVariant( apiValue, value );
         api_property_value safeCopy = apiValue;
         ok = API_FileFormat_SetImageProperty( handle, property.c_str(), &safeCopy ) != api_false;

         if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
              safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
              safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
         {
            APIHackingAttempt( "SetImageProperty" );
         }
      }

      API_FileFormat_EndImagePropertyEmbedding( handle );

      return ok;
   }
   catch ( ... )
   {
      API_FileFormat_EndImagePropertyEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImageProperties( const PropertyArray& properties )
{
   try
   {
      if ( API_FileFormat_BeginImagePropertyEmbedding( handle ) == api_false )
         return false;

      for ( const Property& property : properties )
         if ( property.IsValid() )
         {
            api_property_value apiValue;
            APIPropertyValueFromVariant( apiValue, property.Value() );
            api_property_value safeCopy = apiValue;
            bool ok = API_FileFormat_SetImageProperty( handle, property.Id().c_str(), &safeCopy ) != api_false;

            if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
                 safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
                 safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
            {
               APIHackingAttempt( "SetImageProperty" );
            }

            if ( !ok )
            {
               API_FileFormat_EndImagePropertyEmbedding( handle );
               return false;
            }
         }

      API_FileFormat_EndImagePropertyEmbedding( handle );
      return true;
   }
   catch ( ... )
   {
      API_FileFormat_EndImagePropertyEmbedding( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImage( const pcl::Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const pcl::DImage& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt8Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt16Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt32Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CanWriteIncrementally() const
{
   return API_FileFormat_CanWriteIncrementally( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CreateImage( const ImageInfo& info )
{
   api_image_info i;
   PCLImageInfoToAPI( i, info );
   return API_FileFormat_CreateImage( handle, &i ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CloseImage()
{
   return API_FileFormat_CloseImage( handle ) != api_false;
}

// ----------------------------------------------------------------------------

static bool WriteSamples( file_format_handle handle,
   const void* buffer, int startRow, int rowCount, int channel, int bitsPerSample, bool floatSample )
{
   return API_FileFormat_WriteSamples( handle, buffer, startRow, rowCount, channel,
                                             bitsPerSample, floatSample, false ) != api_false;
}

bool FileFormatInstance::WriteSamples( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 32, true );
}

bool FileFormatInstance::WriteSamples( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 64, true );
}

bool FileFormatInstance::WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 8, false );
}

bool FileFormatInstance::WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 16, false );
}

bool FileFormatInstance::WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return pcl::WriteSamples( handle, buffer, startRow, rowCount, channel, 32, false );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WasLossyWrite() const
{
   return API_FileFormat_WasLossyWrite( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void* FileFormatInstance::CloneHandle() const
{
   throw Error( "FileFormatInstance::CloneHandle(): Cannot clone a file format instance handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FileFormatInstance.cpp - Released 2025-04-07T08:53:32Z
