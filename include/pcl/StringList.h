//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/StringList.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_StringList_h
#define __PCL_StringList_h

/// \file pcl/StringList.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/KeyValue.h>
#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_lists String List Classes
 */

/*!
 * \class pcl::StringList
 * \ingroup string_lists
 * \brief Dynamic list of Unicode (UTF-16) strings.
 *
 * %StringList is a template instantiation of Array for the String class.
 */
using StringList = Array<String>;

/*!
 * \class pcl::SortedStringList
 * \ingroup string_lists
 * \brief Dynamic sorted list of Unicode (UTF-16) strings.
 *
 * %SortedStringList is a template instantiation of SortedArray for String.
 */
using SortedStringList = SortedArray<String>;

/*!
 * \class pcl::IsoStringList
 * \ingroup string_lists
 * \brief Dynamic list of 8-bit strings.
 *
 * %IsoStringList is a template instantiation of Array for IsoString.
 */
using IsoStringList = Array<IsoString>;

/*!
 * \class pcl::SortedIsoStringList
 * \ingroup string_lists
 * \brief Dynamic sorted list of 8-bit strings.
 *
 * %SortedIsoStringList is a template instantiation of SortedArray for the
 * IsoString class.
 */
using SortedIsoStringList = SortedArray<IsoString>;

/*!
 * \defgroup key_value_string_lists Key-Value String List Classes
 */

/*!
 * \class pcl::StringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic list of key-value associations implemented as Unicode (UTF-16) strings.
 */
using StringKeyValueList = Array<StringKeyValue>;

/*!
 * \class pcl::SortedStringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic sorted list of key-value associations implemented as Unicode (UTF-16) strings.
 */
using SortedStringKeyValueList = SortedArray<StringKeyValue>;

/*!
 * \class pcl::IsoStringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic list of key-value associations implemented as 8-bit strings.
 */
using IsoStringKeyValueList = Array<IsoStringKeyValue>;

/*!
 * \class pcl::SortedIsoStringKeyValueList
 * \ingroup string_lists
 * \ingroup key_value_string_lists
 * \brief Dynamic sorted list of key-value associations implemented as 8-bit strings.
 */
using SortedIsoStringKeyValueList = SortedArray<IsoStringKeyValue>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_StringList_h

// ----------------------------------------------------------------------------
// EOF pcl/StringList.h - Released 2025-04-07T08:52:44Z
