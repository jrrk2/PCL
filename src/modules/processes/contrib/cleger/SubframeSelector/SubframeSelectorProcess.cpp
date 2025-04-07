//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SubframeSelectorInstance.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorParameters.h"
#include "SubframeSelectorProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorProcess* TheSubframeSelectorProcess = nullptr;

// ----------------------------------------------------------------------------

SubframeSelectorProcess::SubframeSelectorProcess()
{
   TheSubframeSelectorProcess = this;

   new SSRoutine( this );
   new SSNonInteractive( this );

   new SSSubframes( this );
   new SSSubframeEnabled( TheSSSubframesParameter );
   new SSSubframePath( TheSSSubframesParameter );
   new SSLocalNormalizationDataPath( TheSSSubframesParameter );
   new SSDrizzlePath( TheSSSubframesParameter );

   new SSFileCache( this );

   new SSSubframeScale( this );
   new SSCameraGain( this );
   new SSCameraResolution( this );
   new SSSiteLocalMidnight( this );
   new SSScaleUnit( this );
   new SSDataUnit( this );
   new SSTrimmingFactor( this );

   new SSStructureLayers( this );
   new SSNoiseLayers( this );
   new SSHotPixelFilterRadius( this );
   new SSNoiseReductionFilterRadius( this );
   new SSMinStructureSize( this );
   new SSSensitivity( this );
   new SSPeakResponse( this );
   new SSBrightThreshold( this );
   new SSMaxDistortion( this );
   new SSAllowClusteredSources( this );
   new SSUpperLimit( this );
   new SSPSFFit( this );
   new SSPSFFitCircular( this );
   new SSMaxPSFFits( this );
   new SSROIX0( this );
   new SSROIY0( this );
   new SSROIX1( this );
   new SSROIY1( this );

   new SSNoNoiseAndSignalWarnings( this );

   new SSPedestalMode( this );
   new SSPedestal( this );
   new SSPedestalKeyword( this );

   new SSInputHints( this );
   new SSOutputHints( this );

   new SSOutputDirectory( this );
   new SSOutputExtension( this );
   new SSOutputPrefix( this );
   new SSOutputPostfix( this );
   new SSOutputKeyword( this );
   new SSGenerateHistoryProperties( this );
   new SSOverwriteExistingFiles( this );
   new SSOnError( this );

   new SSApprovalExpression( this );
   new SSWeightingExpression( this );

   new SSSortingProperty( this );
   new SSGraphProperty( this );
   new SSAuxGraphProperty( this );

   new SSUseFileThreads( this );
   new SSFileThreadOverload( this );
   new SSMaxFileReadThreads( this );
   new SSMaxFileWriteThreads( this );

   /*
    * ### N.B. Please do not change the order of instantiation of the following
    * table row parameters, as doing so will break compatibility with existing
    * scripts that depend on the SubframeSelector process.
    */

   new SSMeasurements( this );
   new SSMeasurementIndex( TheSSMeasurementsParameter );
   new SSMeasurementEnabled( TheSSMeasurementsParameter );
   new SSMeasurementLocked( TheSSMeasurementsParameter );
   new SSMeasurementPath( TheSSMeasurementsParameter );
   new SSMeasurementWeight( TheSSMeasurementsParameter );
   new SSMeasurementFWHM( TheSSMeasurementsParameter );
   new SSMeasurementEccentricity( TheSSMeasurementsParameter );
   new SSMeasurementPSFSignalWeight( TheSSMeasurementsParameter );
   new SSMeasurementUnused01( TheSSMeasurementsParameter ); // ### retained for compatibility
   new SSMeasurementSNRWeight( TheSSMeasurementsParameter );
   new SSMeasurementMedian( TheSSMeasurementsParameter );
   new SSMeasurementMedianMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementNoise( TheSSMeasurementsParameter );
   new SSMeasurementNoiseRatio( TheSSMeasurementsParameter );
   new SSMeasurementStars( TheSSMeasurementsParameter );
   new SSMeasurementStarResidual( TheSSMeasurementsParameter );
   new SSMeasurementFWHMMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementEccentricityMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementStarResidualMeanDev( TheSSMeasurementsParameter );
   new SSMeasurementAzimuth( TheSSMeasurementsParameter );
   new SSMeasurementAltitude( TheSSMeasurementsParameter );
   new SSMeasurementPSFFlux( TheSSMeasurementsParameter );
   new SSMeasurementPSFFluxPower( TheSSMeasurementsParameter );
   new SSMeasurementPSFTotalMeanFlux( TheSSMeasurementsParameter );
   new SSMeasurementPSFTotalMeanPowerFlux( TheSSMeasurementsParameter );
   new SSMeasurementPSFCount( TheSSMeasurementsParameter );
   new SSMeasurementMStar( TheSSMeasurementsParameter );
   new SSMeasurementNStar( TheSSMeasurementsParameter );
   new SSMeasurementPSFSNR( TheSSMeasurementsParameter );
   new SSMeasurementPSFScale( TheSSMeasurementsParameter );
   new SSMeasurementPSFScaleSNR( TheSSMeasurementsParameter );
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorProcess::Id() const
{
   return "SubframeSelector";
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorProcess::Category() const
{
   return "ImageInspection,Preprocessing";
}

// ----------------------------------------------------------------------------

uint32 SubframeSelectorProcess::Version() const
{
   return 2;
}

// ----------------------------------------------------------------------------

String SubframeSelectorProcess::Description() const
{
   return
   "<html>"
   "Facilitates subframe evaluation, selection and weighting based on several subframe "
   "quality related measurements, including estimates of star profile <i>full width at "
   "half maximum</i> (FWHM), star profile <i>eccentricity</i> and subframe "
   "<i>signal to noise ratio weight</i>. Approved/rejected subframes may be copied/moved "
   "to output directories for postprocessing. Subframe weights may be recorded in the "
   "FITS header of the copies."
   "</html>";
}

// ----------------------------------------------------------------------------

String SubframeSelectorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SubframeSelector.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SubframeSelectorProcess::DefaultInterface() const
{
   return TheSubframeSelectorInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SubframeSelectorProcess::Create() const
{
   return new SubframeSelectorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SubframeSelectorProcess::Clone( const ProcessImplementation& p ) const
{
   const SubframeSelectorInstance* instance = dynamic_cast<const SubframeSelectorInstance*>( &p );
   return (instance != nullptr) ? new SubframeSelectorInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorProcess.cpp - Released 2025-04-07T08:53:56Z
