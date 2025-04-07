//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// SampleFormatConversionInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SampleFormatConversionInstance_h
#define __SampleFormatConversionInstance_h

#include <pcl/ProcessImplementation.h>

#include "SampleFormatConversionParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SampleFormatConversionInstance : public ProcessImplementation
{
public:

   SampleFormatConversionInstance( const MetaProcess*, int = SampleFormatConversion::ToFloat );
   SampleFormatConversionInstance( const SampleFormatConversionInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   pcl_enum sampleFormat;

   friend class SampleFormatConversionProcess;
   friend class SampleFormatConversionInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SampleFormatConversionInstance_h

// ----------------------------------------------------------------------------
// EOF SampleFormatConversionInstance.h - Released 2025-04-07T08:53:56Z
