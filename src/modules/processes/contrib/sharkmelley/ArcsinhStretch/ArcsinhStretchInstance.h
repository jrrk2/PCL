//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ArcsinhStretchInstance_h
#define __ArcsinhStretchInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ArcsinhStretchInstance : public ProcessImplementation
{
public:

   ArcsinhStretchInstance( const MetaProcess* );
   ArcsinhStretchInstance( const ArcsinhStretchInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

   void Preview( UInt16Image& ) const;

private:

   float    p_stretch;
   float    p_blackpoint;
   float    p_coarse = 0;
   float    p_fine = 0;
   pcl_bool p_protectHighlights;
   pcl_bool p_useRgbws;
   pcl_bool p_previewClipped;

   float    m_blackpointBeforeAdjustment;

   friend class ArcsinhStretchEngine;
   friend class ArcsinhStretchProcess;
   friend class ArcsinhStretchInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ArcsinhStretchInstance_h

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchInstance.h - Released 2025-04-07T08:53:56Z
