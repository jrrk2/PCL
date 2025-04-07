//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// AssignICCProfileInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AssignICCProfileInstance_h
#define __AssignICCProfileInstance_h

#include <pcl/MetaParameter.h> // pcl_bool
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class AssignICCProfileInstance : public ProcessImplementation
{
public:

   AssignICCProfileInstance( const MetaProcess* );
   AssignICCProfileInstance( const AssignICCProfileInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool IsMaskable( const View&, const ImageWindow& ) const override;
   UndoFlags UndoMode( const View& ) const override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type /*tableRow*/ ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   String   p_targetProfile;
   pcl_enum p_mode;

   friend class AssignICCProfileProcess;
   friend class AssignICCProfileInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AssignICCProfileInstance_h

// ----------------------------------------------------------------------------
// EOF AssignICCProfileInstance.h - Released 2025-04-07T08:53:55Z
