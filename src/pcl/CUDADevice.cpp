//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CUDADevice.cpp - Released 2025-04-07T08:53:32Z
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

#ifdef __PCL_HAVE_CUDA
#  if defined( __PCL_COMPATIBILITY ) || !defined( __PCL_LINUX ) // ### FIXME - Only available on Linux
#    undef __PCL_HAVE_CUDA
#  endif
#endif

#ifdef __PCL_HAVE_CUDA
#  include <cuda.h>
#  include <cuda_runtime_api.h>
#endif

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/CUDADevice.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static ::cuda_device_handle s_deviceHandle = 0;
static AtomicInt s_initialized;
static Mutex s_mutex;

static void EnsureInitialized() noexcept
{
#ifdef __PCL_HAVE_CUDA

   if ( !s_initialized )
   {
      volatile AutoLock lock( s_mutex );
      if ( s_initialized.Load() == 0 )
      {
         if ( (*API->GPU->IsCUDADeviceAvailable)( ModuleHandle() ) )
            s_deviceHandle = (*API->GPU->GetCUDASelectedDevice)( ModuleHandle() );
         s_initialized.Store( 1 );
      }
   }

#endif
}

// ----------------------------------------------------------------------------

bool CUDADevice::IsAvailable() noexcept
{
   EnsureInitialized();
   return s_deviceHandle != 0;
}

// ----------------------------------------------------------------------------

IsoString CUDADevice::Name()
{
#ifdef __PCL_HAVE_CUDA

   EnsureInitialized();
   if ( s_deviceHandle == 0 )
      return IsoString();
   cudaDeviceProp properties;
   if ( (*API->GPU->GetCUDADeviceProperties)( ModuleHandle(), s_deviceHandle, &properties, sizeof( cudaDeviceProp ) ) == api_false )
      throw APIFunctionError( "GetCUDADeviceProperties" );

   return IsoString( properties.name );

#else

   return IsoString();

#endif
}

// ----------------------------------------------------------------------------

size_type CUDADevice::TotalGlobalMemory() noexcept
{
   EnsureInitialized();
   if ( s_deviceHandle == 0 )
      return 0;
   return (*API->GPU->GetCUDADeviceTotalGlobalMem)( ModuleHandle(), s_deviceHandle );
}

// ----------------------------------------------------------------------------

size_type CUDADevice::SharedMemoryPerBlock() noexcept
{
   EnsureInitialized();
   if ( s_deviceHandle == 0 )
      return 0;
   return (*API->GPU->GetCUDADeviceSharedMemoryPerBlock)( ModuleHandle(), s_deviceHandle );
}

// ----------------------------------------------------------------------------

int CUDADevice::MaxThreadsPerBlock() noexcept
{
   EnsureInitialized();
   if ( s_deviceHandle == 0 )
      return 0;
   return (*API->GPU->GetCUDADeviceMaxThreadsPerBlock)( ModuleHandle(), s_deviceHandle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CUDADevice.cpp - Released 2025-04-07T08:53:32Z
