//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// CurveBase.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CurveBase_h
#define __CurveBase_h

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/UnidimensionalInterpolation.h>

#include "CurvesTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CurveBase
{
public:

   typedef SortedArray<double>   input_list;
   typedef Array<double>         output_list;

   typedef UnidimensionalInterpolation<double>  interpolator;

   CurveBase() = default;
   CurveBase( const CurveBase& ) = default;
   CurveBase& operator =( const CurveBase& ) = default;

   virtual ~CurveBase()
   {
   }

   size_type Length() const
   {
      return x.Length();
   }

   DPoint operator []( size_type i ) const
   {
      return DPoint( x[i], y[i] );
   }

   double X( size_type i ) const
   {
      return x[i];
   }

   double Y( size_type i ) const
   {
      return y[i];
   }

   double& Y( size_type i )
   {
      return y[i];
   }

   const double* XVector() const
   {
      return x.Begin();
   }

   const double* YVector() const
   {
      return y.Begin();
   }

   void Reset()
   {
      x.Clear();
      y.Clear();
      Initialize();
   }

   void Remove( size_type i )
   {
      if ( i > 0 && i < Length()-1 )
      {
         x.Remove( x.MutableAt( i ) );
         y.Remove( y.At( i ) );
      }
   }

   void Add( double px, double py )
   {
      input_list::const_iterator i = x.Add( px );
      y.Insert( y.At( i - x.Begin() ), py );
   }

   void Add( const DPoint& p )
   {
      Add( p.x, p.y );
   }

   size_type Search( double px ) const
   {
      return x.Search( px ) - x.Begin();
   }

   int Type() const
   {
      return type;
   }

   void SetType( int t )
   {
      type = t;
   }

   virtual void Reverse() = 0;

   virtual bool IsIdentity() const = 0;

   interpolator* InitInterpolator() const;

   static double Interpolate( interpolator* i, double x )
   {
      return Range( (*i)( x ), 0.0, 1.0 );
   }

protected:

   input_list  x;
   output_list y;
   int32       type = CurveType::Default;

   virtual void Initialize() = 0;

   friend class CurvesTransformationInstance;
   friend class ColorSaturationInstance;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurveBase_h

// ----------------------------------------------------------------------------
// EOF CurveBase.h - Released 2025-04-07T08:53:56Z
