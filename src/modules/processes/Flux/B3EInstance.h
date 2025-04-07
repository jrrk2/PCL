//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// B3EInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __B3EInstance_h
#define __B3EInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------

class B3EInstance : public ProcessImplementation
{
public:

   B3EInstance( const MetaProcess* );
   B3EInstance( const B3EInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool Validate( String& info ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( pcl::String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   // Process parameters

   struct InputViewParameters
   {
      InputViewParameters();
      InputViewParameters( const InputViewParameters& ) = default;

      InputViewParameters& operator =( const InputViewParameters& ) = default;

      String      id;
      double      center = 0;
      pcl_bool    subtractBackground;
      String      backgroundReferenceViewId;
      float       backgroundLow;
      float       backgroundHigh;
      pcl_bool    backgroundUseROI;
      Rect        backgroundROI = 0;
      pcl_bool    outputBackgroundReferenceMask;
   };

   InputViewParameters  p_inputView[ 2 ];

   /*
   String      p_inputViewId1;
   String      p_inputViewId2;
   double      p_inputCenter1;
   double      p_inputCenter2;
   */
   double      p_outputCenter = 0;
   pcl_enum    p_intensityUnits;
   pcl_bool    p_syntheticImage;
   pcl_bool    p_thermalMap;
   pcl_bool    p_outOfRangeMask;

   /*
   struct BackgroundParameters
   {
      // Background calibration 1
      pcl_bool    subtractBackground;
      String      backgroundReferenceViewId;
      float       backgroundLow;
      float       backgroundHigh;
      pcl_bool    backgroundUseROI;
      Rect        backgroundROI;
      pcl_bool    outputBackgroundReferenceMask;
   } p_background[2];
   */

   // Read-only properties

   String      o_syntheticImageViewId;
   String      o_thermalMapViewId;
   String      o_outOfRangeMaskViewId;

   static double EvaluateBackground( const InputViewParameters& imgParams );

   friend class B3EEngine;
   friend class B3EProcess;
   friend class B3EInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __B3EInstance_h

// ----------------------------------------------------------------------------
// EOF B3EInstance.h - Released 2025-04-07T08:53:55Z
