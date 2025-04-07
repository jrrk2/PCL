//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// NewImageInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NewImageInstance_h
#define __NewImageInstance_h

#include <pcl/ProcessImplementation.h>

#include "NewImageParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class NewImageInstance : public ProcessImplementation
{
public:

   NewImageInstance( const MetaProcess* );
   NewImageInstance( const NewImageInstance& );

   bool Validate( String& info ) override;
   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool ValidateParameter( void* value, const MetaParameter*, size_type tableRow ) const override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter*, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   String   id;               // Image identifier
   int32    width;            // Width in pixels
   int32    height;           // Height in pixels
   int32    numberOfChannels; // Number of channels
   pcl_enum colorSpace;       // RGB or grayscale
   pcl_enum sampleFormat;     // integer/real, bit count
   double   v0;               // Red/Gray
   double   v1;               // Green
   double   v2;               // Blue
   double   va;               // Alpha

   friend class NewImageProcess;
   friend class NewImageInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NewImageInstance_h

// ----------------------------------------------------------------------------
// EOF NewImageInstance.h - Released 2025-04-07T08:53:56Z
