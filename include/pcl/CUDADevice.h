//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/CUDADevice.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_CUDADevice_h
#define __PCL_CUDADevice_h

/// \file pcl/CUDADevice.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class CUDADevice
 * \brief Access to core CUDA device services
 *
 * ### TODO: Write description.
 */
class PCL_CLASS CUDADevice
{
public:

   /*!
    * Default constructor - deleted, not an instantiable class.
    */
   CUDADevice() = delete;

   /*!
    * Copy constructor - deleted, not an instantiable class.
    */
   CUDADevice( const CUDADevice& ) = delete;

   /*!
    * Copy assignment operator - deleted, not an instantiable class.
    */
   CUDADevice& operator =( CUDADevice& ) = delete;

   /*!
    * Returns true iff a valid and operational CUDA device is currently
    * available on the running PixInsight platform.
    */
   static bool IsAvailable() noexcept;

   /*!
    * Returns the identifying name of the active CUDA device, or an empty
    * string if there is no valid CUDA device available on the running
    * PixInsight platform.
    */
   static IsoString Name();

   /*!
    * Returns the total global memory available on the active CUDA device in
    * bytes, or zero if no valid CUDA device is available.
    */
   static size_type TotalGlobalMemory() noexcept;

   /*!
    * Returns the total shared memory available per block on the active CUDA
    * device in bytes, or zero if no valid CUDA device is available.
    */
   static size_type SharedMemoryPerBlock() noexcept;

   /*!
    * Returns the maximum number of threads per block available in the active
    * CUDA device, or zero if no valid CUDA device is available.
    */
   static int MaxThreadsPerBlock() noexcept;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_CUDADevice_h

// ----------------------------------------------------------------------------
// EOF pcl/CUDADevice.h - Released 2025-04-07T08:52:44Z
