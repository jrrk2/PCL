//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/WordArray.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_WordArray_h
#define __PCL_WordArray_h

/// \file pcl/WordArray.h

#include <pcl/Defs.h>

#include <pcl/Array.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup word_arrays Word Array Classes
 */

/*!
 * \class pcl::WordArray
 * \brief Dynamic array of 16-bit unsigned integers.
 *
 * %WordArray is a template instantiation of Array for \c uint16.
 *
 * \ingroup word_arrays
 * \ingroup dynamic_arrays
 */
using WordArray = Array<uint16>;

/*!
 * \class pcl::SortedWordArray
 * \brief Dynamic sorted array of 16-bit unsigned integers.
 *
 * %SortedWordArray is a template instantiation of SortedArray for \c uint16.
 *
 * \ingroup word_arrays
 * \ingroup dynamic_arrays
 */
using SortedWordArray = SortedArray<uint16>;

/*!
 * \class pcl::ShortArray
 * \brief Dynamic array of 16-bit signed integers.
 *
 * %ShortArray is a template instantiation of Array for \c int16.
 *
 * \ingroup word_arrays
  * \ingroup dynamic_arrays
*/
using ShortArray = Array<int16>;

/*!
 * \class pcl::SortedShortArray
 * \brief Dynamic sorted array of 16-bit signed integers.
 *
 * %SortedShortArray is a template instantiation of SortedArray for \c int16.
 *
 * \ingroup word_arrays
 * \ingroup dynamic_arrays
 */
using SortedShortArray = SortedArray<int16>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WordArray_h

// ----------------------------------------------------------------------------
// EOF pcl/WordArray.h - Released 2025-04-07T08:52:44Z
