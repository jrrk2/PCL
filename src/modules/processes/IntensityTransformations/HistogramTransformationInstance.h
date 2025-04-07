//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// HistogramTransformationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __HistogramTransformationInstance_h
#define __HistogramTransformationInstance_h

#include <pcl/ProcessImplementation.h>

#include "HistogramTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HistogramTransformationInstance : public ProcessImplementation
{
public:

   HistogramTransformationInstance( const MetaProcess* );
   HistogramTransformationInstance( const HistogramTransformationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteOn( const ImageVariant& image, String& whyNot ) const override;
   bool ExecuteOn( ImageVariant& image, const IsoString& hints ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   double MidtonesBalance( int i ) const
   {
      return m[i];
   }

   void SetMidtonesBalance( int i, double b )
   {
      m[i] = pcl::Range( b, 0.0, 1.0 );
   }

   double ShadowsClipping( int i ) const
   {
      return c0[i];
   }

   double HighlightsClipping( int i ) const
   {
      return c1[i];
   }

   void SetShadowsClipping( int i, double c )
   {
      c0[i] = pcl::Range( c, 0.0, 1.0 );
      if ( c1[i] < c0[i] )
         pcl::Swap( c0[i], c1[i] );
   }

   void SetHighlightsClipping( int i, double c )
   {
      c1[i] = pcl::Range( c, 0.0, 1.0 );
      if ( c1[i] < c0[i] )
         pcl::Swap( c0[i], c1[i] );
   }

   double LowRange( int i ) const
   {
      return r0[i];
   }

   double HighRange( int i ) const
   {
      return r1[i];
   }

   void SetLowRange( int i, double r )
   {
      r0[i] = Min( 0.0, r );
   }

   void SetHighRange( int i, double r )
   {
      r1[i] = Max( 1.0, r );
   }

   bool IsIdentity( int i ) const
   {
      return m[i] == 0.5 && c0[i] == 0 && c1[i] == 1 && r0[i] == 0 && r1[i] == 1;
   }

   void Reset()
   {
      for ( int i = 0; i < 5; ++i )
      {
         m[i] = 0.5;
         c0[i] = r0[i] = 0;
         c1[i] = r1[i] = 1;
      }
   }

   void Preview( UInt16Image& ) const;

private:

   // Channel indexes: 0=R, 1=G, 2=B, 3=RGB/K, 4=Alpha
   double m[ 5 ];           // midtones balance
   double c0[ 5 ], c1[ 5 ]; // shadows and highlights clipping points
   double r0[ 5 ], r1[ 5 ]; // shadows and highlights dynamic range expansion
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF HistogramTransformationInstance.h - Released 2025-04-07T08:53:56Z
