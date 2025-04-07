//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// FilterManagerInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FilterManagerInstance_h
#define __FilterManagerInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FilterManagerInstance : public ProcessImplementation
{
public:

   FilterManagerInstance( const MetaProcess* );
   FilterManagerInstance( const FilterManagerInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   pcl_enum p_task;
   String   p_path;
   pcl_bool p_noGUIMessages;

   friend class FilterManagerProcess;
   friend class FilterManagerInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FilterManagerInstance_h

// ----------------------------------------------------------------------------
// EOF FilterManagerInstance.h - Released 2025-04-07T08:53:56Z
