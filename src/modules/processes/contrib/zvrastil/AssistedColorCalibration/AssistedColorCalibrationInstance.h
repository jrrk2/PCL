//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AssistedColorCalibrationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AssistedColorCalibrationInstance_h
#define __AssistedColorCalibrationInstance_h

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class AssistedColorCalibrationInstance : public ProcessImplementation
{
public:

   AssistedColorCalibrationInstance( const MetaProcess* );
   AssistedColorCalibrationInstance( const AssistedColorCalibrationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void GetWBCorrectionFactors( double& red, double& green, double& blue ) const;
   const String& GetBackgroundReference() const;
   void GetHistogramTransformation( double& shadows, double& highlights, double& midtones ) const;
   double GetSaturationBoost() const;

   void CalculateBackgroundReference( double& red, double& green, double& blue ) const;
   void CorrectBackgroundReference( double& red, double& green, double& blue ) const;

private:

   float redCorrectionFactor;   // white balance RED correction factor
   float greenCorrectionFactor; // white balance GREEN correction factor
   float blueCorrectionFactor;  // white balance BLUE correction factor

   String backgroundReference; // view with background reference (view's full id)

   float histogramShadows;    // shadows clipping value of histogram transformation
   float histogramHighlights; // highlights clipping value of histogram transformation
   float histogramMidtones;   // midtones balance of histogram transformation

   float saturationBoost; // color saturation enhancement factor

   friend class AssistedColorCalibrationProcess;
   friend class AssistedColorCalibrationInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __AssistedColorCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationInstance.h - Released 2025-04-07T08:53:56Z
