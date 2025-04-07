//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// IndigoClient.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef INDIGOCLIENT_H
#define INDIGOCLIENT_H

#include "IndigoDefs.h"

#include <indigo/indigo_client.h>
#include <indigo/indigo_names.h>

#include <functional>
#include <iostream>
#include <set>
#include <string>

class IndigoClient
{
public:
  enum class ReturnCode
  {
    INDIGO_OK,
    INDIGO_DUPLICATED,
    INDIGO_ERROR
  };

   explicit IndigoClient( const char* clientName );
   IndigoClient( const char* clientName, const char* host, int32_t port );
   IndigoClient( const IndigoClient& ) = delete;
   IndigoClient( IndigoClient& ) = delete;

   ~IndigoClient(); // do not make it virtual, no vtable allowed

   ReturnCode connectServer( std::ostream& errMessage, uint32_t interfaceIndex );
   bool disconnectServer(uint32_t interfaceIndex);
   bool serverIsConnected( std::ostream& errMessage, uint32_t interfaceIndex ) const;
   void attach();
   void detach();

   bool connectDevice( const std::string& deviceName );
   bool disconnectDevice( const std::string& deviceName );
   bool sendNewProperty( indigo_property* property );
   bool sendNewNumberProperty( char* deviceName, const char* propertyName, size_t numOfItems, const char** items, const double* values );
   bool sendNewSwitchProperty( char* deviceName, const char* propertyName, size_t numOfItems, const char** items, const bool* values );
   bool sendNewTextProperty( char* deviceName, const char* propertyName, size_t numOfItems, const char** items, const char** values );
   bool loadDeviceDriver( const std::string& driver );

   std::function<void( const std::string& )> newDevice = []( const std::string& ) {};
   std::function<void( const std::string& )> removeDevice = []( const std::string& ) {};
   std::function<void( indigo_property* )> newProperty = []( indigo_property* ) {};
   std::function<void( indigo_property* )> removeProperty = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newSwitch = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newNumber = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newText = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newLight = []( indigo_property* ) {};
   std::function<void( indigo_property* )> newBlob = []( indigo_property* ) {};
   std::function<void( const char*, int )> newMessage = []( const char*, int severity ) {};

private:

   typedef indigo_result ( *clientAttachCallBack_t )( indigo_client* );
   typedef indigo_result ( *definePropertyCallBack_t )( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   typedef indigo_result ( *deletePropertyCallBack_t )( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   typedef indigo_result ( *updatePropertyCallBack_t )( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   typedef indigo_result ( *getMessageCallback_t )( indigo_client* client, indigo_device* device, const char* message );

   struct indigo_client m_indigoClient = {
      "PixInsight", false, nullptr, INDIGO_OK, INDIGO_VERSION_LEGACY, nullptr,
      reinterpret_cast<clientAttachCallBack_t>( &IndigoClient::clientAttach ),
      reinterpret_cast<definePropertyCallBack_t>( &IndigoClient::defineProperty ),
      reinterpret_cast<updatePropertyCallBack_t>( &IndigoClient::updateProperty ),
      reinterpret_cast<deletePropertyCallBack_t>( &IndigoClient::deleteProperty ),
      reinterpret_cast<getMessageCallback_t>( &IndigoClient::getMessage ),
      nullptr
   };

   std::string                    m_serverHost;
   uint32_t                       m_port;
   indigo_server_entry*           m_serverEntry = nullptr;
#ifndef __PCL_WINDOWS
   std::set<indigo_driver_entry*> m_devices;
#endif

   static indigo_result clientAttach( indigo_client* client );
   static indigo_result defineProperty( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   static indigo_result deleteProperty( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   static indigo_result updateProperty( indigo_client* client, indigo_device* device, indigo_property* property, const char* message );
   static indigo_result getMessage( indigo_client* client, indigo_device* device, const char* message );
};

#endif // INDIGOCLIENT_H

// ----------------------------------------------------------------------------
// EOF IndigoClient.h - Released 2025-04-07T08:53:56Z
