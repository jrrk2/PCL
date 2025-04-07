//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ReadWriteMutex.cpp - Released 2025-04-07T08:53:32Z
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
#include <pcl/ReadWriteMutex.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ReadWriteMutex::ReadWriteMutex()
   : UIObject( (*API->Mutex->CreateReadWriteMutex)( ModuleHandle(), this, 0/*flags*/ ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateReadWriteMutex" );
}

// ----------------------------------------------------------------------------

ReadWriteMutex& ReadWriteMutex::Null()
{
   static ReadWriteMutex* nullMutex = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullMutex == 0 )
      nullMutex = new ReadWriteMutex( reinterpret_cast<void*>( 0 ) );
   return *nullMutex;
}

// ----------------------------------------------------------------------------

void ReadWriteMutex::LockForRead()
{
   (*API->Mutex->LockForRead)( handle, api_false );
}

// ----------------------------------------------------------------------------

void ReadWriteMutex::LockForWrite()
{
   (*API->Mutex->LockForWrite)( handle, api_false );
}

// ----------------------------------------------------------------------------

bool ReadWriteMutex::TryLockForRead()
{
   return (*API->Mutex->LockForRead)( handle, api_true ) != api_false;
}

// ----------------------------------------------------------------------------

bool ReadWriteMutex::TryLockForWrite()
{
   return (*API->Mutex->LockForWrite)( handle, api_true ) != api_false;
}

// ----------------------------------------------------------------------------

void ReadWriteMutex::Unlock()
{
   (*API->Mutex->Unlock)( handle );
}

// ----------------------------------------------------------------------------

void* ReadWriteMutex::CloneHandle() const
{
   throw Error( "Cannot clone a ReadWriteMutex handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ReadWriteMutex.cpp - Released 2025-04-07T08:53:32Z
