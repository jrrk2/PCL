//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// CreateAlphaChannelsInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CreateAlphaChannelsInstance_h
#define __CreateAlphaChannelsInstance_h

#include <pcl/ProcessImplementation.h>

#include "CreateAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CreateAlphaChannelsInstance : public ProcessImplementation
{
public:

   CreateAlphaChannelsInstance( const MetaProcess* );
   CreateAlphaChannelsInstance( const CreateAlphaChannelsInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   pcl_bool fromImage;
   String   imageId;
   pcl_bool invertSourceImage;
   pcl_bool closeSourceImage;
   double   transparency;
   pcl_bool replace;
   int32    count; // only accesible through the command-line interface

   friend class CreateAlphaChannelsProcess;
   friend class CreateAlphaChannelsInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CreateAlphaChannelsInstance_h

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsInstance.h - Released 2025-04-07T08:53:56Z
