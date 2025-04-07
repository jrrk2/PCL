//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExtractAlphaChannelsInstance_h
#define __ExtractAlphaChannelsInstance_h

#include <pcl/ProcessImplementation.h>

#include "ExtractAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ExtractAlphaChannelsInstance : public ProcessImplementation
{
public:

   ExtractAlphaChannelsInstance( const MetaProcess* );
   ExtractAlphaChannelsInstance( const ExtractAlphaChannelsInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool Validate( String& info ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   bool IsHistoryUpdater( const View& v ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   static void ParseChannelList( SortedArray<int>&, const String& );

private:

   pcl_enum channels;
   String   channelList;
   pcl_bool extractChannels;
   pcl_bool deleteChannels;

   friend class ExtractAlphaChannelsProcess;
   friend class ExtractAlphaChannelsInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExtractAlphaChannelsInstance_h

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsInstance.h - Released 2025-04-07T08:53:56Z
