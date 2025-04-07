//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GREYCstorationProcess.h - Released 2025-04-07T08:53:56Z
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

#ifndef __GREYCstorationProcess_h
#define __GREYCstorationProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class GREYCstorationProcess : public MetaProcess
{
public:

   GREYCstorationProcess();

   IsoString Id() const override;
   IsoString Category() const override;
   uint32 Version() const override;
   String Description() const override;
   String IconImageSVGFile() const override;
   ProcessInterface* DefaultInterface() const override;
   ProcessImplementation* Create() const override;
   ProcessImplementation* Clone( const ProcessImplementation& ) const override;
   bool CanProcessCommandLines() const override;
   int ProcessCommandLine( const StringList& ) const override;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GREYCstorationProcess* TheGREYCstorationProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GREYCstorationProcess_h

// ----------------------------------------------------------------------------
// EOF GREYCstorationProcess.h - Released 2025-04-07T08:53:56Z
