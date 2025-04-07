//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// FluxCalibrationInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FluxCalibrationInstance_h
#define __FluxCalibrationInstance_h

#include "FluxCalibrationParameters.h"

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FluxCalibrationInstance : public ProcessImplementation
{
public:

   FluxCalibrationInstance( const MetaProcess* );
   FluxCalibrationInstance( const FluxCalibrationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

private:

   struct CalibrationParameter
   {
      float    value;
      pcl_enum mode;
      String   keyword;
      String   stdKeyword;

      CalibrationParameter( float a_value, pcl_enum a_mode, const String& a_stdKeyword )
         : value( a_value )
         , mode( a_mode )
         , stdKeyword( a_stdKeyword )
      {
      }

      CalibrationParameter() = default;
      CalibrationParameter( const CalibrationParameter& ) = default;
      CalibrationParameter& operator =( const CalibrationParameter& ) = default;

      float operator =( float x )
      {
         return value = x;
      }

      float GetValue( const FITSKeywordArray& keywords ) const
      {
         if ( mode == pcl_enum( FCParameterMode::Literal ) )
            return value;

         for ( const FITSHeaderKeyword& k : keywords )
            if ( mode == pcl_enum( FCParameterMode::StandardKeyword ) && String( k.name ) == stdKeyword ||
                 mode == pcl_enum( FCParameterMode::CustomKeyword ) && String( k.name ) == keyword )
            {
               float retVal;
               if ( k.value.TryToFloat( retVal ) )
                  return retVal;
            }

         return -1;
      }
   };

   CalibrationParameter p_wavelength;            // nm
   CalibrationParameter p_transmissivity;        // (0,1]
   CalibrationParameter p_filterWidth;           // nm
   CalibrationParameter p_aperture;              // mm    *
   CalibrationParameter p_centralObstruction;    // mm    *
   CalibrationParameter p_exposureTime;          // s     *
   CalibrationParameter p_atmosphericExtinction; // [0,1]
   CalibrationParameter p_sensorGain;            //       *
   CalibrationParameter p_quantumEfficiency;     // [0,1)

   friend class FluxCalibrationEngine;
   friend class FluxCalibrationProcess;
   friend class FluxCalibrationInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __FluxCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF FluxCalibrationInstance.h - Released 2025-04-07T08:53:55Z
