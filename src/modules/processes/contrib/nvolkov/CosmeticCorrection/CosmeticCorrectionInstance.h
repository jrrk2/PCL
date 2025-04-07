//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 1.4.1
// ----------------------------------------------------------------------------
// CosmeticCorrectionInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CosmeticCorrectionInstance_h
#define __CosmeticCorrectionInstance_h

#include <pcl/Convolution.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/ProcessImplementation.h>

#include "CosmeticCorrectionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CCThread;
struct CCThreadData;

typedef IndirectArray<CCThread> thread_list;

class CosmeticCorrectionInstance : public ProcessImplementation
{
public:

   CosmeticCorrectionInstance( const MetaProcess* );
   CosmeticCorrectionInstance( const CosmeticCorrectionInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& v ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   struct ImageItem
   {
      pcl_bool enabled = true; // if disabled, skip (ignore) this image
      String path;             // absolute file path

      ImageItem( const String& p = String() )
         : path( p )
      {
      }

      ImageItem( const ImageItem& ) = default;

      bool IsValid() const
      {
         return !enabled || !path.IsEmpty();
      }
   };

   struct DefectItem
   {
      pcl_bool enabled; // if disabled, skip (ignore) this defect
      pcl_bool isRow;   // true == Row, false == Col
      uint16 address;   // address of selected col or row
      pcl_bool isRange; // true == range of selected pixels defined, false == All pixels selected as bad
      uint16 begin;     // first bad pixel in col/row
      uint16 end;       // last bad pixel in col/row

      DefectItem( const bool a_enabled = false, const bool a_isRow = false,
                  const uint16 a_address = 0, const bool a_isRange = false,
                  const uint16 a_begin = 0, const uint16 a_end = 0 )
         : enabled( a_enabled )
         , isRow( a_isRow )
         , address( a_address )
         , isRange( a_isRange )
         , begin( a_begin )
         , end( a_end )
      {
      }

      DefectItem( const DefectItem& ) = default;
   };

#define MapImg UInt8Image
#define DarkImg UInt16Image

   typedef Array<ImageItem> image_list;
   typedef Array<DefectItem> defect_list;

   Rect    m_geometry;
   MapImg* m_mapDarkHot = nullptr;
   MapImg* m_mapDarkCold = nullptr;

   image_list  p_targetFrames;
   String      p_outputDir;
   String      p_outputExtension; // ### DEPRECATED
   pcl_bool    p_generateHistoryProperties; // generate initial state history properties in output files
   pcl_bool    p_overwrite;
   String      p_prefix;
   String      p_postfix;

   pcl_bool    p_cfa;
   float       p_amount;

   pcl_bool    p_useMasterDark;
   String      p_masterDark;
   pcl_bool    p_hotDarkCheck;
   float       p_hotDarkLevel;
   pcl_bool    p_coldDarkCheck;
   float       p_coldDarkLevel;

   pcl_bool    p_useAutoDetect;
   pcl_bool    p_hotAutoCheck;
   float       p_hotAutoValue;
   pcl_bool    p_coldAutoCheck;
   float       p_coldAutoValue;

   pcl_bool    p_useDefectList;
   defect_list p_defects;

   bool CanExecute( String& whyNot ) const;
   MorphologicalTransformation* BkgMT() const;
   MorphologicalTransformation* MedMT() const;
   MorphologicalTransformation* AvrMT() const;
   DarkImg GetDark( const String& );
   void PrepareMasterDarkMaps();

   thread_list LoadTargetFrame( const String&, const CCThreadData& );
   String OutputFilePath( const String&, const size_t );
   void SaveImage( const CCThread* );

   friend class CCThread;
   friend class CosmeticCorrectionInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __CosmeticCorrectionInstance_h

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionInstance.h - Released 2025-04-07T08:53:56Z
