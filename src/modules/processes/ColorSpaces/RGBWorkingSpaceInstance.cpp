//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInstance.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "RGBWorkingSpaceInstance.h"
#include "RGBWorkingSpaceParameters.h"
#include "RGBWorkingSpaceProcess.h"

#include <pcl/ImageWindow.h>
#include <pcl/RGBColorSystem.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RGBWorkingSpaceInstance::RGBWorkingSpaceInstance( const MetaProcess* P )
   : ProcessImplementation( P )
   , Y( RGBColorSystem::sRGB.LuminanceCoefficients() )
   , x( RGBColorSystem::sRGB.ChromaticityXCoordinates() )
   , y( RGBColorSystem::sRGB.ChromaticityYCoordinates() )
   , gamma( RGBColorSystem::sRGB.Gamma() )
   , sRGB( true )
   , applyGlobalRGBWS( false )
{
}

// ----------------------------------------------------------------------------

RGBWorkingSpaceInstance::RGBWorkingSpaceInstance( const RGBWorkingSpaceInstance& p )
   : ProcessImplementation( p )
{
   Assign( p );
}

// ----------------------------------------------------------------------------

RGBWorkingSpaceInstance::RGBWorkingSpaceInstance( const MetaProcess* P, const RGBColorSystem& rgbws )
   : ProcessImplementation( P )
   , Y( rgbws.LuminanceCoefficients() )
   , x( rgbws.ChromaticityXCoordinates() )
   , y( rgbws.ChromaticityYCoordinates() )
   , gamma( rgbws.Gamma() )
   , sRGB( rgbws.IsSRGB() )
   , applyGlobalRGBWS( false )
{
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::Validate( String& info )
{
   try
   {
      RGBWorkingSpaceInstance t( *this );
      t.NormalizeLuminanceCoefficients();

      RGBColorSystem rgbws( t.gamma, t.sRGB != 0, t.x, t.y, t.Y );

      info.Clear();
      return true;
   }
   catch ( Exception& e )
   {
      info = e.Message();
      return false;
   }
}

// ----------------------------------------------------------------------------

void RGBWorkingSpaceInstance::Assign( const ProcessImplementation& p )
{
   const RGBWorkingSpaceInstance* t = dynamic_cast<const RGBWorkingSpaceInstance*>( &p );
   if ( t != nullptr )
   {
      Y = t->Y;
      x = t->x;
      y = t->y;
      gamma = t->gamma;
      sRGB = t->sRGB;
      applyGlobalRGBWS = t->applyGlobalRGBWS;
   }
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::IsMaskable( const View&, const ImageWindow& /*mask*/ ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags RGBWorkingSpaceInstance::UndoMode( const View& ) const
{
   return UndoFlag::RGBWS;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "RGBWorkingSpace cannot be executed on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::ExecuteOn( View& view )
{
   RGBWorkingSpaceInstance t( *this );
   t.NormalizeLuminanceCoefficients();

   RGBColorSystem rgbws( t.gamma, t.sRGB != 0, t.x, t.y, t.Y );

   view.Window().SetRGBWS( rgbws );
   return true;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::ExecuteGlobal()
{
   RGBWorkingSpaceInstance t( *this );
   t.NormalizeLuminanceCoefficients();

   RGBColorSystem rgbws( t.gamma, t.sRGB != 0, t.x, t.y, t.Y );

   ImageWindow::SetGlobalRGBWS( rgbws );
   return true;
}

// ----------------------------------------------------------------------------

void* RGBWorkingSpaceInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheRGBWSLuminanceCoefficientParameter )
      return Y.Begin() + tableRow;
   if ( p == TheRGBWSChromaticityXParameter )
      return x.Begin() + tableRow;
   if ( p == TheRGBWSChromaticityYParameter )
      return y.Begin() + tableRow;
   if ( p == TheRGBWSGammaParameter )
      return &gamma;
   if ( p == TheRGBWSsRGBGParameter )
      return &sRGB;
   if ( p == TheRGBWSApplyGlobalParameter )
      return &applyGlobalRGBWS;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool RGBWorkingSpaceInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheRGBWSChannelTableParameter )
      return true;

   return false;
}

// ----------------------------------------------------------------------------

size_type RGBWorkingSpaceInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheRGBWSChannelTableParameter )
      return 3;

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInstance.cpp - Released 2025-04-07T08:53:55Z
