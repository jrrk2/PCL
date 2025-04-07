//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageSelections.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ImageSelections_h
#define __PCL_ImageSelections_h

/// \file pcl/ImageSelections.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Point.h>
#include <pcl/Rectangle.h>
#include <pcl/StatusMonitor.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \struct ImageSelections
 * \brief A structure used to store rectangular image selections, channel
 *        ranges, anchor points, and clipping ranges.
 *
 * %ImageSelections defines a rectangular area, a channel range, and an anchor
 * point that can be used as a selection for 2D images.
 *
 * This structure also defines a clipping range in the normalized [0,1] range.
 * Range clipping is used for calculation of statistics such as mean, median,
 * standard deviation, average and median deviations, etc.
 */
struct PCL_CLASS ImageSelections
{
   int    channel     = 0;     //!< First selected channel
   int    lastChannel = 0;     //!< Last selected channel
   Point  point       = 0;     //!< Current anchor point
   Rect   rectangle   = 0;     //!< Current rectangular selection
   double clipLow     = 0.0;   //!< Clipping range, lower bound
   double clipHigh    = 1.0;   //!< Clipping range, upper bound
   bool   clippedLow  = false; //!< Use range clipping, low pixels
   bool   clippedHigh = false; //!< Use range clipping, high pixels

   /*!
    * Constructs a default %ImageSelections instance:
    *
    * \li Only the first channel selected: channel = lastChannel = 0.
    * \li Empty rectangular selection: rectangle = Rect( 0 ).
    * \li Anchor point at the origin of image coordinates: point = Point( 0 ).
    * \li Clipping range lower bound = 0.0
    * \li Clipping range upper bound = 1.0
    * \li Range clipping disabled.
    */
   ImageSelections() = default;

   /*!
    * Copy constructor.
    */
   ImageSelections( const ImageSelections& ) = default;

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   ImageSelections& operator =( const ImageSelections& ) = default;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageSelections_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageSelections.h - Released 2025-04-07T08:52:44Z
