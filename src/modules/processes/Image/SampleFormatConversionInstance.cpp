//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// SampleFormatConversionInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SampleFormatConversionInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversionInstance::SampleFormatConversionInstance( const MetaProcess* P, int t )
   : ProcessImplementation( P )
   , sampleFormat( t )
{
}

// ----------------------------------------------------------------------------

SampleFormatConversionInstance::SampleFormatConversionInstance( const SampleFormatConversionInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// -------------------------------------------------------------------------

void SampleFormatConversionInstance::Assign( const ProcessImplementation& p )
{
   const SampleFormatConversionInstance* x = dynamic_cast<const SampleFormatConversionInstance*>( &p );
   if ( x != nullptr )
      sampleFormat = x->sampleFormat;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags SampleFormatConversionInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "SampleFormatConversion cannot be executed on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   ImageWindow window = view.Window();

   Console().EnableAbort();

   switch ( sampleFormat )
   {
   case SampleFormatConversion::To8Bit:
      window.SetSampleFormat( 8, false );
      break;
   case SampleFormatConversion::To16Bit:
      window.SetSampleFormat( 16, false );
      break;
   case SampleFormatConversion::To32Bit:
      window.SetSampleFormat( 32, false );
      break;
   case SampleFormatConversion::ToFloat:
      window.SetSampleFormat( 32, true );
      break;
   case SampleFormatConversion::ToDouble:
      window.SetSampleFormat( 64, true );
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* SampleFormatConversionInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheSampleFormatConversionParameter )
      return &sampleFormat;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionInstance.cpp - Released 2025-04-07T08:53:56Z
