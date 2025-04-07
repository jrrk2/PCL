//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// LRGBCombinationInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LRGBCombinationInstance_h
#define __LRGBCombinationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Point.h>
#include <pcl/MetaParameter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class LRGBCombinationInstance : public ProcessImplementation
{
public:

   LRGBCombinationInstance( const MetaProcess* );
   LRGBCombinationInstance( const LRGBCombinationInstance& );

   bool Validate( pcl::String& info ) override;
   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   // Array indexes: 0=R, 1=G, 2=B, 3=L

   pcl_bool channelEnabled[ 4 ];
   String   channelId[ 4 ];
   double   channelWeight[ 4 ];
   float    luminanceMTF;
   float    saturationMTF;
   pcl_bool clipHighlights;
   pcl_bool noiseReduction;
   uint32   numberOfRemovedWaveletLayers;
   uint32   numberOfProtectedWaveletLayers;
   pcl_bool p_inheritAstrometricSolution;

   friend class LRGBCombinationInterface;
   friend class LRGBChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LRGBCombinationInstance_h

// ----------------------------------------------------------------------------
// EOF LRGBCombinationInstance.h - Released 2025-04-07T08:53:55Z
