//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ColorSaturationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ColorSaturationInstance_h
#define __ColorSaturationInstance_h

#include <pcl/ProcessImplementation.h>

#include "CurveBase.h"
#include "ColorSaturationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HSCurve : public CurveBase
{
public:

   typedef CurveBase::input_list    input_list;
   typedef CurveBase::output_list   output_list;
   typedef CurveBase::interpolator  interpolator;

   HSCurve()
   {
      Initialize();
   }

   HSCurve( const HSCurve& ) = default;
   HSCurve& operator =( const HSCurve& ) = default;

   void Reverse() override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         y[i] = -y[i];
   }

   bool IsIdentity() const override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         if ( y[i] + 1 != 1 )
            return false;
      return true;
   }

   static double Interpolate( interpolator* i, double x )
   {
      return Range( (*i)( x ), -10.0, 10.0 );
   }

private:

   void Initialize() override
   {
      x << 0.0 << 1.0;
      y << 0.0 << 0.0;
   }
};

// ----------------------------------------------------------------------------

class ColorSaturationInstance : public ProcessImplementation
{
public:

   ColorSaturationInstance( const MetaProcess* );
   ColorSaturationInstance( const ColorSaturationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   const HSCurve& Curve() const
   {
      return C;
   }

   template <typename T> T ShiftHueValue( T f ) const
   {
      if ( f > hueShift )
         f -= hueShift;
      else
         f += 1 - hueShift;
      return f;
   }

   template <typename T> T UnshiftHueValue( T f ) const
   {
      if ( f > 1 - hueShift )
         f -= 1-hueShift;
      else
         f += hueShift;
      return f;
   }

   void Preview( UInt16Image& ) const;

private:

   HSCurve C;
   float   hueShift = 0;

   friend class ColorSaturationInterface;
   friend class ColorSaturationProcess;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorSaturationInstance_h

// ----------------------------------------------------------------------------
// EOF ColorSaturationInstance.h - Released 2025-04-07T08:53:56Z
