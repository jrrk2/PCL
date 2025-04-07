//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelCombinationInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelCombinationInstance_h
#define __ChannelCombinationInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/Point.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelCombinationInstance : public ProcessImplementation
{
public:

   ChannelCombinationInstance( const MetaProcess* );
   ChannelCombinationInstance( const ChannelCombinationInstance& );

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

   int ColorSpace() const
   {
      return p_colorSpace;
   }

   bool IsChannelEnabled( int c ) const
   {
      return p_channelEnabled[c];
   }

   const String& ChannelId( int c ) const
   {
      return p_channelId[c];
   }

private:

   pcl_enum p_colorSpace;
   pcl_bool p_channelEnabled[ 3 ];
   String   p_channelId[ 3 ];
   pcl_bool p_inheritAstrometricSolution;

   friend class ChannelCombinationInterface;
   friend class ChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelCombinationInstance_h

// ----------------------------------------------------------------------------
// EOF ChannelCombinationInstance.h - Released 2025-04-07T08:53:55Z
