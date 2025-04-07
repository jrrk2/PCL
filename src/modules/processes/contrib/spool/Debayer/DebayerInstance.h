//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 1.12.1
// ----------------------------------------------------------------------------
// DebayerInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __DebayerInstance_h
#define __DebayerInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS FileFormatInstance;

class DebayerInstance : public ProcessImplementation
{
public:

   DebayerInstance( const MetaProcess* );
   DebayerInstance( const DebayerInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   struct Item
   {
      pcl_bool enabled = true;
      String   path;

      Item() = default;
      Item( const Item& ) = default;

      Item( const String& path_ ) : path( path_ )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }
   };

   typedef Array<Item>  item_list;

   // Process parameters
   pcl_enum   p_bayerPattern;
   pcl_enum   p_debayerMethod;
   int32      p_fbddNoiseReduction;
   pcl_bool   p_showImages;        // optional for view execution only
   String     p_cfaSourceFilePath; // ...
   item_list  p_targets;
   pcl_bool   p_noGUIMessages; // ### DEPRECATED

   // Noise estimates
   pcl_bool   p_evaluateNoise;
   pcl_enum   p_noiseEvaluationAlgorithm;

   // PSF signal estimates
   pcl_bool   p_evaluateSignal;
   int32      p_structureLayers;
   float      p_saturationThreshold;
   pcl_bool   p_saturationRelative;
   int32      p_noiseLayers;
   int32      p_hotPixelFilterRadius;
   int32      p_noiseReductionFilterRadius;
   int32      p_minStructureSize;
   pcl_enum   p_psfType;
   float      p_psfGrowth;
   int32      p_maxStars;

   // Format hints
   String      p_inputHints;
   String      p_outputHints;

   // Output images
   pcl_bool    p_outputRGBImages;
   pcl_bool    p_outputSeparateChannels;
   String      p_outputDirectory;
   String      p_outputExtension; // ### DEPRECATED
   String      p_outputPrefix;
   String      p_outputPostfix;
   pcl_bool    p_generateHistoryProperties; // generate initial state history properties in output files
   pcl_bool    p_generateFITSKeywords;      // generate deprecated FITS keywords for compatibility
   pcl_bool    p_overwriteExistingFiles;
   pcl_enum    p_onError;

   // High-level parallelism
   pcl_bool    p_useFileThreads;
   float       p_fileThreadOverload;
   int32       p_maxFileReadThreads;
   int32       p_maxFileWriteThreads;
   pcl_bool    p_memoryLoadControl;
   float       p_memoryLoadLimit;
   int         m_maxFileReadThreads = 1;
   int         m_maxFileWriteThreads = 1;

   // Read-only output properties, view execution.
   String     o_imageId;
   StringList o_channelIds = StringList( size_type( 3 ) );
   Vector     o_psfTotalFluxEstimates = Vector( 0.0, 3 );
   Vector     o_psfTotalPowerFluxEstimates = Vector( 0.0, 3 );
   Vector     o_psfTotalMeanFluxEstimates = Vector( 0.0, 3 );
   Vector     o_psfTotalMeanPowerFluxEstimates = Vector( 0.0, 3 );
   Vector     o_psfMStarEstimates = Vector( 0.0, 3 );
   Vector     o_psfNStarEstimates = Vector( 0.0, 3 );
   IVector    o_psfCounts = IVector( 0, 3 );
   Vector     o_noiseEstimates = Vector( 0.0, 3 );
   Vector     o_noiseFractions = Vector( 0.0, 3 );
   Vector     o_noiseScaleLow = Vector( 0.0, 3 );
   Vector     o_noiseScaleHigh = Vector( 0.0, 3 );
   StringList o_noiseAlgorithms = StringList( size_type( 3 ) );

   // Read-only output properties, batch execution.
   struct OutputFileData
   {
      String     filePath;
      StringList channelFilePaths = StringList( size_type( 3 ) );
      Vector     psfTotalFluxEstimates = Vector( 0.0, 3 );
      Vector     psfTotalPowerFluxEstimates = Vector( 0.0, 3 );
      Vector     psfTotalMeanFluxEstimates = Vector( 0.0, 3 );
      Vector     psfTotalMeanPowerFluxEstimates = Vector( 0.0, 3 );
      Vector     psfMStarEstimates = Vector( 0.0, 3 );
      Vector     psfNStarEstimates = Vector( 0.0, 3 );
      IVector    psfCounts = IVector( 0, 3 );
      Vector     noiseEstimates = Vector( 0.0, 3 );
      Vector     noiseFractions = Vector( 0.0, 3 );
      Vector     noiseScaleLow = Vector( 0.0, 3 );
      Vector     noiseScaleHigh = Vector( 0.0, 3 );
      StringList noiseAlgorithms = StringList( size_type( 3 ) );
   };
   Array<OutputFileData> o_outputFileData;

   pcl_enum BayerPatternFromTarget( const View& ) const;
   pcl_enum BayerPatternFromTarget( FileFormatInstance& ) const;
   static pcl_enum BayerPatternFromTargetProperty( const Variant& );
   static pcl_enum BayerPatternFromTargetPropertyValue( const IsoString&, int dx = 0, int dy = 0 );

   IsoString CFAPatternIdFromTarget( const View&, bool xtrans ) const;
   IsoString CFAPatternIdFromTarget( FileFormatInstance&, bool xtrans ) const;
   static IsoString CFAPatternIdFromTargetProperty( const Variant&, int dx = 0, int dy = 0 );

   static bool IsXTransCFAFromTarget( const View& );
   static bool IsXTransCFAFromTarget( FileFormatInstance& );
   static bool IsXTransCFAFromTargetProperty( const Variant& );

   static IMatrix XTransPatternFiltersFromTarget( const View& );
   static IMatrix XTransPatternFiltersFromTarget( FileFormatInstance& );
   static IMatrix XTransPatternFiltersFromTargetProperty( const Variant& );

   static FMatrix sRGBConversionMatrixFromTarget( const View& );
   static FMatrix sRGBConversionMatrixFromTarget( FileFormatInstance& );
   static FMatrix sRGBConversionMatrixFromTargetProperty( const Variant& );

   void EvaluateSignalAndNoise( Vector& psfTotalFluxEstimates, Vector& psfTotalPowerFluxEstimates,
                                Vector& psfTotalMeanFluxEstimates, Vector& psfTotalMeanPowerFluxEstimates,
                                Vector& psfMStarEstimates, Vector& psfNStarEstimates, IVector& psfCounts,
                                Vector& noiseEstimates, Vector& noiseFractions,
                                Vector& noiseScaleLow, Vector& noiseScaleHigh, StringList& noiseAlgorithms,
                                const ImageVariant&, const IsoString& cfaPattern ) const;

   void ApplyErrorPolicy();

   friend class DebayerProcess;
   friend class DebayerInterface;
   friend class DebayerEngine;
   friend class DebayerFileThread;
   friend class SignalAndNoiseEvaluationThread;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __DebayerInstance_h

// ----------------------------------------------------------------------------
// EOF DebayerInstance.h - Released 2025-04-07T08:53:56Z
