//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ImageInfo.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ImageInfo_h
#define __PCL_ImageInfo_h

/// \file pcl/ImageInfo.h

#include <pcl/Defs.h>

#include <pcl/AbstractImage.h>
#include <pcl/Relational.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageInfo
 * \brief A simple structure to hold basic information about images
 */
class PCL_CLASS ImageInfo
{
public:

   /*!
    * Represents a color space. See the ColorSpace namespace for supported
    * values.
    */
   using color_space = ColorSpace::value_type;

   int         width;            //!< %Image width in pixels
   int         height;           //!< %Image height in pixels
   int         numberOfChannels; //!< Number of channels, including alpha channels
   color_space colorSpace;       //!< %Color space, compatible with ImageColor::color_space
   bool        supported;        //!< Whether we support this image

   /*!
    * Constructs an %ImageInfo object with default values.
    */
   ImageInfo()
   {
      Reset();
   }

   /*!
    * Copy constructor.
    */
   ImageInfo( const ImageInfo& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ImageInfo& operator =( const ImageInfo& ) = default;

   /*!
    * Constructs an %ImageInfo object with data obtained from the specified
    * \a image.
    *
    * The width, height and numberOfChannels data members are set to the
    * dimensions of the selected rectangle and the number of selected channels
    * in \a image, respectively.
    */
   ImageInfo( const AbstractImage& image )
   {
      width            = image.SelectedRectangle().Width();
      height           = image.SelectedRectangle().Height();
      numberOfChannels = image.NumberOfSelectedChannels();
      colorSpace       = image.ColorSpace();
      supported        = IsValid();
   }

   /*!
    * Returns the number of pixels in the image, or width*height.
    */
   size_type NumberOfPixels() const
   {
      return size_type( width )*size_type( height );
   }

   /*!
    * Returns the number of samples in the image, or
    * width*height*numberOfChannels.
    */
   size_type NumberOfSamples() const
   {
      return NumberOfPixels()*size_type( numberOfChannels );
   }

   /*!
    * Initializes this %ImageInfo structure with default values, corresponding
    * to an empty image.
    */
   void Reset()
   {
      width = height = numberOfChannels = 0;
      colorSpace = ColorSpace::Unknown;
      supported = false;
   }

   /*!
    * Returns true iff this object describes a valid nonempty image.
    */
   bool IsValid() const
   {
      return width > 0 &&
             height > 0 &&
             colorSpace != ColorSpace::Unknown &&
             numberOfChannels >= ((colorSpace == ColorSpace::Gray) ? 1 : 3);
   }

   /*!
    * Returns true iff this object is equal to another instance \a x.
    */
   bool operator ==( const ImageInfo& x ) const
   {
      return width == x.width &&
             height == x.height &&
             numberOfChannels == x.numberOfChannels &&
             colorSpace == x.colorSpace;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageInfo_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageInfo.h - Released 2025-04-07T08:52:44Z
