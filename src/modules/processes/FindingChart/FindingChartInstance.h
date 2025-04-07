//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard FindingChart Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// FindingChartInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard FindingChart PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FindingChartInstance_h
#define __FindingChartInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FindingChartInstance : public ProcessImplementation
{
public:

   FindingChartInstance( const MetaProcess* );
   FindingChartInstance( const FindingChartInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   int32    p_chartSize;         // px, >= 512
   pcl_bool p_autoChartResolution;
   float    p_chartResolution;   // as, ignored if p_autoChartResolution
   pcl_bool p_autoChartMaxMagnitude;
   float    p_chartMaxMagnitude; // mag, ignored if p_autoChartMaxMagnitude
   pcl_bool p_drawGrid;
   pcl_bool p_drawEcliptic;
   pcl_bool p_drawGalacticEquator;
   pcl_bool p_drawConstellationBorders;
   pcl_bool p_drawConstellationLines;
   pcl_bool p_drawConstellationNames;
   pcl_bool p_drawStars;
   pcl_bool p_drawStarNames;
   pcl_bool p_drawMessierObjects;
   pcl_bool p_drawNGCObjects;
   pcl_bool p_generateBitmapFile;
   String   p_outputDirectory;
   String   p_outputSuffix;
   pcl_bool p_closeOutputWindow; // ignored if !p_generateBitmapFile
   uint32   p_imageRegionColor;
   uint32   p_imageRegionBorderColor;
   uint32   p_gridColor;
   uint32   p_gridTextColor;
   uint32   p_eclipticColor;
   uint32   p_eclipticTextColor;
   uint32   p_galacticEquatorColor;
   uint32   p_galacticEquatorTextColor;
   uint32   p_constellationBorderColor;
   uint32   p_constellationLineColor;
   uint32   p_constellationTextColor;
   uint32   p_starTextColor;
   uint32   p_messierTextColor;
   uint32   p_ngcTextColor;

   friend class FindingChartEngine;
   friend class FindingChartProcess;
   friend class FindingChartInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __FindingChartInstance_h

// ----------------------------------------------------------------------------
// EOF FindingChartInstance.h - Released 2025-04-07T08:53:55Z
