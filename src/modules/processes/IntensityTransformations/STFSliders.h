//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// STFSliders.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __STFSliders_h
#define __STFSliders_h

#include <pcl/Control.h>

#include "ScreenTransferFunctionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Bitmap;
class Graphics;

class STFSliders : public Control
{
public:

   STFSliders();
   virtual ~STFSliders();

   void SetChannel( int c, bool rgb );

   bool IsRGB() const
   {
      return rgb;
   }

   void SetValues( const STF& );

   void SetVisibleRange( double a, double b );

   void GetVisibleRange( double& a, double& b ) const
   {
      a = v0; b = v1;
   }

   // Event handlers

   typedef void (Control::*value_event_handler)( STFSliders& sender,
                                                 int channel, int item, double value,
                                                 unsigned modifiers, bool final );

   typedef void (Control::*range_event_handler)( STFSliders& sender,
                                                 int channel, double v0, double v1,
                                                 unsigned modifiers );

   void OnValueUpdated( value_event_handler, Control& );
   void OnRangeUpdated( range_event_handler, Control& );

private:

   int      channel      = 0;
   bool     rgb          = true;
   double   m            = 0.5;
   double   c0           = 0;
   double   c1           = 1;
   double   v0           = 0;
   double   v1           = 1;
   Bitmap   gradient     = Bitmap::Null();
   int      beingDragged = -1;
   bool     scrolling    = false;
   int      scrollOrigin = 0;
   int      m_wheelSteps = 0; // accumulated 1/8-degree wheel steps

   value_event_handler onValueUpdated         = nullptr;
   Control*            onValueUpdatedReceiver = nullptr;

   range_event_handler onRangeUpdated         = nullptr;
   Control*            onRangeUpdatedReceiver = nullptr;

   double SliderToSTF( int x ) const
   {
      return v0 + x*(v1 - v0)/(Width() - 1);
   }

   int STFToSlider( double v ) const
   {
      return RoundInt( (Width() - 1)*(v - v0)/(v1 - v0) );
   }

   FPoint ZoomInRange( int x ) const
   {
      double dv = v1 - v0;
      double a = Max( 0.0, SliderToSTF( x ) - dv/4 );
      double b = Min( a + dv/2, 1.0 );
      return FPoint( a, b );
   }

   FPoint ZoomOutRange( int x ) const
   {
      double dv = v1 - v0;
      double a = Max( 0.0, SliderToSTF( x ) - dv );
      double b = Min( a + 2*dv, 1.0 );
      return FPoint( a, b );
   }

   void InvalidateGradient();
   void RebuildGradient();
   RGBA HandlerColor( double ) const;
   void DrawHandler( Graphics&, double );
   int FindHandler( double );
   double UpdateDragging( int );
   bool UpdateScrolling( int );

   void __Paint( Control& sender, const pcl::Rect& updateRect );
   void __Resize( Control& sender, int newWidth, int newHeight, int oldWidth, int oldHeight );
   void __MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void __MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __MouseWheel( Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __STFSliders_h

// ----------------------------------------------------------------------------
// EOF STFSliders.h - Released 2025-04-07T08:53:56Z
