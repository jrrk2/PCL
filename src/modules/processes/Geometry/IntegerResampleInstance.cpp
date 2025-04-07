//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// IntegerResampleInstance.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "GeometryModule.h"
#include "IntegerResampleInstance.h"
#include "IntegerResampleParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleInstance::IntegerResampleInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , p_zoomFactor( int32( TheIRZoomFactorParameter->DefaultValue() ) )
   , p_downsampleMode( TheIRDownsamplingModeParameter->Default )
   , p_resolution( TheIRXResolutionParameter->DefaultValue(), TheIRYResolutionParameter->DefaultValue() )
   , p_metric( TheIRMetricResolutionParameter->DefaultValue() )
   , p_forceResolution( TheIRForceResolutionParameter->DefaultValue() )
   , p_gammaCorrection( TheIRGammaCorrectionParameter->DefaultValue() )
   , p_noGUIMessages( TheIRNoGUIMessagesParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

IntegerResampleInstance::IntegerResampleInstance( const IntegerResampleInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void IntegerResampleInstance::Assign( const ProcessImplementation& p )
{
   const IntegerResampleInstance* x = dynamic_cast<const IntegerResampleInstance*>( &p );
   if ( x != nullptr )
   {
      p_zoomFactor      = x->p_zoomFactor;
      p_downsampleMode  = x->p_downsampleMode;
      p_resolution      = x->p_resolution;
      p_metric          = x->p_metric;
      p_forceResolution = x->p_forceResolution;
      p_gammaCorrection = x->p_gammaCorrection;
      p_noGUIMessages   = x->p_noGUIMessages;
   }
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags IntegerResampleInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData
        | UndoFlag::Keywords
        | UndoFlag::AstrometricSolution
        | (p_forceResolution ? UndoFlag::Resolution : 0);
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "IntegerResample cannot be executed on previews.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::BeforeExecution( View& view )
{
   if ( p_zoomFactor > 1 || p_zoomFactor < -1 )
      return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id(), p_noGUIMessages );
   return true;
}

// ----------------------------------------------------------------------------

void IntegerResampleInstance::GetNewSizes( int& width, int& height ) const
{
   IntegerResample R( p_zoomFactor );
   R.GetNewSizes( width, height );
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   Console console;

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( !image.IsComplexSample() )
      if ( p_zoomFactor > 1 || p_zoomFactor < -1 )
      {
         console.EnableAbort();

         StandardStatus status;
         image.SetStatusCallback( &status );

         IntegerResample Z( p_zoomFactor, static_cast<IntegerResample::downsample_mode>( p_downsampleMode ) );

         // On 32-bit systems, make sure the resulting image requires less than 4 GB.
         if ( sizeof( void* ) == sizeof( uint32 ) )
         {
            // Dimensions of target image
            int w0 = image.Width();
            int h0 = image.Height();

            // Dimensions of transformed image
            int width = w0, height = h0;
            Z.GetNewSizes( width, height );
            width = Max( 1, width );
            height = Max( 1, height );

            uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
            if ( sz > uint64( uint32_max-256 ) )
               throw Error( "IntegerResample: Invalid operation: The resulting image would require more than 4 GiB" );
         }

         DeleteAstrometryMetadataAndPreviewsAndMask( window,
                                                     false,              /*deleteCenterMetadata*/
                                                     false,              /*deleteScaleMetadata*/
                                                     1/Z.ScalingFactor() /*pixelSizeScalingFactor*/ );

         if ( p_gammaCorrection )
         {
            RGBColorSystem rgbws;
            window.GetRGBWS( rgbws );
            Z.EnableGammaCorrection();
            Z.SetRGBWorkingSpace( rgbws );
         }

         Z >> image;
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

void* IntegerResampleInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheIRZoomFactorParameter )
      return &p_zoomFactor;
   if ( p == TheIRDownsamplingModeParameter )
      return &p_downsampleMode;
   if ( p == TheIRXResolutionParameter )
      return &p_resolution.x;
   if ( p == TheIRYResolutionParameter )
      return &p_resolution.y;
   if ( p == TheIRMetricResolutionParameter )
      return &p_metric;
   if ( p == TheIRForceResolutionParameter )
      return &p_forceResolution;
   if ( p == TheIRGammaCorrectionParameter )
      return &p_gammaCorrection;
   if ( p == TheIRNoGUIMessagesParameter )
      return &p_noGUIMessages;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleInstance.cpp - Released 2025-04-07T08:53:55Z
