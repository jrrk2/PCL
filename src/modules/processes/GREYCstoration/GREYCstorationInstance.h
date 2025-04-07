//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GREYCstorationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#ifndef __GREYCstorationInstance_h
#define __GREYCstorationInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class GREYCstorationInstance : public ProcessImplementation
{
public:

   GREYCstorationInstance( const MetaProcess* );
   GREYCstorationInstance( const GREYCstorationInstance& );

   void Assign( const ProcessImplementation& ) override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;

private:

   float    amplitude;
   int32    numberOfIterations;
   float    sharpness;
   float    anisotropy;
   float    alpha;
   float    sigma;
   pcl_bool fastApproximation;
   float    precision;
   float    spatialStepSize;
   float    angularStepSize;
   pcl_enum interpolation;
   pcl_bool coupledChannels;

   friend class GREYCstorationProcess;
   friend class GREYCstorationInterface;
   friend class GREYCstorationEngine;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __GREYCstorationInstance_h

// ----------------------------------------------------------------------------
// EOF GREYCstorationInstance.h - Released 2025-04-07T08:53:56Z
