//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationInstance.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FastRotationInstance.h"
#include "GeometryModule.h"

#include <pcl/AutoViewLock.h>
#include <pcl/FastRotation.h>
#include <pcl/ImageWindow.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FastRotationInstance::FastRotationInstance( const MetaProcess* m, int r )
   : ProcessImplementation( m )
   , p_mode( r )
   , p_noGUIMessages( TheFRNoGUIMessagesParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

FastRotationInstance::FastRotationInstance( const FastRotationInstance& x )
   : ProcessImplementation( x )
   , p_mode( x.p_mode )
   , p_noGUIMessages( x.p_noGUIMessages )
{
}

// ----------------------------------------------------------------------------

void FastRotationInstance::Assign( const ProcessImplementation& p )
{
   const FastRotationInstance* x = dynamic_cast<const FastRotationInstance*>( &p );
   if ( x != nullptr )
   {
      p_mode = x->p_mode;
      p_noGUIMessages = x->p_noGUIMessages;
   }
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags FastRotationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData
        | UndoFlag::Keywords
        | UndoFlag::AstrometricSolution;
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "FastRotation cannot be executed on previews.";
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::BeforeExecution( View& view )
{
   return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id(), p_noGUIMessages );
}

// ----------------------------------------------------------------------------

bool FastRotationInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( (p_mode == FRMode::Rotate90CW || p_mode == FRMode::Rotate90CCW) && image.Width() != image.Height() )
      DeleteAstrometryMetadataAndPreviewsAndMask( window, false/*deleteCenterMetadata*/, false/*deleteScaleMetadata*/ );
   else
      DeleteAstrometryMetadataAndPreviews( window, false/*deleteCenterMetadata*/, false/*deleteScaleMetadata*/ );

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   switch ( p_mode )
   {
   default:
   case FRMode::Rotate180:
      Rotate180() >> image;
      break;
   case FRMode::Rotate90CW:
      Rotate90CW() >> image;
      break;
   case FRMode::Rotate90CCW:
      Rotate90CCW() >> image;
      break;
   case FRMode::HorizontalMirror:
      HorizontalMirror() >> image;
      break;
   case FRMode::VerticalMirror:
      VerticalMirror() >> image;
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* FastRotationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFRModeParameter )
      return &p_mode;
   if ( p == TheFRNoGUIMessagesParameter )
      return &p_noGUIMessages;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FastRotationInstance.cpp - Released 2025-04-07T08:53:55Z
