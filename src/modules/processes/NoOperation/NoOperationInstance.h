//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoOperation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// NoOperationInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoOperation PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NoOperationInstance_h
#define __NoOperationInstance_h

#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class NoOperationInstance : public ProcessImplementation
{
public:

   NoOperationInstance( const MetaProcess* );

   bool IsHistoryUpdater( const View& ) const override;
   bool IsMaskable( const View&, const ImageWindow& mask ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NoOperationInstance_h

// ----------------------------------------------------------------------------
// EOF NoOperationInstance.h - Released 2025-04-07T08:53:56Z
