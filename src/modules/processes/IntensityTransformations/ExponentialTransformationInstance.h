//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExponentialTransformationInstance_h
#define __ExponentialTransformationInstance_h

#include <pcl/Image.h>
#include <pcl/ProcessImplementation.h>

#include "ExponentialTransformationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ExponentialTransformationInstance : public ProcessImplementation
{
public:

   ExponentialTransformationInstance( const MetaProcess* );
   ExponentialTransformationInstance( const ExponentialTransformationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View& v, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

   String TypeAsString() const;

   void Preview( UInt16Image& ) const;

private:

   pcl_enum p_type;
   float    p_order;
   float    p_sigma;
   pcl_bool p_useLightnessMask;

   friend class ExponentialTransformationEngine;
   friend class ExponentialTransformationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExponentialTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationInstance.h - Released 2025-04-07T08:53:56Z
