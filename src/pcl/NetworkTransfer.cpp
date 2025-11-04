//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/NetworkTransfer.cpp - Released 2025-04-07T08:53:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include <pcl/AutoLock.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>
#include <pcl/NetworkTransfer.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<NetworkTransfer*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class NetworkTransferEventDispatcher
{
public:

   static api_bool DownloadDataAvailable( network_transfer_handle hSender, control_handle hReceiver, const void* buffer, fsize_type size )
   {
      if ( sender->onDownloadDataAvailable != nullptr )
         return (receiver->*sender->onDownloadDataAvailable)( *sender, buffer, size );
      return api_false; // should never happen
   }

   static fsize_type UploadDataRequested( network_transfer_handle hSender, control_handle hReceiver, void* buffer, fsize_type maxSize )
   {
      if ( sender->onUploadDataRequested != nullptr )
         return (receiver->*sender->onUploadDataRequested)( *sender, buffer, maxSize );
      return 0; // should never happen
   }

   static api_bool TransferProgress( network_transfer_handle hSender, control_handle hReceiver,
                                     fsize_type dlTotal, fsize_type dlCurrent, fsize_type ulTotal, fsize_type ulCurrent )
   {
      if ( sender->onTransferProgress != nullptr )
         return (receiver->*sender->onTransferProgress)( *sender, dlTotal, dlCurrent, ulTotal, ulCurrent );
      return api_false; // should never happen
   }

}; // NetworkTransferEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

NetworkTransfer::NetworkTransfer()
   : UIObject( API_NetworkTransfer_CreateNetworkTransfer( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateNetworkTransfer" );
}

// ----------------------------------------------------------------------------

NetworkTransfer& NetworkTransfer::Null()
{
   static NetworkTransfer* nullNetworkTransfer = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullNetworkTransfer == nullptr )
      nullNetworkTransfer = new NetworkTransfer( nullptr );
   return *nullNetworkTransfer;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetURL( const String& url, const String& userName, const String& userPassword )
{
   if ( API_NetworkTransfer_SetNetworkTransferURL( handle, url.c_str(), userName.c_str(), userPassword.c_str() ) == api_false )
      throw APIFunctionError( "SetNetworkTransferURL" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetProxyURL( const String& url, const String& userName, const String& userPassword )
{
   if ( API_NetworkTransfer_SetNetworkTransferProxyURL( handle, url.c_str(), userName.c_str(), userPassword.c_str() ) == api_false )
      throw APIFunctionError( "SetNetworkTransferProxyURL" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetSSL( bool useSSL, bool forceSSL, bool verifyPeer, bool verifyHost )
{
   if ( API_NetworkTransfer_SetNetworkTransferSSL( handle, useSSL, forceSSL, verifyPeer, verifyHost ) == api_false )
      throw APIFunctionError( "SetNetworkTransferSSL" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetCustomHTTPHeaders( const String& nlsHeaders )
{
   if ( API_NetworkTransfer_SetNetworkTransferCustomHTTPHeaders( handle, nlsHeaders.c_str() ) == api_false )
      throw APIFunctionError( "SetNetworkTransferCustomHTTPHeaders" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetConnectionTimeout( int seconds )
{
   if ( API_NetworkTransfer_SetNetworkTransferConnectionTimeout( handle, seconds ) == api_false )
      throw APIFunctionError( "SetNetworkTransferConnectionTimeout" );
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::Download()
{
   return API_NetworkTransfer_PerformNetworkTransferDownload( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::Upload( fsize_type uploadSize )
{
   return API_NetworkTransfer_PerformNetworkTransferUpload( handle, uploadSize ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::POST( const String& fields )
{
   return API_NetworkTransfer_PerformNetworkTransferPOST( handle, fields.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::SMTP( const String& mailFrom, const StringList& mailRecipients )
{
   String csRecipients;
   mailRecipients.ToCommaSeparated( csRecipients );
   return API_NetworkTransfer_PerformNetworkTransferSMTP( handle, mailFrom.c_str(), csRecipients.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::CloseConnection()
{
   API_NetworkTransfer_CloseNetworkTransferConnection( handle );
}

// ----------------------------------------------------------------------------

String NetworkTransfer::URL() const
{
   size_type len = 0;
   API_NetworkTransfer_GetNetworkTransferURL( handle, 0, &len );

   String url;
   if ( len > 0 )
   {
      url.SetLength( len );
      if ( API_NetworkTransfer_GetNetworkTransferURL( handle, url.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferURL" );
      url.ResizeToNullTerminated();
   }
   return url;
}

// ----------------------------------------------------------------------------

String NetworkTransfer::ProxyURL() const
{
   size_type len = 0;
   API_NetworkTransfer_GetNetworkTransferProxyURL( handle, 0, &len );

   String url;
   if ( len > 0 )
   {
      url.SetLength( len );
      if ( API_NetworkTransfer_GetNetworkTransferProxyURL( handle, url.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferProxyURL" );
      url.ResizeToNullTerminated();
   }
   return url;
}

// ----------------------------------------------------------------------------

String NetworkTransfer::CustomHTTPHeaders() const
{
   size_type len = 0;
   API_NetworkTransfer_GetNetworkTransferCustomHTTPHeaders( handle, 0, &len );

   String nlsHeaders;
   if ( len > 0 )
   {
      nlsHeaders.SetLength( len );
      if ( API_NetworkTransfer_GetNetworkTransferCustomHTTPHeaders( handle, nlsHeaders.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferCustomHTTPHeaders" );
      nlsHeaders.ResizeToNullTerminated();
   }
   return nlsHeaders;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::PerformedOK() const
{
   return API_NetworkTransfer_GetNetworkTransferStatus( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::WasAborted() const
{
   return API_NetworkTransfer_GetNetworkTransferIsAborted( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int NetworkTransfer::ResponseCode() const
{
   return API_NetworkTransfer_GetNetworkTransferResponseCode( handle );
}

// ----------------------------------------------------------------------------

String NetworkTransfer::ContentType() const
{
   size_type len = 0;
   API_NetworkTransfer_GetNetworkTransferContentType( handle, 0, &len );

   String contentType;
   if ( len > 0 )
   {
      contentType.SetLength( len );
      if ( API_NetworkTransfer_GetNetworkTransferContentType( handle, contentType.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferContentType" );
      contentType.ResizeToNullTerminated();
   }
   return contentType;
}

// ----------------------------------------------------------------------------

fsize_type NetworkTransfer::BytesTransferred() const
{
   return API_NetworkTransfer_GetNetworkTransferBytesTransferred( handle );
}

// ----------------------------------------------------------------------------

double NetworkTransfer::TotalSpeed() const
{
   double KiBPerSec = 0;
   API_NetworkTransfer_GetNetworkTransferTotalSpeed( handle, &KiBPerSec );
   return KiBPerSec;
}

// ----------------------------------------------------------------------------

double NetworkTransfer::TotalTime() const
{
   double totalSecs = 0;
   API_NetworkTransfer_GetNetworkTransferTotalTime( handle, &totalSecs );
   return totalSecs;
}

// ----------------------------------------------------------------------------

String NetworkTransfer::ErrorInformation() const
{
   size_type len = 0;
   API_NetworkTransfer_GetNetworkTransferErrorInformation( handle, 0, &len );

   String errorInfo;
   if ( len > 0 )
   {
      errorInfo.SetLength( len );
      if ( API_NetworkTransfer_GetNetworkTransferErrorInformation( handle, errorInfo.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferErrorInformation" );
      errorInfo.ResizeToNullTerminated();
   }
   return errorInfo;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::OnDownloadDataAvailable( download_event_handler handler, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLERS;
   onDownloadDataAvailable = nullptr;
   if ( API_NetworkTransfer_SetNetworkTransferDownloadEventRoutine( handle, &receiver,
                     (handler != nullptr) ? NetworkTransferEventDispatcher::DownloadDataAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetNetworkTransferDownloadEventRoutine" );
   onDownloadDataAvailable = handler;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::OnUploadDataRequested( upload_event_handler handler, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLERS;
   onUploadDataRequested = nullptr;
   if ( API_NetworkTransfer_SetNetworkTransferUploadEventRoutine( handle, &receiver,
                     (handler != nullptr) ? NetworkTransferEventDispatcher::UploadDataRequested : nullptr ) == api_false )
      throw APIFunctionError( "SetNetworkTransferUploadEventRoutine" );
   onUploadDataRequested = handler;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::OnTransferProgress( progress_event_handler handler, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLERS;
   onTransferProgress = nullptr;
   if ( API_NetworkTransfer_SetNetworkTransferProgressEventRoutine( handle, &receiver,
                     (handler != nullptr) ? NetworkTransferEventDispatcher::TransferProgress : nullptr ) == api_false )
      throw APIFunctionError( "SetNetworkTransferProgressEventRoutine" );
   onTransferProgress = handler;
}

// ----------------------------------------------------------------------------

void* NetworkTransfer::CloneHandle() const
{
   throw Error( "Cannot clone a NetworkTransfer handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/NetworkTransfer.cpp - Released 2025-04-07T08:53:32Z
