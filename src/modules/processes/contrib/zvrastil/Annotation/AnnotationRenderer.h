//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationRenderer.h - Released 2025-04-07T08:53:56Z
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

#ifndef __AnnotationRenderer_h
#define __AnnotationRenderer_h

#include "AnnotationInstance.h"

#include "pcl/Font.h"
#include "pcl/Graphics.h"

namespace pcl
{

// ----------------------------------------------------------------------------

// Auxiliary static class, providing annotation rendering for dynamic interface
// and final execution
class AnnotationRenderer
{
public:

   // Builds the font according to current instance parameters
   static Font GetAnnotationFont( const AnnotationInstance& instance, double zoomFactor = 1.0 );

   static Rect GetTextRect( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY );

   // computes the rectangle, which contains both text and leader
   // also computes relative positions of the text and leader endpoint in this rectangle
   static Rect GetBoundingRectangle( const AnnotationInstance& instance,
      int& relPosX, int& relPosY, int& relLeaderPosX, int& relLeaderPosY );

   // Renders the given annotation to a given graphics object.
   // Coordinates should be given relative to actual drawing context
   static void RenderAnnotation( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY, int leaderPosX, int leaderPosY );

   // Renders the given annotation to a given graphics object.
   // Coordinates should be given relative to actual drawing context
   static void RenderGrips( const AnnotationInstance& instance, Graphics& g,
      int posX, int posY, int leaderPosX, int leaderPosY );

   // Creates bitmap and renders annotation with leader into the bitmap
   // returns the bitmap and relative position of the text in the bitmap
   static Bitmap CreateAnnotationBitmap( const AnnotationInstance& instance,
      int& relPosX, int& relPosY,
      bool renderGrips );

private:

   // Helper method. Renders text and leader with current pen to given location + offset.
   static void RenderTextAndLeader( const AnnotationInstance& instance, Graphics& g,
      const Rect& textRect, int leaderPosX, int leaderPosY, int offsetX, int offsetY );
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __AnnotationRenderer_h

// ----------------------------------------------------------------------------
// EOF AnnotationRenderer.h - Released 2025-04-07T08:53:56Z
