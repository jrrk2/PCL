//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// RescaleInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RescaleInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RescaleInstance::RescaleInstance( const MetaProcess* m, int r )
   : ProcessImplementation( m )
   , p_mode( r )
{
}

// ----------------------------------------------------------------------------

RescaleInstance::RescaleInstance( const RescaleInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void RescaleInstance::Assign( const ProcessImplementation& p )
{
   const RescaleInstance* x = dynamic_cast<const RescaleInstance*>( &p );
   if ( x != nullptr )
      p_mode = x->p_mode;
}

// ----------------------------------------------------------------------------

UndoFlags RescaleInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool RescaleInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Rescale cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool RescaleInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   switch ( p_mode )
   {
   default:
   case RescalingMode::RGBK:
      image->SelectNominalChannels();
      image.Rescale();
      break;
   case RescalingMode::RGBK_Individual:
      for ( int c = 0; c < image->NumberOfNominalChannels(); ++c )
      {
         image->SelectChannel( c );
         image.Rescale();
      }
      break;
   case RescalingMode::CIEL:
      {
         ImageVariant L;
         image.GetLightness( L );
         L.Rescale();
         image.SetLightness( L );
      }
      break;
   case RescalingMode::CIEY:
      {
         ImageVariant Y;
         image.GetLuminance( Y );
         Y.Rescale();
         image.SetLuminance( Y );
      }
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* RescaleInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheRescalingModeParameter )
      return &p_mode;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RescaleInstance.cpp - Released 2025-04-07T08:53:56Z
