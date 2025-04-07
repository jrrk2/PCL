//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AdaptiveStretchInstance_h
#define __AdaptiveStretchInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>

#include "AdaptiveStretchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Base class for the adaptive stretch curve.
 */
class StretchCurve : public FVector
{
public:

   StretchCurve() = default;
   StretchCurve( const StretchCurve& ) = default;

   StretchCurve( int n ) : FVector( n )
   {
   }

   /*
    * Returns a transformed value f in [0,1]
    */
   double operator ()( double f ) const
   {
      int n1 = Length() - 1;
      f *= n1;
      int a = TruncI( f );
      if ( a < n1 )
      {
         double fa = (*this)[a];
         return fa + (f - a)*((*this)[a+1] - fa);
      }
      return 1;
   }
};

// ----------------------------------------------------------------------------

class AdaptiveStretchInstance : public ProcessImplementation
{
public:

   AdaptiveStretchInstance( const MetaProcess* );
   AdaptiveStretchInstance( const AdaptiveStretchInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

   // Reimplemented to launch the auxiliary graph interface.
   ProcessInterface* SelectInterface() const override;

   StretchCurve Preview( UInt16Image&, const View& ) const;

private:

   double   p_noiseThreshold;
   double   p_protection;
   pcl_bool p_useProtection;
   int32    p_maxCurvePoints;
   pcl_bool p_useROI;
   Rect     p_roi = 0;

   friend class AdaptiveStretchCurve;
   friend class AdaptiveStretchEngine;
   friend class AdaptiveStretchProcess;
   friend class AdaptiveStretchInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveStretchInstance_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchInstance.h - Released 2025-04-07T08:53:56Z
