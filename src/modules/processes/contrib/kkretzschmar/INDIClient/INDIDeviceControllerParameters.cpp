//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerParameters.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "INDIDeviceControllerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IDCServerHostName* TheIDCServerHostNameParameter = nullptr;
IDCServerPort* TheIDCServerPortParameter = nullptr;
IDCServerConnect* TheIDCServerConnectParameter = nullptr;
IDCServerCommand* TheIDCServerCommandParameter = nullptr;
IDCAbort* TheIDCAbortParameter = nullptr;
IDCVerbosity* TheIDCVerbosityParameter = nullptr;
IDCGetCommandParameters* TheIDCGetCommandParametersParameter = nullptr;
IDCGetCommandResult* TheIDCGetCommandResultParameter = nullptr;

IDCDevices* TheIDCDevicesParameter = nullptr;
IDCDeviceName* TheIDCDeviceNameParameter = nullptr;
IDCDeviceLabel* TheIDCDeviceLabelParameter = nullptr;

IDCProperties* TheIDCPropertiesParameter = nullptr;
IDCPropertyName* TheIDCPropertyNameParameter = nullptr;
IDCPropertyValue* TheIDCPropertyValueParameter = nullptr;
IDCPropertyState* TheIDCPropertyStateParameter = nullptr;
IDCPropertyType* TheIDCPropertyTypeParameter = nullptr;

IDCNewProperties* TheIDCNewPropertiesParameter = nullptr;
IDCNewPropertyKey* TheIDCNewPropertyKeyParameter = nullptr;
IDCNewPropertyType* TheIDCNewPropertyTypeParameter = nullptr;
IDCNewPropertyValue* TheIDCNewPropertyValueParameter = nullptr;

// ----------------------------------------------------------------------------

IDCServerHostName::IDCServerHostName( MetaProcess* P )
   : MetaString( P )
{
   TheIDCServerHostNameParameter = this;
}

IsoString IDCServerHostName::Id() const
{
   return "serverHostName";
}

String IDCServerHostName::DefaultValue() const
{
   return "localhost";
}

// ----------------------------------------------------------------------------

IDCServerPort::IDCServerPort( MetaProcess* P )
   : MetaUInt32( P )
{
   TheIDCServerPortParameter = this;
}

IsoString IDCServerPort::Id() const
{
   return "serverPort";
}

double IDCServerPort::DefaultValue() const
{
   return 7624;
}

double IDCServerPort::MinimumValue() const
{
   return 0;
}

double IDCServerPort::MaximumValue() const
{
   return 65535;
}

// ----------------------------------------------------------------------------

IDCServerConnect::IDCServerConnect( MetaProcess* P )
   : MetaBoolean( P )
{
   TheIDCServerConnectParameter = this;
}

IsoString IDCServerConnect::Id() const
{
   return "serverConnect";
}

bool IDCServerConnect::DefaultValue() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCServerCommand::IDCServerCommand( MetaProcess* P )
   : MetaString( P )
{
   TheIDCServerCommandParameter = this;
}

IsoString IDCServerCommand::Id() const
{
   return "serverCommand";
}

// ----------------------------------------------------------------------------

IDCAbort::IDCAbort( MetaProcess* P )
   : MetaBoolean( P )
{
   TheIDCAbortParameter = this;
}

IsoString IDCAbort::Id() const
{
   return "abort";
}

// --------------------------------------------------------------------------

IDCVerbosity::IDCVerbosity( MetaProcess* P )
   : MetaInt32( P )
{
   TheIDCVerbosityParameter = this;
}

IsoString IDCVerbosity::Id() const
{
   return "verbosity";
}

double IDCVerbosity::DefaultValue() const
{
   return 1; // 0=none 1=errors/notifications 2+=informative
}

// --------------------------------------------------------------------------

IDCGetCommandParameters::IDCGetCommandParameters( MetaProcess* P )
   : MetaString( P )
{
   TheIDCGetCommandParametersParameter = this;
}

IsoString IDCGetCommandParameters::Id() const
{
   return "getCommandParameters";
}

// --------------------------------------------------------------------------

IDCGetCommandResult::IDCGetCommandResult( MetaProcess* P )
   : MetaString( P )
{
   TheIDCGetCommandResultParameter = this;
}

IsoString IDCGetCommandResult::Id() const
{
   return "getCommandResult";
}

bool IDCGetCommandResult::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCDevices::IDCDevices( MetaProcess* P )
   : MetaTable( P )
{
   TheIDCDevicesParameter = this;
}

IsoString IDCDevices::Id() const
{
   return "devices";
}

bool IDCDevices::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCDeviceName::IDCDeviceName( MetaTable* T )
   : MetaString( T )
{
   TheIDCDeviceNameParameter = this;
}

IsoString IDCDeviceName::Id() const
{
   return "deviceName";
}

bool IDCDeviceName::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCDeviceLabel::IDCDeviceLabel( MetaTable* T )
   : MetaString( T )
{
   TheIDCDeviceLabelParameter = this;
}

IsoString IDCDeviceLabel::Id() const
{
   return "deviceLabel";
}

bool IDCDeviceLabel::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCProperties::IDCProperties( MetaProcess* P )
   : MetaTable( P )
{
   TheIDCPropertiesParameter = this;
}

IsoString IDCProperties::Id() const
{
   return "properties";
}

bool IDCProperties::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyName::IDCPropertyName( MetaTable* T )
   : MetaString( T )
{
   TheIDCPropertyNameParameter = this;
}

IsoString IDCPropertyName::Id() const
{
   return "propertyName";
}

bool IDCPropertyName::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyValue::IDCPropertyValue( MetaTable* T )
   : MetaString( T )
{
   TheIDCPropertyValueParameter = this;
}

IsoString IDCPropertyValue::Id() const
{
   return "propertyValue";
}

bool IDCPropertyValue::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyType::IDCPropertyType( MetaTable* T )
   : MetaString( T )
{
   TheIDCPropertyTypeParameter = this;
}

IsoString IDCPropertyType::Id() const
{
   return "propertyType";
}

bool IDCPropertyType::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCPropertyState::IDCPropertyState( MetaTable* T )
   : MetaUInt32( T )
{
   TheIDCPropertyStateParameter = this;
}

IsoString IDCPropertyState::Id() const
{
   return "propertyState";
}

double IDCPropertyState::DefaultValue() const
{
   return 0;
}

double IDCPropertyState::MinimumValue() const
{
   return 0;
}

double IDCPropertyState::MaximumValue() const
{
   return 3;
}

bool IDCPropertyState::IsReadOnly() const
{
   return true;
}

// --------------------------------------------------------------------------

IDCNewProperties::IDCNewProperties( MetaProcess* P )
   : MetaTable( P )
{
   TheIDCNewPropertiesParameter = this;
}

IsoString IDCNewProperties::Id() const
{
   return "newProperties";
}

// --------------------------------------------------------------------------

IDCNewPropertyKey::IDCNewPropertyKey( MetaTable* T )
   : MetaString( T )
{
   TheIDCNewPropertyKeyParameter = this;
}

IsoString IDCNewPropertyKey::Id() const
{
   return "newPropertyKey";
}

// --------------------------------------------------------------------------

IDCNewPropertyType::IDCNewPropertyType( MetaTable* T )
   : MetaString( T )
{
   TheIDCNewPropertyTypeParameter = this;
}

IsoString IDCNewPropertyType::Id() const
{
   return "newPropertyType";
}

// --------------------------------------------------------------------------

IDCNewPropertyValue::IDCNewPropertyValue( MetaTable* T )
   : MetaString( T )
{
   TheIDCNewPropertyValueParameter = this;
}

IsoString IDCNewPropertyValue::Id() const
{
   return "newPropertyValue";
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerParameters.cpp - Released 2025-04-07T08:53:56Z
