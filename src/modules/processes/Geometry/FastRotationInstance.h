//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FastRotationInstance_h
#define __FastRotationInstance_h

#include <pcl/ProcessImplementation.h>

#include "FastRotationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FastRotationInstance : public ProcessImplementation
{
public:

   FastRotationInstance( const MetaProcess*, int = FRMode::Rotate180 );
   FastRotationInstance( const FastRotationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool BeforeExecution( View& ) override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;

private:

   pcl_enum p_mode;
   pcl_bool p_noGUIMessages;    // only show warning messages on the console

   friend class FastRotationProcess;
   friend class FastRotationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FastRotationInstance_h

// ----------------------------------------------------------------------------
// EOF FastRotationInstance.h - Released 2025-04-07T08:53:55Z
