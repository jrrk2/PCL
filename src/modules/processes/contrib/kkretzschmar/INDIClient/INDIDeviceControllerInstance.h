//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIDeviceControllerInstance_h
#define __INDIDeviceControllerInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "INDIClient.h" // for INDIxxxListItemArray declarations
#include "INDIDeviceControllerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIDeviceControllerInstance : public ProcessImplementation
{
public:

   INDIDeviceControllerInstance( const MetaProcess* );
   INDIDeviceControllerInstance( const INDIDeviceControllerInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   String                       p_serverHostName;
   uint32                       p_serverPort;
   pcl_bool                     p_serverConnect;
   String                       p_serverCommand;
   pcl_bool                     p_abort = false;
   int32                        p_verbosity;
   INDINewPropertyListItemArray p_newProperties;
   String                       p_getCommandParameters;

   INDIDeviceListItemArray      o_devices;
   INDIPropertyListItemArray    o_properties;
   String                       o_getCommandResult;

   void AcquireINDIClientProperties();

   friend class INDIDeviceControllerInterface;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIDeviceControllerInstance_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInstance.h - Released 2025-04-07T08:53:56Z
