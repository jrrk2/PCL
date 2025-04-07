//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropInstance.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "CropInstance.h"
#include "CropParameters.h"
#include "GeometryModule.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CropInstance::CropInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_margins( 0.0 )
   , p_mode( CRMode::Default )
   , p_resolution( TheCRXResolutionParameter->DefaultValue(), TheCRYResolutionParameter->DefaultValue() )
   , p_metric( TheCRMetricResolutionParameter->DefaultValue() )
   , p_forceResolution( TheCRForceResolutionParameter->DefaultValue() )
   , p_fillColor( TheCRFillRedParameter->DefaultValue(),
                  TheCRFillGreenParameter->DefaultValue(),
                  TheCRFillBlueParameter->DefaultValue(),
                  TheCRFillAlphaParameter->DefaultValue() )
   , p_noGUIMessages( TheCRNoGUIMessagesParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

CropInstance::CropInstance( const CropInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void CropInstance::Assign( const ProcessImplementation& p )
{
   const CropInstance* x = dynamic_cast<const CropInstance*>( &p );
   if ( x != nullptr )
   {
      p_margins         = x->p_margins;
      p_mode            = x->p_mode;
      p_resolution      = x->p_resolution;
      p_metric          = x->p_metric;
      p_forceResolution = x->p_forceResolution;
      p_fillColor       = x->p_fillColor;
      p_noGUIMessages   = x->p_noGUIMessages;
   }
}

// ----------------------------------------------------------------------------

bool CropInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags CropInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData
        | UndoFlag::Keywords
        | UndoFlag::AstrometricSolution
        | (p_forceResolution ? UndoFlag::Resolution : 0);
}

// ----------------------------------------------------------------------------

bool CropInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "Crop cannot be executed on previews.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool CropInstance::BeforeExecution( View& view )
{
   if ( p_margins != 0.0 )
      return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id(), p_noGUIMessages );
   return true;
}

// ----------------------------------------------------------------------------

void CropInstance::GetNewSizes( int& w, int& h ) const
{
   Crop C( p_margins );
   C.SetMode( Crop::crop_mode( p_mode ) );
   C.SetResolution( p_resolution.x, p_resolution.y );
   C.SetMetricResolution( p_metric );
   C.GetNewSizes( w, h );
}

// ----------------------------------------------------------------------------

bool CropInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   Console console;

   ImageWindow window = view.Window();

   if ( p_margins != 0.0 )
   {
      ImageVariant image = view.Image();

      Crop C( p_margins );
      C.SetMode( static_cast<Crop::crop_mode>( p_mode ) );
      C.SetResolution( p_resolution.x, p_resolution.y );
      C.SetMetricResolution( p_metric );
      C.SetFillValues( p_fillColor );

      // Dimensions of target image
      int w0 = image.Width();
      int h0 = image.Height();

      // Dimensions of transformed image
      int width = w0, height = h0;
      C.GetNewSizes( width, height );

      if ( width < 1 || height < 1 )
         throw Error( "Crop: Invalid operation: Null target image dimensions" );

      // On 32-bit systems, make sure the resulting image requires less than 4 GiB.
      if ( sizeof( void* ) == sizeof( uint32 ) )
      {
         uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
         if ( sz > uint64( uint32_max-256 ) )
            throw Error( "Crop: Invalid operation: The resulting image would require more than 4 GiB" );
      }

      DeleteAstrometryMetadataAndPreviewsAndMask( window, false/*deleteCenterMetadata*/, false/*deleteScaleMetadata*/ );

      console.EnableAbort();

      StandardStatus status;
      image.SetStatusCallback( &status );

      C >> image;
   }
   else
   {
      console.WriteLn( "<end><cbr>&lt;* identity *&gt;" );
   }

   if ( p_forceResolution )
   {
      console.WriteLn( String().Format( "<end><cbr>Setting resolution: h:%.3f, v:%.3f, u:px/%s",
                                       p_resolution.x, p_resolution.y, p_metric ? "cm" : "inch" ) );
      window.SetResolution( p_resolution.x, p_resolution.y, p_metric );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* CropInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCRLeftMarginParameter )
      return &p_margins.x0;
   if ( p == TheCRTopMarginParameter )
      return &p_margins.y0;
   if ( p == TheCRRightMarginParameter )
      return &p_margins.x1;
   if ( p == TheCRBottomMarginParameter )
      return &p_margins.y1;
   if ( p == TheCRModeParameter )
      return &p_mode;
   if ( p == TheCRXResolutionParameter )
      return &p_resolution.x;
   if ( p == TheCRYResolutionParameter )
      return &p_resolution.y;
   if ( p == TheCRMetricResolutionParameter )
      return &p_metric;
   if ( p == TheCRForceResolutionParameter )
      return &p_forceResolution;
   if ( p == TheCRFillRedParameter )
      return p_fillColor.At( 0 );
   if ( p == TheCRFillGreenParameter )
      return p_fillColor.At( 1 );
   if ( p == TheCRFillBlueParameter )
      return p_fillColor.At( 2 );
   if ( p == TheCRFillAlphaParameter )
      return p_fillColor.At( 3 );
   if ( p == TheCRNoGUIMessagesParameter )
      return & p_noGUIMessages;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropInstance.cpp - Released 2025-04-07T08:53:55Z
