//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// CurvesTransformationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CurvesTransformationInstance_h
#define __CurvesTransformationInstance_h

#include <pcl/ProcessImplementation.h>

#include "CurveBase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Curve : public CurveBase
{
public:

   typedef CurveBase::input_list    input_list;
   typedef CurveBase::output_list   output_list;
   typedef CurveBase::interpolator  interpolator;

   Curve()
   {
      Initialize();
   }

   Curve( const Curve& ) = default;
   Curve& operator =( const Curve& ) = default;

   void Reverse() override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         y[i] = 1 - y[i];
   }

   bool IsIdentity() const override
   {
      for ( size_type i = 0; i < x.Length(); ++i )
         if ( x[i] != y[i] )
            return false;
      return true;
   }

private:

   void Initialize() override
   {
      x << 0.0 << 1.0;
      y << 0.0 << 1.0;
   }
};

// ----------------------------------------------------------------------------

class CurvesTransformationInstance : public ProcessImplementation
{
public:

   typedef Curve::interpolator   interpolator;

   CurvesTransformationInstance( const MetaProcess* );
   CurvesTransformationInstance( const CurvesTransformationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool Validate( String& info ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   const Curve& operator[]( int curveIdx ) const
   {
      return C[curveIdx];
   }

   Curve& operator[]( int curveIdx )
   {
      return C[curveIdx];
   }

   void Preview( UInt16Image& ) const;

private:

   Curve C[ CurveIndex::NumberOfCurves ];
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurvesTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF CurvesTransformationInstance.h - Released 2025-04-07T08:53:56Z
