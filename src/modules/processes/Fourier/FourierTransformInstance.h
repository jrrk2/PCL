//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// FourierTransformInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FourierTransformInstance_h
#define __FourierTransformInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h>

#include "FourierTransformParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FourierTransformInstance : public ProcessImplementation
{
public:

   FourierTransformInstance( const MetaProcess* );
   FourierTransformInstance( const FourierTransformInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsHistoryUpdater( const View& ) const override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   pcl_bool radialCoordinates;
   pcl_bool centered;

   friend class FourierTransformEngine;
   friend class FourierTransformInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FourierTransformInstance_h

// ----------------------------------------------------------------------------
// EOF FourierTransformInstance.h - Released 2025-04-07T08:53:55Z
