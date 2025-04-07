//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.h - Released 2025-04-07T08:53:56Z
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

#ifndef __SubframeSelectorInstance_h
#define __SubframeSelectorInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "SubframeSelectorMeasureData.h"
#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorMeasureThread;
class SubframeSelectorOutputThread;

// ----------------------------------------------------------------------------

class SubframeSelectorInstance : public ProcessImplementation
{
public:

   SubframeSelectorInstance( const MetaProcess* );
   SubframeSelectorInstance( const SubframeSelectorInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& v ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void ApproveMeasurements();
   void WeightMeasurements();

   const IsoString& CacheKey() const
   {
      return m_cacheKey;
   }

private:

   struct SubframeItem
   {
      pcl_bool     enabled = true; // if disabled, skip (ignore) this image
      String       path;    // absolute file path
      String       nmlPath; // local normalization data file
      String       drzPath; // drizzle data file
      TimePoint    obsTime; // observation time, for sorting subframes
      MeasureItem* measureItem = nullptr; // for dual sorting of subframe/measure item lists

      SubframeItem() = default;
      SubframeItem( const SubframeItem& ) = default;

      SubframeItem( const String& a_path )
         : path( a_path )
      {
      }
   };

   typedef Array<SubframeItem>  subframe_list;

   // Types of run methods.
   pcl_enum           p_routine;
   pcl_bool           p_nonInteractive;

   // The set of subframes to measure.
   pcl_bool           p_fileCache;
   subframe_list      p_subframes;

   // Measurements and reporting.
   float              p_subframeScale;
   float              p_cameraGain;
   pcl_enum           p_cameraResolution;
   int32              p_siteLocalMidnight;
   pcl_enum           p_scaleUnit;
   pcl_enum           p_dataUnit;
   float              p_trimmingFactor;

   // Star detection and PSF fitting.
   int32              p_structureLayers;
   int32              p_noiseLayers;
   int32              p_hotPixelFilterRadius;
   int32              p_noiseReductionFilterRadius;
   int32              p_minStructureSize;
   float              p_sensitivity;
   float              p_peakResponse;
   float              p_brightThreshold;
   float              p_maxDistortion;
   pcl_bool           p_allowClusteredSources;
   float              p_upperLimit;
   pcl_enum           p_psfFit;
   pcl_bool           p_psfFitCircular;
   int32              p_maxPSFFits;
   Rect               p_roi = 0;
   pcl_bool           p_noNoiseAndSignalWarnings; // hidden

   // Subtractive pedestal.
   int32              p_pedestal;      // in 16-bit DN
   pcl_enum           p_pedestalMode;  // literal | default keyword | custom keyword
   String             p_pedestalKeyword;

   // Format hints.
   String             p_inputHints;
   String             p_outputHints;

   // Output files.
   String             p_outputDirectory;
   String             p_outputExtension; // ### DEPRECATED
   String             p_outputPrefix;
   String             p_outputPostfix;
   String             p_outputKeyword;
   pcl_bool           p_generateHistoryProperties; // generate initial state history properties in output files
   pcl_bool           p_overwriteExistingFiles;
   pcl_enum           p_onError;

   // The expressions.
   String             p_approvalExpression;
   String             p_weightingExpression;

   // How to view the measured subframes.
   pcl_enum           p_sortingProperty;
   pcl_enum           p_graphProperty;
   pcl_enum           p_auxGraphProperty;

   // High-level parallelism
   pcl_bool           p_useFileThreads;
   float              p_fileThreadOverload;
   int32              p_maxFileReadThreads;
   int32              p_maxFileWriteThreads;
   int                m_maxFileReadThreads = 1;
   int                m_maxFileWriteThreads = 1;

   IsoString          m_cacheKey;

   // The set of measured subframes.
   MeasureItemList    o_measures;

   bool CanTestStarDetector( String& whyNot ) const;
   void TestStarDetector();

   bool CanMeasure( String& whyNot ) const;
   void Measure();

   bool CanOutput( String& whyNot ) const;
   void Output();

   void ApplyErrorPolicy();

   typedef IndirectArray<SubframeSelectorMeasureThread>  measure_thread_list;
   typedef IndirectArray<SubframeSelectorOutputThread>   output_thread_list;

   friend struct MeasureData;
   friend class SubframeSelectorProcess;
   friend class SubframeSelectorInterface;
   friend class SubframeSelectorExpressionsInterface;
   friend class SubframeSelectorMeasurementsInterface;
   friend class SubframeSelectorMeasureThread;
   friend class SubframeSelectorOutputThread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorInstance_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.h - Released 2025-04-07T08:53:56Z
