//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ScreenTransferFunctionInstance.h"

#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ScreenTransferFunctionInstance::ScreenTransferFunctionInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_stf( 4 )
   , p_interaction( STFInteraction::Default )
{
}

// ----------------------------------------------------------------------------

ScreenTransferFunctionInstance::ScreenTransferFunctionInstance( const ScreenTransferFunctionInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInstance::Assign( const ProcessImplementation& p )
{
   const ScreenTransferFunctionInstance* x = dynamic_cast<const ScreenTransferFunctionInstance*>( &p );
   if ( x != nullptr )
   {
      p_stf.Assign( x->p_stf );
      p_interaction = x->p_interaction;
   }
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInstance::ExecuteOn( View& view )
{
   ApplyTo( view );
   return true;
}

// ----------------------------------------------------------------------------

void* ScreenTransferFunctionInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSTFInteractionParameter )
      return &p_interaction;
   if ( tableRow < 4 )
      if ( p == TheSTFShadowsClippingParameter )
         return &p_stf[tableRow].c0;
      else if ( p == TheSTFHighlightsClippingParameter )
         return &p_stf[tableRow].c1;
      else if ( p == TheSTFMidtonesBalanceParameter )
         return &p_stf[tableRow].m;
      else if ( p == TheSTFLowRangeParameter )
         return &p_stf[tableRow].r0;
      else if ( p == TheSTFHighRangeParameter )
         return &p_stf[tableRow].r1;
   return nullptr;
}

// ----------------------------------------------------------------------------

bool ScreenTransferFunctionInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSTFSetParameter )
   {
      p_stf.Clear();
      if ( length > 0 )
         p_stf.Add( STF(), length );
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

size_type ScreenTransferFunctionInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheSTFSetParameter )
      return 4;
   return 0;
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInstance::GetViewSTF( const View& view )
{
   if ( !view.IsNull() )
   {
      View::stf_list F;
      view.GetScreenTransferFunctions( F );
      for ( int c = 0; c < 4; ++c )
      {
         STF& f = p_stf[c];
         const HistogramTransformation& H = F[c];
         f.m = H.MidtonesBalance();
         f.c0 = H.ShadowsClipping();
         f.c1 = H.HighlightsClipping();
         f.r0 = H.LowRange();
         f.r1 = H.HighRange();
      }

      p_interaction = view.IsColor() ? STFInteraction::SeparateChannels : STFInteraction::Grayscale;
   }
}

// ----------------------------------------------------------------------------

void ScreenTransferFunctionInstance::ApplyTo( View& view ) const
{
   View::stf_list F;
   for ( int c = 0; c < 4; ++c )
   {
      const STF& f = p_stf[c];
      F.Add( HistogramTransformation( f.m, f.c0, f.c1, f.r0, f.r1 ) );
   }

   view.SetScreenTransferFunctions( F );
   if ( !view.AreScreenTransferFunctionsEnabled() )
      view.EnableScreenTransferFunctions();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInstance.cpp - Released 2025-04-07T08:53:56Z
