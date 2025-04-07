//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// InvertInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "InvertInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

InvertInstance::InvertInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

InvertInstance::InvertInstance( const InvertInstance& x )
   : ProcessImplementation( x )
{
}

// ----------------------------------------------------------------------------

void InvertInstance::Assign( const ProcessImplementation& )
{
   // Nothing to assign (Invert has no parameters)
}

// ----------------------------------------------------------------------------

UndoFlags InvertInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool InvertInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Invert cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool InvertInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   // Don't invert alpha channels
   image.SelectNominalChannels();

   image.Invert();

   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InvertInstance.cpp - Released 2025-04-07T08:53:56Z
