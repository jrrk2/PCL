//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// ReadoutOptionsInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ReadoutOptionsInstance.h"

#include <pcl/ReadoutOptions.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadoutOptionsInstance::ReadoutOptionsInstance( const MetaProcess* p )
   : ProcessImplementation( p )
{
   SetOptions( ReadoutOptions() );
}

// ----------------------------------------------------------------------------

ReadoutOptionsInstance::ReadoutOptionsInstance( const ReadoutOptionsInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInstance::Assign( const ProcessImplementation& p )
{
   const ReadoutOptionsInstance* x = dynamic_cast<const ReadoutOptionsInstance*>( &p );
   if ( x != nullptr )
      SetOptions( x->Options() );
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "ReadoutOptions can only be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::ExecuteGlobal()
{
   ReadoutOptions::SetCurrentOptions( Options() );
   return true;
}

// ----------------------------------------------------------------------------

void* ReadoutOptionsInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheReadoutOptionsDataParameter )
      return &data;
   if ( p == TheReadoutOptionsModeParameter )
      return &mode;
   if ( p == TheReadoutOptionsProbeSizeParameter )
      return &probeSize;
   if ( p == TheReadoutOptionsPreviewSizeParameter )
      return &previewSize;
   if ( p == TheReadoutOptionsPreviewZoomFactorParameter )
      return &previewZoom;
   if ( p == TheReadoutRealPrecisionParameter )
      return &precision;
   if ( p == TheReadoutIntegerRangeParameter )
      return &range;
   if ( p == TheReadoutAlphaParameter )
      return &showAlpha;
   if ( p == TheReadoutMaskParameter )
      return &showMask;
   if ( p == TheReadoutPreviewParameter )
      return &showPreview;
   if ( p == TheReadoutPreviewCenterParameter )
      return &previewCenter;
   if ( p == TheReadoutShowEquatorialParameter )
      return &showEquatorial;
   if ( p == TheReadoutShowEclipticParameter )
      return &showEcliptic;
   if ( p == TheReadoutShowGalacticParameter )
      return &showGalactic;
   if ( p == TheReadoutCoordinateItemsParameter )
      return &coordinateItems;
   if ( p == TheReadoutCoordinatePrecisionParameter )
      return &coordinatePrecision;
   if ( p == TheReadoutBroadcastParameter )
      return &broadcast;
   if ( p == TheReadoutRealParameter )
      return &real;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool ReadoutOptionsInstance::ValidateParameter( void* value, const MetaParameter* p, size_type tableRow ) const
{
#define i32 *reinterpret_cast<int32*>( value )
#define u32 *reinterpret_cast<uint32*>( value )
#define u8  *reinterpret_cast<uint8*>( value )

   if ( p == TheReadoutOptionsDataParameter )
   {
      if ( i32 < 0 )
      {
         i32 = 0;
         return false;
      }

      if ( i32 >= ReadoutData::NumberOfModes )
      {
         i32 = ReadoutData::NumberOfModes-1;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsModeParameter )
   {
      if ( i32 < 0 )
      {
         i32 = 0;
         return false;
      }

      if ( i32 >= ReadoutMode::NumberOfModes )
      {
         i32 = ReadoutMode::NumberOfModes-1;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsProbeSizeParameter )
   {
      if ( u8 < 1 )
      {
         u8 = 1;
         return false;
      }

      if ( u8 > ReadoutOptions::MaxProbeSize )
      {
         u8 = ReadoutOptions::MaxProbeSize;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsPreviewSizeParameter )
   {
      if ( u8 < ReadoutOptions::MinPreviewSize )
      {
         u8 = ReadoutOptions::MinPreviewSize;
         return false;
      }

      if ( u8 > ReadoutOptions::MaxPreviewSize )
      {
         u8 = ReadoutOptions::MaxPreviewSize;
         return false;
      }
   }
   else if ( p == TheReadoutOptionsPreviewZoomFactorParameter )
   {
      if ( u8 < 1 )
      {
         u8 = 1;
         return false;
      }

      if ( u8 > ReadoutOptions::MaxPreviewZoomFactor )
      {
         u8 = ReadoutOptions::MaxPreviewZoomFactor;
         return false;
      }
   }
   else if ( p == TheReadoutRealPrecisionParameter )
   {
      if ( u8 > ReadoutOptions::MaxPrecision )
      {
         u8 = ReadoutOptions::MaxPrecision;
         return false;
      }
   }
   else if ( p == TheReadoutIntegerRangeParameter )
   {
      if ( u32 < 1 )
      {
         u32 = 1;
         return false;
      }
   }
   else if ( p == TheReadoutCoordinateItemsParameter )
   {
      if ( i32 < 1 )
      {
         i32 = 1;
         return false;
      }
      if ( i32 > 3 )
      {
         i32 = 3;
         return false;
      }
   }
   else if ( p == TheReadoutCoordinatePrecisionParameter )
   {
      if ( i32 < 0 )
      {
         i32 = 0;
         return false;
      }
      if ( i32 > 8 )
      {
         i32 = 8;
         return false;
      }
   }

   return true;

#undef i32
#undef u32
#undef u8
}

// ----------------------------------------------------------------------------

void ReadoutOptionsInstance::LoadCurrentOptions()
{
   SetOptions( ReadoutOptions::GetCurrentOptions() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsInstance.cpp - Released 2025-04-07T08:53:56Z
