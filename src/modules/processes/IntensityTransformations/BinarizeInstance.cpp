//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "BinarizeInstance.h"
#include "BinarizeParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Histogram.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeInstance::BinarizeInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , p_global( TheBinarizeIsGlobalParameter->DefaultValue() )
{
   p_level[0] = TheBinarizeLevelRParameter->DefaultValue();
   p_level[1] = TheBinarizeLevelGParameter->DefaultValue();
   p_level[2] = TheBinarizeLevelBParameter->DefaultValue();
}

// ----------------------------------------------------------------------------

BinarizeInstance::BinarizeInstance( const BinarizeInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void BinarizeInstance::Assign( const ProcessImplementation& p )
{
   const BinarizeInstance* x = dynamic_cast<const BinarizeInstance*>( &p );
   if ( x != nullptr )
   {
      for ( int i = 0; i < 3; ++i )
         p_level[i] = x->p_level[i];
      p_global = x->p_global;
   }
}

// ----------------------------------------------------------------------------

UndoFlags BinarizeInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool BinarizeInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Binarize cannot be executed on complex images.";
      return false;
   }

   return true;
}

//----------------------------------------------------------------------------

class BinarizeEngine
{
public:

   static void Apply( ImageVariant& image, const BinarizeInstance& instance )
   {
      for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
      {
         image.SelectChannel( c );
         if ( instance.p_global )
            image.Binarize( instance.p_level[0] );
         else
            image.Binarize( instance.p_level[c] );
      }
   }
};

// ----------------------------------------------------------------------------

void BinarizeInstance::Preview( UInt16Image& image ) const
{
   try
   {
      ImageVariant var( &image );
      BinarizeEngine::Apply( var, *this );
   }
   catch ( ... )
   {
   }
}

// ----------------------------------------------------------------------------

bool BinarizeInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   BinarizeEngine::Apply( image, *this );

   return true;
}

// ----------------------------------------------------------------------------

void* BinarizeInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheBinarizeLevelRParameter )
      return p_level+0;
   if ( p == TheBinarizeLevelGParameter )
      return p_level+1;
   if ( p == TheBinarizeLevelBParameter )
      return p_level+2;
   if ( p == TheBinarizeIsGlobalParameter )
      return &p_global;
  return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeInstance.cpp - Released 2025-04-07T08:53:56Z
