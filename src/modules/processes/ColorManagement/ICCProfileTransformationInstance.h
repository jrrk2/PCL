//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// ICCProfileTransformationInstance.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ICCProfileTransformationInstance_h
#define __ICCProfileTransformationInstance_h

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ICCProfileTransformationInstance : public ProcessImplementation
{
public:

   ICCProfileTransformationInstance( const MetaProcess* );
   ICCProfileTransformationInstance( const ICCProfileTransformationInstance& );

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
   pcl_bool p_toDefaultProfile;
   pcl_enum p_renderingIntent;
   pcl_bool p_useBlackPointCompensation;
   pcl_bool p_useFloatingPointTransformation;

   friend class ICCProfileTransformationProcess;
   friend class ICCProfileTransformationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ICCProfileTransformationInstance_h

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationInstance.h - Released 2025-04-07T08:53:55Z
