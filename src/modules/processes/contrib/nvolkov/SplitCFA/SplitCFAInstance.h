//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SplitCFAInstance_h
#define __SplitCFAInstance_h

#include <pcl/FileFormatInstance.h>
#include <pcl/ProcessImplementation.h>

#include "SplitCFAParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SplitCFAThread;
struct SplitCFAThreadData;

typedef IndirectArray<SplitCFAThread> thread_list;

class SplitCFAInstance : public ProcessImplementation
{
public:

   SplitCFAInstance( const MetaProcess* );
   SplitCFAInstance( const SplitCFAInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool IsHistoryUpdater( const View& v ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   struct ImageItem
   {
      String path;      // absolute file path
      String folder;    // absolute file folder
      pcl_bool enabled = true; // if disabled, skip (ignore) this image

      ImageItem( const String& p = String(), const String& f = String() )
         : path( p )
         , folder( f )
      {
      }

      ImageItem( const ImageItem& ) = default;
   };

#define outputExtension ".xisf"

   typedef Array<ImageItem> image_list;

   image_list p_targetFrames;
   pcl_bool p_outputTree;
   pcl_bool p_outputSubDirCFA;
   String p_outputDir;
   pcl_bool p_overwrite;
   String p_prefix;
   String p_postfix;

   String o_outputViewId[4];

   template <class P>
   void SplitCFAViewImage( const GenericImage<P>& source, const View& view ); // for ExecuteOn()

   thread_list LoadTargetFrame( size_t fileIndex );
   String OutputFilePath( const String&, const String&, const size_t, const int );
   void SaveImage( const SplitCFAThread* );

   friend class SplitCFAThread;
   friend class SplitCFAInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __SplitCFAInstance_h

// ----------------------------------------------------------------------------
// EOF SplitCFAInstance.h - Released 2025-04-07T08:53:56Z
