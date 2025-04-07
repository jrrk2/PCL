//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ByteArray.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ByteArray_h
#define __PCL_ByteArray_h

/// \file pcl/ByteArray.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/SortedArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup byte_arrays Byte Array Classes
 */

/*!
 * \class pcl::ByteArray
 * \brief Dynamic array of 8-bit unsigned integers
 *
 * %ByteArray is a template instantiation of Array for \c uint8.
 *
 * \ingroup byte_arrays
 * \ingroup dynamic_arrays
 */
using ByteArray = Array<uint8>;

/*!
 * \class pcl::SortedByteArray
 * \brief Dynamic sorted array of 8-bit unsigned integers
 *
 * %SortedByteArray is a template instantiation of SortedArray for \c uint8.
 *
 * \ingroup byte_arrays
 * \ingroup dynamic_arrays
 */
using SortedByteArray = SortedArray<uint8>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ByteArray_h

// ----------------------------------------------------------------------------
// EOF pcl/ByteArray.h - Released 2025-04-07T08:52:44Z
