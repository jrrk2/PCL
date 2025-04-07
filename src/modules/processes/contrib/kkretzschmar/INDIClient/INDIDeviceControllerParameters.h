//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerParameters.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIDeviceControllerParameters_h
#define __INDIDeviceControllerParameters_h

#include <pcl/MetaParameter.h>

#include "INDIParamListTypes.h"

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class IDCServerHostName : public MetaString
{
public:

   IDCServerHostName( MetaProcess* );

   IsoString Id() const override;
   String DefaultValue() const override;
};

extern IDCServerHostName* TheIDCServerHostNameParameter;

// ----------------------------------------------------------------------------

class IDCServerPort : public MetaUInt32
{
public:

   IDCServerPort( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
};

extern IDCServerPort* TheIDCServerPortParameter;

// ----------------------------------------------------------------------------

class IDCServerConnect : public MetaBoolean
{
public:

   IDCServerConnect( MetaProcess* );

   IsoString Id() const override;
   bool DefaultValue() const override;
};

extern IDCServerConnect* TheIDCServerConnectParameter;

// ----------------------------------------------------------------------------

class IDCServerCommand : public MetaString
{
public:

   IDCServerCommand( MetaProcess* );

   IsoString Id() const override;
};

extern IDCServerCommand* TheIDCServerCommandParameter;

// ----------------------------------------------------------------------------

class IDCAbort : public MetaBoolean
{
public:

   IDCAbort( MetaProcess* );

   IsoString Id() const override;
};

extern IDCAbort* TheIDCAbortParameter;

// ----------------------------------------------------------------------------

class IDCVerbosity : public MetaInt32
{
public:

   IDCVerbosity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern IDCVerbosity* TheIDCVerbosityParameter;

// ----------------------------------------------------------------------------

class IDCGetCommandParameters : public MetaString
{
public:

   IDCGetCommandParameters( MetaProcess* );

   IsoString Id() const override;
};

extern IDCGetCommandParameters* TheIDCGetCommandParametersParameter;

// ----------------------------------------------------------------------------

class IDCGetCommandResult : public MetaString
{
public:

   IDCGetCommandResult( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCGetCommandResult* TheIDCGetCommandResultParameter;

// ----------------------------------------------------------------------------

class IDCDevices : public MetaTable
{
public:

   IDCDevices( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCDevices* TheIDCDevicesParameter;

// ----------------------------------------------------------------------------

class IDCDeviceName : public MetaString
{
public:

   IDCDeviceName( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCDeviceName* TheIDCDeviceNameParameter;

// ----------------------------------------------------------------------------

class IDCDeviceLabel : public MetaString
{
public:

   IDCDeviceLabel( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCDeviceLabel* TheIDCDeviceLabelParameter;

// ----------------------------------------------------------------------------

class IDCProperties : public MetaTable
{
public:

   IDCProperties( MetaProcess* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCProperties* TheIDCPropertiesParameter;

// ----------------------------------------------------------------------------

class IDCPropertyName : public MetaString
{
public:

   IDCPropertyName( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyName* TheIDCPropertyNameParameter;

// ----------------------------------------------------------------------------

class IDCPropertyValue : public MetaString
{
public:

   IDCPropertyValue( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyValue* TheIDCPropertyValueParameter;

// ----------------------------------------------------------------------------

class IDCPropertyType : public MetaString
{
public:

   IDCPropertyType( MetaTable* );

   IsoString Id() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyType* TheIDCPropertyTypeParameter;

// ----------------------------------------------------------------------------

class IDCPropertyState : public MetaUInt32
{
public:

   IDCPropertyState( MetaTable* );

   IsoString Id() const override;
   double DefaultValue() const override;
   double MinimumValue() const override;
   double MaximumValue() const override;
   bool IsReadOnly() const override;
};

extern IDCPropertyState* TheIDCPropertyStateParameter;

// ----------------------------------------------------------------------------

class IDCNewProperties : public MetaTable
{
public:

   IDCNewProperties( MetaProcess* );

   IsoString Id() const override;
};

extern IDCNewProperties* TheIDCNewPropertiesParameter;

// ----------------------------------------------------------------------------

class IDCNewPropertyKey : public MetaString
{
public:

   IDCNewPropertyKey( MetaTable* );

   IsoString Id() const override;
};

extern IDCNewPropertyKey* TheIDCNewPropertyKeyParameter;

// ----------------------------------------------------------------------------

class IDCNewPropertyType : public MetaString
{
public:

   IDCNewPropertyType( MetaTable* );

   IsoString Id() const override;
};

extern IDCNewPropertyType* TheIDCNewPropertyTypeParameter;

// ----------------------------------------------------------------------------

class IDCNewPropertyValue : public MetaString
{
public:

   IDCNewPropertyValue( MetaTable* );

   IsoString Id() const override;
};

extern IDCNewPropertyValue* TheIDCNewPropertyValueParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // INDIDeviceControllerParameters_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerParameters.h - Released 2025-04-07T08:53:56Z
