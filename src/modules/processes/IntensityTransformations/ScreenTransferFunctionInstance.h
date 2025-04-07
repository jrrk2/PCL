//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ScreenTransferFunctionInstance_h
#define __ScreenTransferFunctionInstance_h

#include <pcl/Array.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/MetaParameter.h> // pcl_enum
#include <pcl/ProcessImplementation.h>

#include "ScreenTransferFunctionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

struct STF
{
   float m;      // midtones balance
   float c0, c1; // shadows and highlights clipping points
   float r0, r1; // shadows and highlights dynamic range expansion

   STF()
   {
      Reset();
   }

   STF( const STF& x ) : m( x.m ), c0( x.c0 ), c1( x.c1 ), r0( x.r0 ), r1( x.r1 )
   {
   }

   void Reset()
   {
      m = 0.5F;
      c0 = r0 = 0;
      c1 = r1 = 1;
   }

   bool operator ==( const STF& x ) const
   {
      return m == x.m && c0 == x.c0 && c1 == x.c1 && r0 == x.r0 && r1 == x.r1;
   }
};

// ----------------------------------------------------------------------------

class ScreenTransferFunctionInstance : public ProcessImplementation
{
public:

   typedef Array<STF>   stf_list;

   ScreenTransferFunctionInstance( const MetaProcess* );
   ScreenTransferFunctionInstance( const ScreenTransferFunctionInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

   void GetViewSTF( const View& );

   void ApplyTo( View& view ) const;

   const STF& operator []( int i ) const
   {
      return p_stf[i];
   }

   STF& operator []( int i )
   {
      return p_stf[i];
   }

   pcl_enum Interaction() const
   {
      return p_interaction;
   }

private:

   stf_list p_stf;

   /*
    * The following parameter is updated by ScreenTransferFunctionInterface and
    * is used to import STF instances by HistogramTransformationInterface.
    */
   pcl_enum p_interaction;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ScreenTransferFunctionInstance_h

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInstance.h - Released 2025-04-07T08:53:56Z
