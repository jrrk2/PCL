//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageWindow.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/ImageWindow.h>
#include <pcl/RGBColorSystem.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class InternalWindowEnumerator
{
public:

   static api_bool api_func Callback( window_handle hW, void* ptrToArray )
   {
      reinterpret_cast<Array<ImageWindow>*>( ptrToArray )->Add( ImageWindow( hW ) );
      return api_true;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageWindow::ImageWindow( int width, int height, int numberOfChannels,
                          int bitsPerSample, bool floatSample, bool color,
                          bool initialProcessing, const IsoString& id )
   : UIObject( API_ImageWindow_CreateImageWindow( width, height, numberOfChannels,
                                                       bitsPerSample, floatSample, color,
                                                       initialProcessing, id.c_str() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateImageWindow" );
}

// ----------------------------------------------------------------------------

ImageWindow& ImageWindow::Null()
{
   static ImageWindow* nullImageWindow = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullImageWindow == nullptr )
      nullImageWindow = new ImageWindow( nullptr );
   return *nullImageWindow;
}

// ----------------------------------------------------------------------------

Array<ImageWindow> ImageWindow::Open( const String& url,
               const IsoString& id, const IsoString& formatHints, bool asACopy, bool allowMessages )
{
   Array<ImageWindow> a;
   if ( API_ImageWindow_LoadImageWindows( url.c_str(), id.c_str(), formatHints.c_str(),
                                 api_bool( asACopy ), api_bool( allowMessages ),
                                 InternalWindowEnumerator::Callback, &a ) == api_false )
      throw APIFunctionError( "LoadImageWindows" );
   return a;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsNew() const
{
   return API_ImageWindow_GetImageWindowNewFlag( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsACopy() const
{
   return API_ImageWindow_GetImageWindowCopyFlag( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String ImageWindow::FilePath() const
{
   size_type len = 0;
   API_ImageWindow_GetImageWindowFilePath( handle, 0, &len );

   String path;
   if ( len > 0 )
   {
      path.SetLength( len );
      if ( API_ImageWindow_GetImageWindowFilePath( handle, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageWindowFilePath" );
      path.ResizeToNullTerminated();
   }
   return path;
}

// ----------------------------------------------------------------------------

String ImageWindow::FileURL() const
{
   size_type len = 0;
   API_ImageWindow_GetImageWindowFileURL( handle, 0, &len );

   String url;
   if ( len > 0 )
   {
      url.SetLength( len );
      if ( API_ImageWindow_GetImageWindowFileURL( handle, url.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageWindowFileURL" );
      url.ResizeToNullTerminated();
   }
   return url;
}

// ----------------------------------------------------------------------------

ImageOptions ImageWindow::FileInfo() const
{
   api_image_file_info a;
   API_ImageWindow_GetImageWindowFileInfo( handle, &a );

   ImageOptions o;
   o.bitsPerSample          = a.bitsPerSample;
   o.ieeefpSampleFormat     = a.ieeefpSampleFormat;
   o.complexSample          = a.complexSample;
   o.signedIntegers         = a.signedIntegers;
   o.metricResolution       = a.metricResolution;
   o.embedICCProfile        = a.embedICCProfile;
   o.embedThumbnail         = a.embedThumbnail;
   o.embedProperties        = a.embedProperties;
   o.embedRGBWS             = a.embedRGBWS;
   o.embedDisplayFunction   = a.embedDisplayFunction;
   o.embedColorFilterArray  = a.embedColorFilterArray;
   o.embedProcessingHistory = a.embedProcessingHistory;
   o.embedPreviewRects      = a.embedPreviewRects;
   o.xResolution            = a.xResolution;
   o.yResolution            = a.yResolution;
   o.isoSpeed               = a.isoSpeed;
   o.exposure               = a.exposure;
   o.aperture               = a.aperture;
   o.focalLength            = a.focalLength;
   o.imageType              = a.imageType;
   o.cfaType                = a.cfaType;
   return o;
}

// ----------------------------------------------------------------------------

ImageWindow::image_type ImageWindow::ImageType() const
{
   return (ImageWindow::image_type)API_ImageWindow_GetImageType( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetImageType( ImageWindow::image_type type, bool notify )
{
   if ( API_ImageWindow_SetImageType( handle, type, api_bool( notify ) ) == api_false )
      throw APIFunctionError( "SetImageType" );
}

// ----------------------------------------------------------------------------

size_type ImageWindow::ModifyCount() const
{
   return API_ImageWindow_GetImageWindowModifyCount( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::Close()
{
   return API_ImageWindow_CloseImageWindow( handle, api_false/*force*/ ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::ForceClose()
{
   if ( API_ImageWindow_CloseImageWindow( handle, api_true/*force*/ ) == api_false )
      throw APIFunctionError( "CloseImageWindow" );
}

// ----------------------------------------------------------------------------

View ImageWindow::MainView() const
{
   return View( API_ImageWindow_GetImageWindowMainView( handle ) );
}

// ----------------------------------------------------------------------------

View ImageWindow::CurrentView() const
{
   return View( API_ImageWindow_GetImageWindowCurrentView( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectView( View& v )
{
   API_ImageWindow_SetImageWindowCurrentView( handle, v.handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::PurgeProperties()
{
   API_ImageWindow_PurgeImageWindowProperties( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsValidView( const View& v ) const
{
   return API_ImageWindow_ValidateImageWindowView( handle, v.handle ) != api_false;
}

// ----------------------------------------------------------------------------

int ImageWindow::NumberOfPreviews() const
{
   return API_ImageWindow_GetPreviewCount( handle );
}

// ----------------------------------------------------------------------------

class InternalPreviewEnumerator
{
public:

   static api_bool api_func Callback( view_handle hV, void* ptrToArray )
   {
      reinterpret_cast<Array<View>*>( ptrToArray )->Add( View( hV ) );
      return api_true;
   }
};

Array<View> ImageWindow::Previews() const
{
   Array<View> a;
   API_ImageWindow_EnumeratePreviews( handle, InternalPreviewEnumerator::Callback, &a );
   return a;
}

// ----------------------------------------------------------------------------

View ImageWindow::PreviewById( const IsoString& previewId ) const
{
   return View( API_ImageWindow_GetPreviewById( handle, previewId.c_str() ) );
}

// ----------------------------------------------------------------------------

View ImageWindow::SelectedPreview() const
{
   return View( API_ImageWindow_GetSelectedPreview( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectPreview( View& preview )
{
   API_ImageWindow_SelectPreview( handle, preview.handle );
}

// ----------------------------------------------------------------------------

View ImageWindow::CreatePreview( int x0, int y0, int x1, int y1, const IsoString& previewId )
{
   return View( API_ImageWindow_CreatePreview( handle, x0, y0, x1, y1, previewId.c_str() ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::ModifyPreview( const IsoString& previewId, int x0, int y0, int x1, int y1, const IsoString& newId )
{
   API_ImageWindow_ModifyPreview( handle, previewId.c_str(), x0, y0, x1, y1, newId.c_str() );
}

// ----------------------------------------------------------------------------

pcl::Rect ImageWindow::PreviewRect( const IsoString& previewId ) const
{
   pcl::Rect r( -1, -1, -1, -1 );
   API_ImageWindow_GetPreviewRect( handle, previewId.c_str(), &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void ImageWindow::DeletePreview( const IsoString& previewId )
{
   API_ImageWindow_DeletePreview( handle, previewId.c_str() );
}

// ----------------------------------------------------------------------------

void ImageWindow::DeletePreviews()
{
   API_ImageWindow_DeletePreviews( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetSampleFormat( int& bitsPerSample, bool& floatSample ) const
{
   uint32 bits;
   api_bool flt;
   API_ImageWindow_GetImageWindowSampleFormat( handle, &bits, &flt );
   bitsPerSample = int( bits );
   floatSample = flt != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetSampleFormat( int bitsPerSample, bool floatSample )
{
   API_ImageWindow_SetImageWindowSampleFormat( handle, bitsPerSample, api_bool( floatSample ) );
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::Mask() const
{
   return ImageWindow( API_ImageWindow_GetImageWindowMask( handle, 0 ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetMask( ImageWindow& mask, bool inverted )
{
   API_ImageWindow_SetImageWindowMask( handle, mask.handle, inverted );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskInverted() const
{
   api_bool inverted;
   (void)API_ImageWindow_GetImageWindowMask( handle, &inverted );
   return inverted != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskEnabled() const
{
   return API_ImageWindow_GetImageWindowMaskEnabled( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableMask( bool enable )
{
   API_ImageWindow_SetImageWindowMaskEnabled( handle, enable );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskVisible() const
{
   return API_ImageWindow_GetImageWindowMaskVisible( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::ShowMask( bool show )
{
   API_ImageWindow_SetImageWindowMaskVisible( handle, show );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskCompatible( const ImageWindow& mask )
{
   return API_ImageWindow_ValidateImageWindowMask( handle, mask.handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::HasMaskReferences() const
{
   return API_ImageWindow_GetMaskReferenceCount( handle ) > 0;
}

// ----------------------------------------------------------------------------

void ImageWindow::RemoveMaskReferences()
{
   API_ImageWindow_RemoveImageWindowMaskReferences( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateMaskReferences()
{
   API_ImageWindow_UpdateImageWindowMaskReferences( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetRGBWS( RGBColorSystem& rgbws ) const
{
   api_RGBWS a;
   API_ImageWindow_GetImageWindowRGBWS( handle, &a );
   rgbws = RGBColorSystem( a.gamma, a.isSRGBGamma != api_false, a.x, a.y, a.Y );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetRGBWS( const RGBColorSystem& rgbws )
{
   api_RGBWS a;
   a.gamma = rgbws.Gamma();
   a.isSRGBGamma = rgbws.IsSRGB();
   memcpy( a.x, *rgbws.ChromaticityXCoordinates(), sizeof( a.x ) );
   memcpy( a.y, *rgbws.ChromaticityYCoordinates(), sizeof( a.y ) );
   memcpy( a.Y, *rgbws.LuminanceCoefficients(), sizeof( a.Y ) );
   API_ImageWindow_SetImageWindowRGBWS( handle, &a );
}

// ----------------------------------------------------------------------------

bool ImageWindow::UsingGlobalRGBWS() const
{
   return API_ImageWindow_GetImageWindowGlobalRGBWS( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::UseGlobalRGBWS()
{
   API_ImageWindow_SetImageWindowGlobalRGBWS( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetGlobalRGBWS( RGBColorSystem& rgbws )
{
   api_RGBWS a;
   API_ImageWindow_GetGlobalRGBWS( &a );
   rgbws = RGBColorSystem( a.gamma, a.isSRGBGamma != api_false, a.x, a.y, a.Y );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetGlobalRGBWS( const RGBColorSystem& rgbws )
{
   api_RGBWS a;
   a.gamma = rgbws.Gamma();
   a.isSRGBGamma = rgbws.IsSRGB();
   memcpy( a.x, *rgbws.ChromaticityXCoordinates(), sizeof( a.x ) );
   memcpy( a.y, *rgbws.ChromaticityYCoordinates(), sizeof( a.y ) );
   memcpy( a.Y, *rgbws.LuminanceCoefficients(), sizeof( a.Y ) );
   API_ImageWindow_SetGlobalRGBWS( &a );
}

// ----------------------------------------------------------------------------

FITSKeywordArray ImageWindow::Keywords() const
{
   FITSKeywordArray keywords;
   int n = API_ImageWindow_GetImageWindowKeywordCount( handle );
   if ( n > 0 )
      for ( int i = 0; i < n; ++i )
      {
         IsoString name, value, comment;
         name.Reserve( 256 );
         value.Reserve( 256 );
         comment.Reserve( 256 );
         // N.B. The passed maximum lengths *do not* include an ending null character.
         API_ImageWindow_GetImageWindowKeyword( handle, i,
                        name.Begin(), 255, value.Begin(), 255, comment.Begin(), 255 );
         name.ResizeToNullTerminated();
         value.ResizeToNullTerminated();
         comment.ResizeToNullTerminated();
         keywords << FITSHeaderKeyword( name.c_str(), value.c_str(), comment.c_str() );
      }
   return keywords;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetKeywords( const FITSKeywordArray& keywords )
{
   API_ImageWindow_ResetImageWindowKeywords( handle );
   for ( const FITSHeaderKeyword& k : keywords )
      API_ImageWindow_AddImageWindowKeyword( handle, k.name.c_str(), k.value.c_str(), k.comment.c_str() );
}

// ----------------------------------------------------------------------------

void ImageWindow::ResetKeywords()
{
   API_ImageWindow_ResetImageWindowKeywords( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::HasAstrometricSolution() const
{
   return API_ImageWindow_GetImageWindowHasAstrometricSolution( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::RegenerateAstrometricSolution( bool allowGUIMessages, bool notify )
{
   return API_ImageWindow_RegenerateImageWindowAstrometricSolution( handle,
                                          api_bool( allowGUIMessages ), api_bool( notify ) ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::CopyAstrometricSolution( const ImageWindow& source, bool notify )
{
   return API_ImageWindow_CopyImageWindowAstrometricSolution( handle,
                                                         source.handle, api_bool( notify ) ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::ClearAstrometricSolution( bool notify )
{
   API_ImageWindow_ClearImageWindowAstrometricSolution( handle, api_bool( notify ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateAstrometryMetadata( bool notify )
{
   API_ImageWindow_UpdateImageWindowAstrometryMetadata( handle, api_bool( notify ) );
}

// ----------------------------------------------------------------------------

bool ImageWindow::ImageToCelestial( double& x, double& y, bool rawRA ) const
{
   return API_ImageWindow_ImageToCelestial( handle, &x, &y, api_bool( rawRA ) ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::CelestialToImage( double& ra, double& dec ) const
{
   return API_ImageWindow_CelestialToImage( handle, &ra, &dec ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::GetResolution( double& xRes, double& yRes, bool& metric ) const
{
   api_bool m;
   API_ImageWindow_GetImageWindowResolution( handle, &xRes, &yRes, &m );
   metric = m != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetResolution( double xRes, double yRes, bool metric )
{
   API_ImageWindow_SetImageWindowResolution( handle, xRes, yRes, metric );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetDefaultResolution( double& xRes, double& yRes, bool& metric )
{
   api_bool m;
   API_ImageWindow_GetDefaultResolution( &xRes, &yRes, &m );
   metric = m != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDefaultICCProfileEmbeddingEnabledForRGBImages()
{
   api_bool rgb = api_false;
   API_ImageWindow_GetDefaultICCProfileEmbedding( &rgb, 0 );
   return rgb != api_false;
}

bool ImageWindow::IsDefaultICCProfileEmbeddingEnabledForGrayscaleImages()
{
   api_bool gray = api_false;
   API_ImageWindow_GetDefaultICCProfileEmbedding( 0, &gray );
   return gray != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDefaultThumbnailEmbeddingEnabled()
{
   return API_ImageWindow_GetDefaultThumbnailEmbedding() != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDefaultPropertiesEmbeddingEnabled()
{
   return API_ImageWindow_GetDefaultPropertiesEmbedding() != api_false;
}

// ----------------------------------------------------------------------------

StringList ImageWindow::SwapDirectories()
{
   StringList directories;

   for ( int i = 0; ; ++i )
   {
      size_type len = 0;
      API_ImageWindow_GetSwapDirectory( i, 0, &len );
      if ( len == 0 )
         break;

      String path;
      path.SetLength( len );
      if ( API_ImageWindow_GetSwapDirectory( i, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetSwapDirectory" );
      path.ResizeToNullTerminated();

      directories.Add( path );
   }

   return directories;
}

// ----------------------------------------------------------------------------

bool ImageWindow::SetSwapDirectories( const StringList& directories )
{
   Array<const char16_type*> strings;
   for ( const String& dir : directories )
      strings.Add( dir.c_str() );
   return API_ImageWindow_SetSwapDirectories( strings.Begin(), int32( strings.Length() ) ) != api_false;
}

// ----------------------------------------------------------------------------

int ImageWindow::CursorTolerance()
{
   return API_ImageWindow_GetCursorTolerance();
}

// ----------------------------------------------------------------------------

ImageWindow::gui_mode ImageWindow::CurrentMode()
{
   return gui_mode( API_ImageWindow_GetImageWindowMode() );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectMode( ImageWindow::gui_mode mode )
{
   API_ImageWindow_SetImageWindowMode( mode );
}

// ----------------------------------------------------------------------------

ImageWindow::display_channel ImageWindow::CurrentChannel() const
{
   return display_channel( API_ImageWindow_GetImageWindowDisplayChannel( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectChannel( ImageWindow::display_channel chn )
{
   API_ImageWindow_SetImageWindowDisplayChannel( handle, chn );
}

// ----------------------------------------------------------------------------

ImageWindow::mask_mode ImageWindow::MaskMode() const
{
   return mask_mode( API_ImageWindow_GetImageWindowMaskMode( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetMaskMode( ImageWindow::mask_mode mode )
{
   API_ImageWindow_SetImageWindowMaskMode( handle, mode );
}

// ----------------------------------------------------------------------------

ImageWindow::background_brush ImageWindow::GetBackgroundBrush( uint32& fgColor, uint32& bgColor )
{
   return background_brush( API_ImageWindow_GetTransparencyBackgroundBrush( &fgColor, &bgColor ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetBackgroundBrush( ImageWindow::background_brush brush, uint32 fgColor, uint32 bgColor )
{
   API_ImageWindow_SetTransparencyBackgroundBrush( brush, fgColor, bgColor );
}

// ----------------------------------------------------------------------------

ImageWindow::transparency_mode ImageWindow::TransparencyMode() const
{
   return transparency_mode( API_ImageWindow_GetImageWindowTransparencyMode( handle, 0 ) );
}

// ----------------------------------------------------------------------------

RGBA ImageWindow::TransparencyColor() const
{
   uint32 color;
   (void)API_ImageWindow_GetImageWindowTransparencyMode( handle, &color );
   return color;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetTransparencyMode( ImageWindow::transparency_mode mode, RGBA color )
{
   API_ImageWindow_SetImageWindowTransparencyMode( handle, mode, color );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetViewport( double cx, double cy, int zoom )
{
   API_ImageWindow_SetImageWindowViewport( handle, cx, cy, zoom );
}

// ----------------------------------------------------------------------------

void ImageWindow::FitWindow()
{
   API_ImageWindow_FitImageWindow( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::ZoomToFit( bool optimalFit, bool allowMagnification, bool allowAnimations, bool noLimits )
{
   API_ImageWindow_ZoomImageWindowToFit( handle, optimalFit, allowMagnification, allowAnimations, noLimits );
}

// ----------------------------------------------------------------------------

int ImageWindow::ZoomFactor() const
{
   return API_ImageWindow_GetImageWindowZoomFactor( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetZoomFactor( int z )
{
   API_ImageWindow_SetImageWindowZoomFactor( handle, z );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetViewportSize( int& width, int& height ) const
{
   API_ImageWindow_GetImageWindowViewportSize( handle, &width, &height );
}

// ----------------------------------------------------------------------------

Point ImageWindow::ViewportPosition() const
{
   Point p;
   API_ImageWindow_GetImageWindowViewportPosition( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetViewportPosition( int x, int y )
{
   API_ImageWindow_SetImageWindowViewportPosition( handle, x, y );
}

// ----------------------------------------------------------------------------

Rect ImageWindow::VisibleViewportRect() const
{
   Rect r;
   API_ImageWindow_GetImageWindowVisibleViewportRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsVisible() const
{
   return API_ImageWindow_GetImageWindowVisible( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::Show( bool fitWindow )
{
   API_ImageWindow_SetImageWindowVisible( handle, api_true );
   if ( fitWindow )
      ZoomToFit();
}

// ----------------------------------------------------------------------------

void ImageWindow::Hide()
{
   API_ImageWindow_SetImageWindowVisible( handle, api_false );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsIconic() const
{
   return API_ImageWindow_GetImageWindowIconic( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::Iconize()
{
   API_ImageWindow_SetImageWindowIconic( handle, api_true );
}

// ----------------------------------------------------------------------------

void ImageWindow::Deiconize()
{
   API_ImageWindow_SetImageWindowIconic( handle, api_false );
}

// ----------------------------------------------------------------------------

void ImageWindow::BringToFront()
{
   API_ImageWindow_BringImageWindowToFront( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::SendToBack()
{
   API_ImageWindow_SendImageWindowToBack( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDynamicSessionActive()
{
   return API_ImageWindow_GetActiveDynamicInterface() != 0;
}

// ----------------------------------------------------------------------------

ProcessInterface* ImageWindow::ActiveDynamicInterface()
{
   return reinterpret_cast<ProcessInterface*>( API_ImageWindow_GetActiveDynamicInterface() );
}

// ----------------------------------------------------------------------------

bool ImageWindow::TerminateDynamicSession( bool closeInterface )
{
   return API_ImageWindow_TerminateDynamicSession( closeInterface ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetDynamicCursor( const char** xpm, int hx, int hy )
{
   API_ImageWindow_SetDynamicCursorXPM( handle, xpm, hx, hy );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetDynamicCursor( const Bitmap& bmp, int hx, int hy )
{
   API_ImageWindow_SetDynamicCursor( handle, bmp.handle, hx, hy );
}

// ----------------------------------------------------------------------------

Bitmap ImageWindow::DynamicCursorBitmap() const
{
   return Bitmap( API_ImageWindow_GetDynamicCursorBitmap( handle ) );
}

// ----------------------------------------------------------------------------

Point ImageWindow::DynamicCursorHotSpot() const
{
   Point p( 0 );
   API_ImageWindow_GetDynamicCursorHotSpot( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

double ImageWindow::DisplayPixelRatio() const
{
   double r;
   if ( API_ImageWindow_GetImageWindowDisplayPixelRatio( handle, &r ) == api_false )
      throw APIFunctionError( "GetImageWindowDisplayPixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( int& x, int& y ) const
{
   API_ImageWindow_ViewportToImage( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( double& x, double& y ) const
{
   API_ImageWindow_ViewportToImageD( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( Point* p, size_type n ) const
{
   API_ImageWindow_ViewportToImageArray( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( DPoint* p, size_type n ) const
{
   API_ImageWindow_ViewportToImageArrayD( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportScalarToImage( int* d, size_type n ) const
{
   API_ImageWindow_ViewportScalarToImageArray( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportScalarToImage( double* d, size_type n ) const
{
   API_ImageWindow_ViewportScalarToImageArrayD( handle, d, n );
}

// ----------------------------------------------------------------------------

double ImageWindow::ViewportScalarToImage( double d ) const
{
   API_ImageWindow_ViewportScalarToImageD( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( int& x, int& y ) const
{
   API_ImageWindow_ImageToViewport( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( double& x, double& y ) const
{
   API_ImageWindow_ImageToViewportD( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( Point* p, size_type n ) const
{
   API_ImageWindow_ImageToViewportArray( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( DPoint* p, size_type n ) const
{
   API_ImageWindow_ImageToViewportArrayD( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageScalarToViewport( int* d , size_type n ) const
{
   API_ImageWindow_ImageScalarToViewportArray( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageScalarToViewport( double* d , size_type n ) const
{
   API_ImageWindow_ImageScalarToViewportArrayD( handle, d, n );
}

// ----------------------------------------------------------------------------

int ImageWindow::ImageScalarToViewport( int d ) const
{
   API_ImageWindow_ImageScalarToViewport( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

double ImageWindow::ImageScalarToViewport( double d ) const
{
   API_ImageWindow_ImageScalarToViewportD( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToGlobal( int& x, int& y ) const
{
   API_ImageWindow_ViewportToGlobal( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::GlobalToViewport( int& x, int& y ) const
{
   API_ImageWindow_GlobalToViewport( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::Regenerate()
{
   API_ImageWindow_RegenerateImageWindowViewport( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::RegenerateViewportRect( int x0, int y0, int x1, int y1 )
{
   API_ImageWindow_RegenerateViewportRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageWindow::RegenerateImageRect( double x0, double y0, double x1, double y1 )
{
   API_ImageWindow_RegenerateImageRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateViewport()
{
   API_ImageWindow_UpdateImageWindowViewport( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateViewportRect( int x0, int y0, int x1, int y1 )
{
   API_ImageWindow_UpdateViewportRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateImageRect( double x0, double y0, double x1, double y1 )
{
   API_ImageWindow_UpdateImageRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

bool ImageWindow::HasPendingUpdates() const
{
   return API_ImageWindow_GetViewportUpdateRect( handle, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::CommitPendingUpdates()
{
   API_ImageWindow_CommitViewportUpdates( handle );
}

// ----------------------------------------------------------------------------

Rect ImageWindow::ViewportUpdateRect() const
{
   Rect r;
   API_ImageWindow_GetViewportUpdateRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

Bitmap ImageWindow::ViewportBitmap( int x0, int y0, int x1, int y1, uint32 flags ) const
{
   return Bitmap( API_ImageWindow_GetViewportBitmap( ModuleHandle(), handle, x0, y0, x1, y1, 0 ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::BeginSelection( int x, int y, uint32 flags )
{
   API_ImageWindow_BeginViewportSelection( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageWindow::ModifySelection( int x, int y, uint32 flags )
{
   API_ImageWindow_ModifyViewportSelection( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateSelection()
{
   API_ImageWindow_UpdateViewportSelection( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::CancelSelection()
{
   API_ImageWindow_CancelViewportSelection( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::EndSelection()
{
   API_ImageWindow_EndViewportSelection( handle );
}

// ----------------------------------------------------------------------------

pcl::Rect ImageWindow::SelectionRect( uint32* flags ) const
{
   pcl::Rect r;
   API_ImageWindow_GetViewportSelection( handle, &r.x0, &r.y0, &r.x1, &r.y1, flags );
   return r;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsSelection() const
{
   return API_ImageWindow_GetViewportSelection( handle, 0, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::WindowById( const IsoString& id )
{
   return ImageWindow( API_ImageWindow_GetImageWindowById( id.c_str() ) );
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::WindowByFilePath( const String& filePath )
{
   return ImageWindow( API_ImageWindow_GetImageWindowByFilePath( filePath.c_str() ) );
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::ActiveWindow()
{
   return ImageWindow( API_ImageWindow_GetActiveImageWindow() );
}

// ----------------------------------------------------------------------------

Array<ImageWindow> ImageWindow::AllWindows( bool includeIconicWindows )
{
   Array<ImageWindow> a;
   API_ImageWindow_EnumerateImageWindows(
               InternalWindowEnumerator::Callback, &a, includeIconicWindows );
   return a;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageWindow.cpp - Released 2025-04-07T08:53:32Z
