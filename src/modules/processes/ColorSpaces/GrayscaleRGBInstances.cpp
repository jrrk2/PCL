//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// GrayscaleRGBInstances.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "GrayscaleRGBInstances.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ConvertToGrayscaleInstance::ConvertToGrayscaleInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

ConvertToGrayscaleInstance::ConvertToGrayscaleInstance( const ConvertToGrayscaleInstance& x )
   : ProcessImplementation( x )
{
}

// ----------------------------------------------------------------------------

UndoFlags ConvertToGrayscaleInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ConvertToGrayscale cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().ColorSpace() == ColorSpace::Gray )
   {
      whyNot = "ConvertToGrayscale cannot be executed on grayscale images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool ConvertToGrayscaleInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   image.SetColorSpace( ColorSpace::Gray );

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ConvertToRGBColorInstance::ConvertToRGBColorInstance( const MetaProcess* m )
   : ProcessImplementation( m )
{
}

// ----------------------------------------------------------------------------

ConvertToRGBColorInstance::ConvertToRGBColorInstance( const ConvertToRGBColorInstance& x )
   : ProcessImplementation( x )
{
}

// ----------------------------------------------------------------------------

UndoFlags ConvertToRGBColorInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ConvertToRGBColor cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().ColorSpace() == ColorSpace::RGB )
   {
      whyNot = "ConvertToRGBColor cannot be executed on color images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool ConvertToRGBColorInstance::ExecuteOn( View& view )
{
   ImageWindow window = view.Window();
   Array<ImageWindow> windows = ImageWindow::AllWindows();
   for ( size_type i = 0; i < windows.Length(); ++i )
      if ( windows[i].Mask() == window && !windows[i].MainView().IsColor() )
         windows[i].RemoveMask();

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   image.SetColorSpace( ColorSpace::RGB );

   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GrayscaleRGBInstances.cpp - Released 2025-04-07T08:53:55Z
