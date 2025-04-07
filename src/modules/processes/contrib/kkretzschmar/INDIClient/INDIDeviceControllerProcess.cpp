//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "INDIDeviceControllerProcess.h"
#include "INDIDeviceControllerInstance.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIDeviceControllerParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDIDeviceControllerProcess* TheINDIDeviceControllerProcess = nullptr;

// ----------------------------------------------------------------------------

INDIDeviceControllerProcess::INDIDeviceControllerProcess()
{
   TheINDIDeviceControllerProcess = this;

   new IDCServerHostName( TheINDIDeviceControllerProcess );
   new IDCServerPort( TheINDIDeviceControllerProcess );
   new IDCServerConnect( TheINDIDeviceControllerProcess );
   new IDCServerCommand( TheINDIDeviceControllerProcess );
   new IDCAbort( TheINDIDeviceControllerProcess );
   new IDCVerbosity( TheINDIDeviceControllerProcess );

   new IDCNewProperties( TheINDIDeviceControllerProcess );
   new IDCNewPropertyKey( TheIDCNewPropertiesParameter );
   new IDCNewPropertyType( TheIDCNewPropertiesParameter );
   new IDCNewPropertyValue( TheIDCNewPropertiesParameter );

   new IDCProperties( TheINDIDeviceControllerProcess );
   new IDCPropertyName( TheIDCPropertiesParameter );
   new IDCPropertyValue( TheIDCPropertiesParameter );
   new IDCPropertyState( TheIDCPropertiesParameter );
   new IDCPropertyType( TheIDCPropertiesParameter );

   new IDCDevices( TheINDIDeviceControllerProcess );
   new IDCDeviceName( TheIDCDevicesParameter );
   new IDCDeviceLabel( TheIDCDevicesParameter );

   new IDCGetCommandResult( TheINDIDeviceControllerProcess );
   new IDCGetCommandParameters( TheINDIDeviceControllerProcess );
}

// ----------------------------------------------------------------------------

IsoString INDIDeviceControllerProcess::Id() const
{
   return "IndigoDeviceController";
}

// ----------------------------------------------------------------------------

IsoString INDIDeviceControllerProcess::Category() const
{
   return "Instrumentation";
}

// ----------------------------------------------------------------------------

uint32 INDIDeviceControllerProcess::Version() const
{
   return 0x200; // required
}

// ----------------------------------------------------------------------------

String INDIDeviceControllerProcess::Description() const
{
   return "<html>"
          "<p>An Indigo client process for controlling Indigo devices.</p>"
          "</html>";
}

// ----------------------------------------------------------------------------

String INDIDeviceControllerProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/IndigoDeviceController.svg";
}

// ----------------------------------------------------------------------------

bool INDIDeviceControllerProcess::PrefersGlobalExecution() const
{
   return true;
}

// ----------------------------------------------------------------------------

ProcessInterface* INDIDeviceControllerProcess::DefaultInterface() const
{
   return TheINDIDeviceControllerInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIDeviceControllerProcess::Create() const
{
   return new INDIDeviceControllerInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* INDIDeviceControllerProcess::Clone( const ProcessImplementation& p ) const
{
   const INDIDeviceControllerInstance* instPtr = dynamic_cast<const INDIDeviceControllerInstance*>( &p );
   return ( instPtr != nullptr ) ? new INDIDeviceControllerInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool INDIDeviceControllerProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

#if 0
static void ShowHelp()
{
   Console().Write(
"<raw>"
"Nothing to show."
"</raw>" );
}
#endif

int INDIDeviceControllerProcess::ProcessCommandLine( const StringList& argv ) const
{
   // ### TODO
   return 0;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerProcess.cpp - Released 2025-04-07T08:53:56Z
