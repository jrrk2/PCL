//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/TextAlign.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_TextAlign_h
#define __PCL_TextAlign_h

/// \file pcl/TextAlign.h

#include <pcl/Defs.h>

#include <pcl/Flags.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::TextAlign
 * \brief     Text alignment modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>TextAlign::Unknown</td>    <td>Unknown or unsupported alignment</td></tr>
 * <tr><td>TextAlign::Left</td>       <td>Left alignment</td></tr>
 * <tr><td>TextAlign::Right</td>      <td>Right alignment</td></tr>
 * <tr><td>TextAlign::HorzCenter</td> <td>Centered horizontally</td></tr>
 * <tr><td>TextAlign::Justify</td>    <td>Left and right justified</td></tr>
 * <tr><td>TextAlign::Top</td>        <td>Top alignment</td></tr>
 * <tr><td>TextAlign::Bottom</td>     <td>Bottom alignment</td></tr>
 * <tr><td>TextAlign::VertCenter</td> <td>Centered vertically</td></tr>
 * <tr><td>TextAlign::Center</td>     <td>Centered horizontally and vertically. Equal to HorzCenter|VertCenter.</td></tr>
 * <tr><td>TextAlign::Default</td>    <td>Default alignment. Equal to Left|Top.</td></tr>
 * </table>
 */
namespace TextAlign
{
   enum mask_type
   {
      Unknown     = 0x00,
      Left        = 0x01,
      Right       = 0x02,
      HorzCenter  = 0x04,
      Justify     = 0x08,
      Top         = 0x20,
      Bottom      = 0x40,
      VertCenter  = 0x80,
      Center      = HorzCenter|VertCenter,
      Default     = Left|Top
   };
}

/*!
 * A collection of text alignment mode flags.
 */
using TextAlignmentFlags = Flags<TextAlign::mask_type>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_TextAlign_h

// ----------------------------------------------------------------------------
// EOF pcl/TextAlign.h - Released 2025-04-07T08:52:44Z
