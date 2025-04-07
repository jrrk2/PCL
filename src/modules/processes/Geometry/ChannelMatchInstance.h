//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// ChannelMatchInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelMatchInstance_h
#define __ChannelMatchInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/Point.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelMatchInstance : public ProcessImplementation
{
public:

   ChannelMatchInstance( const MetaProcess* );
   ChannelMatchInstance( const ChannelMatchInstance& );

   bool Validate( pcl::String& info ) override;
   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   pcl_bool p_channelEnabled[ 3 ];
   FPoint   p_channelOffset[ 3 ];
   double   p_channelFactor[ 3 ];

   friend class ChannelMatchEngine;
   friend class ChannelMatchInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelMatchInstance_h

// ----------------------------------------------------------------------------
// EOF ChannelMatchInstance.h - Released 2025-04-07T08:53:55Z
