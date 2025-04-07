//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// MorphologicalTransformationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MorphologicalTransformationInstance_h
#define __MorphologicalTransformationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Image.h>

#include "MorphologicalTransformationParameters.h"
#include "Structure.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class MorphologicalTransformationInstance : public ProcessImplementation
{
public:

   MorphologicalTransformationInstance( const MetaProcess* );
   MorphologicalTransformationInstance( const MorphologicalTransformationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   int Operator() const
   {
      return morphologicalOperator;
   }

   int InterlacingDistance() const
   {
      return interlacingDistance;
   }

   double LowThreshold() const
   {
      return lowThreshold;
   }

   double HighThreshold() const
   {
      return highThreshold;
   }

   int NumberOfIterations() const
   {
      return numberOfIterations;
   }

   float Amount() const
   {
      return amount;
   }

   float SelectionPoint() const
   {
      return selectionPoint;
   }

   const Structure& GetStructure() const
   {
      return structure;
   }

   Structure& GetStructure()
   {
      return structure;
   }

private:

   pcl_enum  morphologicalOperator;
   uint32    interlacingDistance;
   double    lowThreshold;
   double    highThreshold;
   uint32    numberOfIterations;
   float     amount;
   float     selectionPoint; // for the selection filter only
   Structure structure;

   friend class MorphologicalTransformationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MorphologicalTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF MorphologicalTransformationInstance.h - Released 2025-04-07T08:53:56Z
