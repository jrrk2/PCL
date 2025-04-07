//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// RescaleInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RescaleInstance_h
#define __RescaleInstance_h

#include <pcl/ProcessImplementation.h>

#include "RescaleParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RescaleInstance : public ProcessImplementation
{
public:

   RescaleInstance( const MetaProcess*, int = RescalingMode::RGBK );
   RescaleInstance( const RescaleInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   pcl_enum p_mode;

   friend class RescaleProcess;
   friend class RescaleInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RescaleInstance_h

// ----------------------------------------------------------------------------
// EOF RescaleInstance.h - Released 2025-04-07T08:53:56Z
