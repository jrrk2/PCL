//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CometAlignment Process Module Version 1.3.9
// ----------------------------------------------------------------------------
// CometAlignmentInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2023 Nikolay Volkov
// Copyright (c) 2019-2023 Juan Conejero (PTeam)
// Copyright (c) 2003-2023 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CometAlignmentInstance_h
#define __CometAlignmentInstance_h

#include <pcl/FileFormatInstance.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/TimePoint.h>

#include "CometAlignmentParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CometAlignmentInstance : public ProcessImplementation
{
public:

   CometAlignmentInstance( const MetaProcess* );
   CometAlignmentInstance( const CometAlignmentInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter*, size_type tableRow ) const override;

private:

   struct ImageItem
   {
      String   path;           // absolute file path
      pcl_bool enabled = true; // if disabled, skip (ignore) this image
      String   date;           // ### DEPRECATED
      double   jd = 0;         // observation time, Julian date
      DPoint   position = 0;   // comet coordinates
      pcl_bool fixed = false;  // true if position is user-defined, for dynamic GUI interaction
      String   drzPath;        // drizzle data file

      ImageItem( const String& p, const String& d )
         : path( p )
         , jd( TimePoint( d ).JD() )
      {
      }

      ImageItem() = default;
      ImageItem( const ImageItem& ) = default;

      bool operator <( const ImageItem& item ) const
      {
         if ( jd != item.jd )
            return jd < item.jd;
         return path < item.path;
      }

      bool operator ==( const ImageItem& item ) const
      {
         return jd == item.jd && path == item.path;
      }
   };

   typedef Array<ImageItem> image_list;

   // Working data and parameters
   image_list p_targets;
   uint32     p_referenceIndex;
   pcl_bool   p_fitPSF;
   String     p_operandImageFilePath;
   pcl_bool   p_operandSubtractAligned; // true -> align operand and subtract from target, false -> subtract operand from target and align
   pcl_bool   p_operandLinearFit;
   float      p_operandLinearFitLow;
   float      p_operandLinearFitHigh;
   pcl_bool   p_operandNormalize;
   pcl_bool   p_drizzleWriteStarAlignedImage;
   pcl_bool   p_drizzleWriteCometAlignedImage;
   pcl_enum   p_pixelInterpolation;
   float      p_linearClampingThreshold;

   // Format hints
   String     p_inputHints;
   String     p_outputHints;

   // Output images
   String     p_outputDirectory;
   String     p_outputExtension; // ### DEPRECATED
   String     p_outputPrefix;
   String     p_outputPostfix;
   pcl_bool   p_generateHistoryProperties; // generate initial state history properties in output files
   pcl_bool   p_overwriteExistingFiles;
   pcl_bool   p_generateCometPathMap;
   pcl_enum   p_onError;

   // High-level parallelism
   pcl_bool   p_useFileThreads;
   float      p_fileThreadOverload;
   int32      p_maxFileReadThreads;
   int32      p_maxFileWriteThreads;

   mutable Rect        m_geometry = 0;
   mutable AtomicInt   m_geometryAvailable;
   PixelInterpolation* m_pixelInterpolation = nullptr;
   int                 m_maxFileReadThreads = 1;
   int                 m_maxFileWriteThreads = 1;

   PixelInterpolation* NewPixelInterpolation();

   void ApplyErrorPolicy();

   friend class CometAlignmentThread;
   friend class CometAlignmentInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif // __CometAlignmentInstance_h

// ----------------------------------------------------------------------------
// EOF CometAlignmentInstance.h - Released 2025-04-07T08:53:56Z
