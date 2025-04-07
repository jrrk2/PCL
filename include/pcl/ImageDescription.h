//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageDescription.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ImageDescription_h
#define __PCL_ImageDescription_h

/// \file pcl/ImageDescription.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/ImageInfo.h>
#include <pcl/ImageOptions.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageDescription
 * \brief A structure to transport basic information and options for an image
 *        stored in an image file
 *
 * This structure is used (instantiated in ImageDescriptionArray containers) by
 * the FileFormatImplementation and FileFormaInstance classes to describe
 * individual images stored in image files.
 *
 * \sa ImageInfo, ImageOptions, FileFormatImplementation, FileFormatInstance
 */
struct PCL_CLASS ImageDescription
{
   ImageInfo    info;    //!< Basic image information: dimensions and color space.
   ImageOptions options; //!< Format-independent image file options.
   IsoString    id;      //!< Identifier of this image. Usually a running counter starting from zero.

   /*!
    * Default constructor.
    */
   ImageDescription() = default;

   /*!
    * Constructs an %ImageDescription object from the specified ImageInfo,
    * ImageOptions and image identifier.
    */
   ImageDescription( const ImageInfo& i, const ImageOptions& o, const IsoString& s = IsoString() )
      : info( i ), options( o ), id( s )
   {
   }

   ImageDescription( const ImageInfo& i, const ImageOptions& o, const String& s )
      : ImageDescription( i, o, IsoString( s ) )
   {
   }

   /*!
    * Constructs an %ImageDescription object from the specified ImageInfo
    * structure with default format-independent options and an empty image
    * identifier.
    */
   ImageDescription( const ImageInfo& i )
      : info( i )
   {
   }

   /*!
    * Copy constructor.
    */
   ImageDescription( const ImageDescription& ) = default;

   /*!
    * Cop assignment operator. Returns a reference to this object.
    */
   ImageDescription& operator =( const ImageDescription& ) = default;
};

/*!
 * \class pcl::ImageDescriptionArray
 * \brief Dynamic array of ImageDescription structures
 *
 * %ImageDescriptionArray is a template instantiation of Array for
 * ImageDescription.
 */
using ImageDescriptionArray = Array<ImageDescription>;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageDescription_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageDescription.h - Released 2025-04-07T08:52:44Z
