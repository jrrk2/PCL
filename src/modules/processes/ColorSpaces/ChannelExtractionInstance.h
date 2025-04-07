//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// ChannelExtractionInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ChannelExtractionInstance_h
#define __ChannelExtractionInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelExtractionInstance : public ProcessImplementation
{
public:

   ChannelExtractionInstance( const MetaProcess* );
   ChannelExtractionInstance( const ChannelExtractionInstance& );

   bool Validate( pcl::String& info ) override;
   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
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

   int SampleFormat() const
   {
      return p_sampleFormat;
   }

   bool IsInheritAstrometricSolutionEnabled() const
   {
      return p_inheritAstrometricSolution;
   }

private:

   pcl_enum p_colorSpace;
   pcl_bool p_channelEnabled[ 3 ];
   String   p_channelId[ 3 ];
   pcl_enum p_sampleFormat;
   pcl_bool p_inheritAstrometricSolution;

   friend class ChannelExtractionInterface;
   friend class ExtractCIELAction;
   friend class SplitRGBChannelsAction;
   friend class ExtractRedChannelAction;
   friend class ExtractGreenChannelAction;
   friend class ExtractBlueChannelAction;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelExtractionInstance_h

// ----------------------------------------------------------------------------
// EOF ChannelExtractionInstance.h - Released 2025-04-07T08:53:55Z
