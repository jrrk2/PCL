//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/DisplayFunction.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/DisplayFunction.h>

namespace pcl
{

// ----------------------------------------------------------------------------

template <class P>
static void DoApply( GenericImage<P>& image, const DisplayFunction& F )
{
   int i = image.FirstSelectedChannel();
   if ( i < image.NumberOfNominalChannels() )
   {
      int j = Max( image.LastSelectedChannel(), image.NumberOfNominalChannels()-1 );

      image.Status().Initialize( "Display function transformation", (1+j-i)*image.NumberOfSelectedPixels() );
      image.Status().DisableInitialization();

      try
      {
         image.PushSelections();
         for ( ; i <= j; ++i )
         {
            image.SelectChannel( i );
            F[i] >> image;
         }
         image.PopSelections();
         image.Status().EnableInitialization();
      }
      catch( ... )
      {
         image.Status().EnableInitialization();
         throw;
      }
   }
}

void DisplayFunction::Apply( pcl::Image& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::DImage& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::UInt8Image& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::UInt16Image& image ) const
{
   DoApply( image, *this );
}

void DisplayFunction::Apply( pcl::UInt32Image& image ) const
{
   DoApply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/DisplayFunction.cpp - Released 2025-04-07T08:53:32Z
