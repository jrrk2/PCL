//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Fourier Process Module Version 1.0.4
// ----------------------------------------------------------------------------
// InverseFourierTransformInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Fourier PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __InverseFourierTransformInstance_h
#define __InverseFourierTransformInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "InverseFourierTransformParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class InverseFourierTransformInstance : public ProcessImplementation
{
public:

   InverseFourierTransformInstance( const MetaProcess* );
   InverseFourierTransformInstance( const InverseFourierTransformInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool CanExecuteGlobal( pcl::String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   String   idOfFirstComponent;
   String   idOfSecondComponent;
   pcl_enum onOutOfRangeResult;

   friend class InverseFourierTransformEngine;
   friend class InverseFourierTransformInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __InverseFourierTransformInstance_h

// ----------------------------------------------------------------------------
// EOF InverseFourierTransformInstance.h - Released 2025-04-07T08:53:55Z
