//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageView.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/ErrorHandler.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageView.h>
#include <pcl/RGBColorSystem.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageView::ImageView( Control& parent, int width, int height, int numberOfChannels,
                      int bitsPerSample, bool floatSample, bool color )
   : ScrollBox( nullptr )
{
   TransferHandle( API_ImageView_CreateImageView( ModuleHandle(), this, parent.handle, 0/*flags*/,
                                                       width, height, numberOfChannels,
                                                       bitsPerSample, floatSample, color ) );
   if ( IsNull() )
      throw APIFunctionError( "CreateImageView" );

   m_viewport.TransferHandle( API_ImageView_CreateImageViewViewport( handle, &m_viewport ) );
   if ( m_viewport.IsNull() )
      throw APIFunctionError( "CreateImageViewViewport" );
}

ImageView::ImageView( void* h )
   : ScrollBox( nullptr )
{
   TransferHandle( h );
   if ( !IsNull() )
   {
      m_viewport.TransferHandle( API_ImageView_CreateImageViewViewport( handle, &m_viewport ) );
      if ( m_viewport.IsNull() )
         throw APIFunctionError( "CreateImageViewViewport" );
   }
}

ImageView::ImageView( void* h, void* hV )
   : ScrollBox( h, hV )
{
}

// ----------------------------------------------------------------------------

ImageVariant ImageView::Image() const
{
   image_handle hImg = API_ImageView_GetImageViewImage( handle );
   if ( hImg != 0 )
   {
      uint32 bitsPerSample;
      api_bool isFloat;
      if ( !API_SharedImage_GetImageFormat( hImg, &bitsPerSample, &isFloat ) )
         throw APIFunctionError( "GetImageFormat" );

      /*
       * Use a private ImageVariant constructor that forces image ownership by
       * ImageVariant. This constructor is selected with the second dummy int
       * argument (set to zero below).
       */
      if ( isFloat )
         switch ( bitsPerSample )
         {
         case 32 : return ImageVariant( new pcl::Image( hImg ), 0 );
         case 64 : return ImageVariant( new pcl::DImage( hImg ), 0 );
         }
      else
         switch ( bitsPerSample )
         {
         case  8 : return ImageVariant( new pcl::UInt8Image( hImg ), 0 );
         case 16 : return ImageVariant( new pcl::UInt16Image( hImg ), 0 );
         case 32 : return ImageVariant( new pcl::UInt32Image( hImg ), 0 );
         }
   }

   return ImageVariant();
}

// ----------------------------------------------------------------------------

bool ImageView::IsColorImage() const
{
   return API_ImageView_IsImageViewColorImage( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::GetImageGeometry( int& width, int& height, int& numberOfChannels ) const
{
   if ( API_ImageView_GetImageViewImageGeometry( handle, &width, &height, &numberOfChannels ) == api_false )
      throw APIFunctionError( "GetImageViewImageGeometry" );
}

// ----------------------------------------------------------------------------

void ImageView::GetSampleFormat( int& bitsPerSample, bool& floatSample ) const
{
   api_bool b = floatSample;
   if ( API_ImageView_GetImageViewSampleFormat( handle, &bitsPerSample, &b ) == api_false )
      throw APIFunctionError( "GetImageViewSampleFormat" );
   floatSample = b != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::SetSampleFormat( int bitsPerSample, bool floatSample )
{
   API_ImageView_SetImageViewSampleFormat( handle, bitsPerSample, floatSample );
}

// ----------------------------------------------------------------------------

void ImageView::GetRGBWS( RGBColorSystem& rgbws ) const
{
   api_RGBWS a;
   API_ImageView_GetImageViewRGBWS( handle, &a );
   rgbws = RGBColorSystem( a.gamma, a.isSRGBGamma != api_false, a.x, a.y, a.Y );
}

// ----------------------------------------------------------------------------

void ImageView::SetRGBWS( const RGBColorSystem& rgbws )
{
   api_RGBWS a;
   a.gamma = rgbws.Gamma();
   a.isSRGBGamma = rgbws.IsSRGB();
   memcpy( a.x, *rgbws.ChromaticityXCoordinates(), sizeof( a.x ) );
   memcpy( a.y, *rgbws.ChromaticityYCoordinates(), sizeof( a.y ) );
   memcpy( a.Y, *rgbws.LuminanceCoefficients(), sizeof( a.Y ) );
   API_ImageView_SetImageViewRGBWS( handle, &a );
}

// ----------------------------------------------------------------------------

bool ImageView::IsColorManagementEnabled() const
{
   api_bool enableCM = api_false;
   API_ImageView_GetImageViewCMEnabled( handle, &enableCM, 0, 0 );
   return enableCM != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::EnableColorManagement( bool enable )
{
   api_bool proofing, gamutCheck;
   API_ImageView_GetImageViewCMEnabled( handle, 0, &proofing, &gamutCheck );
   API_ImageView_SetImageViewCMEnabled( handle, enable, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageView::IsProofingEnabled() const
{
   api_bool proofing = api_false;
   API_ImageView_GetImageViewCMEnabled( handle, 0, &proofing, 0 );
   return proofing != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::EnableProofing( bool enable )
{
   api_bool enabled, gamutCheck;
   API_ImageView_GetImageViewCMEnabled( handle, &enabled, 0, &gamutCheck );
   API_ImageView_SetImageViewCMEnabled( handle, enabled, enable, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageView::IsGamutCheckEnabled() const
{
   api_bool gamutCheck = api_false;
   API_ImageView_GetImageViewCMEnabled( handle, 0, 0, &gamutCheck );
   return gamutCheck != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::EnableGamutCheck( bool enable )
{
   api_bool enabled, proofing;
   API_ImageView_GetImageViewCMEnabled( handle, &enabled, &proofing, 0 );
   API_ImageView_SetImageViewCMEnabled( handle, enabled, proofing || enable, enable );
}

// ----------------------------------------------------------------------------

void ImageView::SetColorManagementFeatures( bool cmEnabled, bool proofing, bool gamutCheck )
{
   API_ImageView_SetImageViewCMEnabled( handle, cmEnabled, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageView::GetICCProfile( ICCProfile& profile ) const
{
   profile.Clear();

   size_type profileSize = API_ImageView_GetImageViewICCProfileLength( handle );
   if ( profileSize == 0 )
      return false;

   try
   {
      ByteArray profileData( profileSize );
      API_ImageView_GetImageViewICCProfile( handle, profileData.Begin() );
      profile.Set( profileData );
      return true;
   }
   ERROR_HANDLER

   return false;
}

// ----------------------------------------------------------------------------

void ImageView::SetICCProfile( const ICCProfile& profile )
{
   if ( profile.IsProfile() )
      API_ImageView_SetImageViewICCProfile( handle, profile.ProfileData().Begin() );
}

// ----------------------------------------------------------------------------

void ImageView::SetICCProfile( const String& filePath )
{
   API_ImageView_LoadImageViewICCProfile( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

void ImageView::DeleteICCProfile()
{
   API_ImageView_DeleteImageViewICCProfile( handle );
}

// ----------------------------------------------------------------------------

ImageView::gui_mode ImageView::CurrentMode() const
{
   return gui_mode( API_ImageView_GetImageViewMode( handle ) );
}

// ----------------------------------------------------------------------------

void ImageView::SelectMode( gui_mode mode )
{
   API_ImageView_SetImageViewMode( handle, mode );
}

// ----------------------------------------------------------------------------

ImageView::display_channel ImageView::CurrentChannel() const
{
   return display_channel( API_ImageView_GetImageViewDisplayChannel( handle ) );
}

// ----------------------------------------------------------------------------

void ImageView::SelectChannel( display_channel channel )
{
   API_ImageView_SetImageViewDisplayChannel( handle, channel );
}

// ----------------------------------------------------------------------------

ImageView::transparency_mode ImageView::TransparencyMode() const
{
   return transparency_mode( API_ImageView_GetImageViewTransparencyMode( handle, 0 ) );
}

// ----------------------------------------------------------------------------

RGBA ImageView::TransparencyColor() const
{
   RGBA color;
   API_ImageView_GetImageViewTransparencyMode( handle, &color );
   return color;
}

// ----------------------------------------------------------------------------

void ImageView::SetTransparencyMode( transparency_mode mode, RGBA color )
{
   API_ImageView_SetImageViewTransparencyMode( handle, mode, color );
}

// ----------------------------------------------------------------------------

void ImageView::SetViewport( double cx, double cy, int zoom )
{
   API_ImageView_SetImageViewViewport( handle, cx, cy, zoom );
}

// ----------------------------------------------------------------------------

int ImageView::ZoomFactor() const
{
   return API_ImageView_GetImageViewZoomFactor( handle );
}

// ----------------------------------------------------------------------------

void ImageView::SetZoomFactor( int z )
{
   API_ImageView_SetImageViewZoomFactor( handle, z );
}

// ----------------------------------------------------------------------------

void ImageView::GetViewportSize( int& width, int& height ) const
{
   API_ImageView_GetImageViewViewportSize( handle, &width, &height );
}

// ----------------------------------------------------------------------------

Point ImageView::ViewportPosition() const
{
   Point p;
   API_ImageView_GetImageViewViewportPosition( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void ImageView::SetViewportPosition( int x, int y )
{
   API_ImageView_SetImageViewViewportPosition( handle, x, y );
}

// ----------------------------------------------------------------------------

Rect ImageView::VisibleViewportRect() const
{
   Rect r;
   API_ImageView_GetImageViewVisibleViewportRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( int& x, int& y ) const
{
   API_ImageView_ViewportToImage( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( double& x, double& y ) const
{
   API_ImageView_ViewportToImageD( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( Point* p, size_type n ) const
{
   API_ImageView_ViewportToImageArray( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( DPoint* p, size_type n ) const
{
   API_ImageView_ViewportToImageArrayD( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportScalarToImage( int* d, size_type n ) const
{
   API_ImageView_ViewportScalarToImageArray( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportScalarToImage( double* d, size_type n ) const
{
   API_ImageView_ViewportScalarToImageArrayD( handle, d, n );
}

// ----------------------------------------------------------------------------

double ImageView::ViewportScalarToImage( double d ) const
{
   API_ImageView_ViewportScalarToImageD( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( int& x, int& y ) const
{
   API_ImageView_ImageToViewport( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( double& x, double& y ) const
{
   API_ImageView_ImageToViewportD( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( Point* p, size_type n ) const
{
   API_ImageView_ImageToViewportArray( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( DPoint* p, size_type n ) const
{
   API_ImageView_ImageToViewportArrayD( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ImageScalarToViewport( int* d, size_type n ) const
{
   API_ImageView_ImageScalarToViewportArray( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageView::ImageScalarToViewport( double* d, size_type n ) const
{
   API_ImageView_ImageScalarToViewportArrayD( handle, d, n );
}

// ----------------------------------------------------------------------------

int ImageView::ImageScalarToViewport( int d ) const
{
   API_ImageView_ImageScalarToViewport( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

double ImageView::ImageScalarToViewport( double d ) const
{
   API_ImageView_ImageScalarToViewportD( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToGlobal( int& x, int& y ) const
{
   API_ImageView_ViewportToGlobal( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::GlobalToViewport( int& x, int& y ) const
{
   API_ImageView_GlobalToViewport( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::Regenerate()
{
   API_ImageView_RegenerateImageViewViewport( handle );
}

// ----------------------------------------------------------------------------

void ImageView::RegenerateViewportRect( int x0, int y0, int x1, int y1 )
{
   API_ImageView_RegenerateViewportRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageView::RegenerateImageRect( double x0, double y0, double x1, double y1 )
{
   API_ImageView_RegenerateImageRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateViewport()
{
   API_ImageView_UpdateImageViewViewport( handle );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateViewportRect( int x0, int y0, int x1, int y1 )
{
   API_ImageView_UpdateViewportRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateImageRect( double x0, double y0, double x1, double y1 )
{
   API_ImageView_UpdateImageRect( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

bool ImageView::HasPendingUpdates() const
{
   return API_ImageView_GetViewportUpdateRect( handle, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::CommitPendingUpdates()
{
   API_ImageView_CommitViewportUpdates( handle );
}

// ----------------------------------------------------------------------------

Rect ImageView::ViewportUpdateRect() const
{
   Rect r;
   API_ImageView_GetViewportUpdateRect( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

Bitmap ImageView::ViewportBitmap( int x0, int y0, int x1, int y1, uint32 flags ) const
{
   return Bitmap( API_ImageView_GetViewportBitmap( ModuleHandle(), handle, x0, y0, x1, y1, flags ) );
}

// ----------------------------------------------------------------------------

void ImageView::BeginSelection( int x, int y, uint32 flags )
{
   API_ImageView_BeginViewportSelection( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageView::ModifySelection( int x, int y, uint32 flags )
{
   API_ImageView_ModifyViewportSelection( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateSelection()
{
   API_ImageView_UpdateViewportSelection( handle );
}

// ----------------------------------------------------------------------------

void ImageView::CancelSelection()
{
   API_ImageView_CancelViewportSelection( handle );
}

// ----------------------------------------------------------------------------

void ImageView::EndSelection()
{
   API_ImageView_EndViewportSelection( handle );
}

// ----------------------------------------------------------------------------

Rect ImageView::SelectionRect( uint32* flags ) const
{
   Rect r;
   API_ImageView_GetViewportSelection( handle, &r.x0, &r.y0, &r.x1, &r.y1, flags );
   return r;
}

// ----------------------------------------------------------------------------

bool ImageView::IsSelection() const
{
   return API_ImageView_GetViewportSelection( handle, 0, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ImageView*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ImageViewEventDispatcher
{
public:

   static void ViewportScrolled( control_handle hSender, control_handle hReceiver, int32 dx, int32 dy )
   {
      if ( handlers->onScrollViewport != nullptr )
         (receiver->*handlers->onScrollViewport)( *sender, dx, dy );
   }
}; // ImageViewEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ImageView::OnScrollViewport( scroll_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( API_ImageView_SetImageViewScrollEventRoutine( handle, &receiver,
                  (f != nullptr) ? ImageViewEventDispatcher::ViewportScrolled : nullptr ) == api_false )
      throw APIFunctionError( "SetImageViewScrollEventRoutine" );
   m_handlers->onScrollViewport = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageView.cpp - Released 2025-04-07T08:53:32Z
