//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationInstance.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AnnotationInstance.h"
#include "AnnotationParameters.h"
#include "AnnotationRenderer.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Graphics.h>
#include <pcl/StandardStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationInstance::AnnotationInstance( const MetaProcess* m )
   : ProcessImplementation( m )
   , annotationText( TheAnnotationTextParameter->DefaultValue() )
   , annotationFont( TheAnnotationFontParameter->DefaultValue() )
   , annotationFontSize( TheAnnotationFontSizeParameter->DefaultValue() )
   , annotationFontBold( TheAnnotationFontBoldParameter->DefaultValue() )
   , annotationFontItalic( TheAnnotationFontItalicParameter->DefaultValue() )
   , annotationFontUnderline( TheAnnotationFontUnderlineParameter->DefaultValue() )
   , annotationFontShadow( TheAnnotationFontShadowParameter->DefaultValue() )
   , annotationColor( TheAnnotationColorParameter->DefaultValue() )
   , annotationPositionX( TheAnnotationPositionXParameter->DefaultValue() )
   , annotationPositionY( TheAnnotationPositionYParameter->DefaultValue() )
   , annotationShowLeader( TheAnnotationShowLeaderParameter->DefaultValue() )
   , annotationLeaderPositionX( TheAnnotationLeaderPositionXParameter->DefaultValue() )
   , annotationLeaderPositionY( TheAnnotationLeaderPositionYParameter->DefaultValue() )
   , annotationOpacity( TheAnnotationOpacityParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

AnnotationInstance::AnnotationInstance( const AnnotationInstance& x )
   : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void AnnotationInstance::Assign( const ProcessImplementation& p )
{
   const AnnotationInstance* x = dynamic_cast<const AnnotationInstance*>( &p );
   if ( x != nullptr )
   {
      annotationText = x->annotationText;
      annotationFont = x->annotationFont;
      annotationFontSize = x->annotationFontSize;
      annotationFontBold = x->annotationFontBold;
      annotationFontItalic = x->annotationFontItalic;
      annotationFontUnderline = x->annotationFontUnderline;
      annotationFontShadow = x->annotationFontShadow;
      annotationColor = x->annotationColor;
      annotationPositionX = x->annotationPositionX;
      annotationPositionY = x->annotationPositionY;
      annotationShowLeader = x->annotationShowLeader;
      annotationLeaderPositionX = x->annotationLeaderPositionX;
      annotationLeaderPositionY = x->annotationLeaderPositionY;
      annotationOpacity = x->annotationOpacity;
   }
}

// ----------------------------------------------------------------------------

bool AnnotationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "Annotation cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AnnotationEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const AnnotationInstance& instance )
   {
      int relPosX = 0, relPosY = 0;
      Bitmap annotationBmp = AnnotationRenderer::CreateAnnotationBitmap( instance, relPosX, relPosY, false );
      // blend bitmap to the image
      image.Blend( annotationBmp, Point( instance.annotationPositionX - relPosX, instance.annotationPositionY - relPosY ) );
   }
};

// ----------------------------------------------------------------------------

bool AnnotationInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            AnnotationEngine::Apply( static_cast<pcl::Image&>( *image ), *this );
            break;
         case 64:
            AnnotationEngine::Apply( static_cast<pcl::DImage&>( *image ), *this );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case 8:
            AnnotationEngine::Apply( static_cast<pcl::UInt8Image&>( *image ), *this );
            break;
         case 16:
            AnnotationEngine::Apply( static_cast<pcl::UInt16Image&>( *image ), *this );
            break;
         case 32:
            AnnotationEngine::Apply( static_cast<pcl::UInt32Image&>( *image ), *this );
            break;
         }

   return true;
}

// ----------------------------------------------------------------------------

void* AnnotationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheAnnotationTextParameter )
      return annotationText.Begin();
   if ( p == TheAnnotationFontParameter )
      return annotationFont.Begin();
   if ( p == TheAnnotationFontSizeParameter )
      return &annotationFontSize;
   if ( p == TheAnnotationFontBoldParameter )
      return &annotationFontBold;
   if ( p == TheAnnotationFontItalicParameter )
      return &annotationFontItalic;
   if ( p == TheAnnotationFontUnderlineParameter )
      return &annotationFontUnderline;
   if ( p == TheAnnotationFontShadowParameter )
      return &annotationFontShadow;
   if ( p == TheAnnotationColorParameter )
      return &annotationColor;
   if ( p == TheAnnotationPositionXParameter )
      return &annotationPositionX;
   if ( p == TheAnnotationPositionYParameter )
      return &annotationPositionY;
   if ( p == TheAnnotationShowLeaderParameter )
      return &annotationShowLeader;
   if ( p == TheAnnotationLeaderPositionXParameter )
      return &annotationLeaderPositionX;
   if ( p == TheAnnotationLeaderPositionYParameter )
      return &annotationLeaderPositionY;
   if ( p == TheAnnotationOpacityParameter )
      return &annotationOpacity;

   return nullptr;
}

// ----------------------------------------------------------------------------

bool AnnotationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheAnnotationTextParameter )
   {
      annotationText.Clear();
      if ( sizeOrLength > 0 )
         annotationText.SetLength( sizeOrLength );
   }
   else if ( p == TheAnnotationFontParameter )
   {
      annotationFont.Clear();
      if ( sizeOrLength > 0 )
         annotationFont.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type AnnotationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheAnnotationTextParameter )
      return annotationText.Length();
   if ( p == TheAnnotationFontParameter )
      return annotationFont.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF AnnotationInstance.cpp - Released 2025-04-07T08:53:56Z
