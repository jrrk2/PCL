//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.3
// ----------------------------------------------------------------------------
// pcl/ImageOptions.h - Released 2025-02-21T12:13:32Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (https://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef __PCL_ImageOptions_h
#define __PCL_ImageOptions_h

/// \file pcl/ImageOptions.h

#include <pcl/Defs.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ImageType
 * \brief     Standard image types
 *
 * Standard raw frame and image types supported by the current PixInsight/PCL
 * platform. The same image types are supported by the XISF specification.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ImageType::Unknown</td>                <td>Unknown or unspecified image type.</td></tr>
 * <tr><td>ImageType::Bias</td>                   <td>Bias frame.</td></tr>
 * <tr><td>ImageType::Dark</td>                   <td>Dark frame.</td></tr>
 * <tr><td>ImageType::Flat</td>                   <td>Flat field frame.</td></tr>
 * <tr><td>ImageType::Light</td>                  <td>Light or science frame.</td></tr>
 * <tr><td>ImageType::MasterBias</td>             <td>Master bias frame.</td></tr>
 * <tr><td>ImageType::MasterDark</td>             <td>Master dark frame.</td></tr>
 * <tr><td>ImageType::MasterFlat</td>             <td>Master flat field frame.</td></tr>
 * <tr><td>ImageType::MasterLight</td>            <td>Master light or science frame.</td></tr>
 * <tr><td>ImageType::DefectMap</td>              <td>Defect pixel map.</td></tr>
 * <tr><td>ImageType::RejectionMapHigh</td>       <td>High pixel rejection map.</td></tr>
 * <tr><td>ImageType::RejectionMapLow</td>        <td>Low pixel rejection map.</td></tr>
 * <tr><td>ImageType::BinaryRejectionMapHigh</td> <td>Binary high pixel rejection map.</td></tr>
 * <tr><td>ImageType::BinaryRejectionMapLow</td>  <td>Binary low pixel rejection map.</td></tr>
 * <tr><td>ImageType::SlopeMap</td>               <td>Pixel slope map.</td></tr>
 * <tr><td>ImageType::WeightMap</td>              <td>Pixel weight map.</td></tr>
 * </table>
 *
 * A master frame is the result of integrating two or more individual frames or
 * images of the same type.
 *
 * A defect map is an integer or floating point real image where nonzero pixel
 * sample values represent invalid or defective pixels. Defective pixels are
 * typically ignored or replaced with plausible statistical estimates, such as
 * robust averages of neighbor pixels.
 *
 * Pixel rejection maps are integer or floating point real images where each
 * pixel sample value is proportional to the number of rejected pixels in an
 * integration process at the corresponding pixel coordinates. Low and high
 * rejection maps correspond, respectively, to rejected pixels below and above
 * a central reference value (typically a robust location estimate such as the
 * median of a set of integrated pixels).
 *
 * A rejection map sample value equal to the lower bound of the representable
 * range corresponds to zero rejected pixels, while the upper bound indicates
 * that all integrated pixels have been rejected.
 *
 * A binary rejection map is an integer image where a pixel sample value is
 * nonzero if and only if the corresponding pixel sample of a given image has
 * been rejected in an integration process. Low and high binary rejection maps
 * must be interpreted as in the preceding paragraph for normal rejection maps.
 * Binary rejection maps should be generated as 8-bit unsigned integer images.
 *
 * Slope maps are integer or floating point real images where each pixel sample
 * value is proportional to the slope of a straight line fitted to a set of
 * integrated pixels at the corresponding pixel coordinates. A slope map value
 * equal to the lower bound of the representable range corresponds to a
 * horizontal line (or a slope of zero degrees), while the upper bound
 * represents a vertical line (infinite slope).
 *
 * Weight maps are integer or floating point real images where each pixel
 * sample value is proportional to a statistical weight assigned at the
 * corresponding pixel coordinates. Statistical weights represented by weight
 * maps are typically generated by image calibration, registration, and
 * integration processes but can be produced by any task performing per-pixel
 * evaluations or comparisons.
 */
namespace ImageType
{
   enum value_type
   {
      Unknown,
      Bias,
      Dark,
      Flat,
      Light,
      MasterBias,
      MasterDark,
      MasterFlat,
      MasterLight,
      DefectMap,
      RejectionMapHigh,
      RejectionMapLow,
      BinaryRejectionMapHigh,
      BinaryRejectionMapLow,
      SlopeMap,
      WeightMap,
      NumberOfImageTypes
   };

   /*!
    * Returns a string with the human-readable name of a frame or image type.
    *
    * \param type A supported image type, identified by its corresponding
    *             symbolic constant.
    */
   String Name( int type );

   /*!
    * Returns the master frame type corresponding to the specified \a type.
    *
    * If \a type is one of Bias, Dark, Flat or Light, this function returns
    * MasterBias, MasterDark, MasterFlat or MasterLight, respectively.
    * Otherwise the specified \a type is returned without modification.
    */
   inline value_type ToMaster( int type )
   {
      switch ( type )
      {
      case Bias:  return MasterBias;
      case Dark:  return MasterDark;
      case Flat:  return MasterFlat;
      case Light: return MasterLight;
      default:    return (value_type)type;
      }
   }
};

/*!
 * \namespace pcl::CFAType
 * \brief     Color Filter Array types.
 *
 * A CFA (Color Filter Array) is a mosaic of color filters placed in front of
 * an image sensor, such as CMOS sensors of digital cameras. This enumeration
 * defines the CFA types currently supported in PCL.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>CFAType::None</td> <td>The image uses no CFA pattern, or the CFA pattern is unsupported.</td></tr>
 * <tr><td>CFAType::BGGR, CFAType::GRBG, CFAType::GBRG, CFAType::RGGB</td> <td>RGB CFA types.</td></tr>
 * <tr><td>CFAType::CYGM</td> <td>Cyan/Yellow/Green/Magenta CFA (used by a few digital cameras).</td></tr>
 * </table>
 */
namespace CFAType
{
   enum value_type { None, BGGR, GRBG, GBRG, RGGB, CYGM, NumberOfCFATypes };

   /*!
    * Returns a string representing a type of color filter array.
    *
    * \param cfa  A supported CFA type, identified by its corresponding
    *             symbolic constant.
    */
   String Name( int cfa );
}

/*!
 * \class ImageOptions
 * \brief Format-independent, fundamental image stream options.
 */
class PCL_CLASS ImageOptions
{
public:

   uint8    bitsPerSample          :  8; //!< Number of bits per sample: 8, 16, 32 or 64. The default value is 16.
   bool     ieeefpSampleFormat     :  1; //!< Sample type: IEEE 754 floating point (true) or integers (false). The default value is false.
   bool     complexSample          :  1; //!< Sample type: Complex (true) or real (false) samples. The default value is false.
   bool     signedIntegers         :  1; //!< Integer sample values are signed (true) or unsigned (false). The default value is false.
   bool     metricResolution       :  1; //!< Resolution units: centimeter (true) or inch (false). The default value is false.
   bool     readNormalized         :  1; //!< Normalize floating-point sample values after reading. The default value is true.
   bool     embedICCProfile        :  1; //!< Embed an ICC profile (when available). The default value is true.
   bool     embedThumbnail         :  1; //!< Embed thumbnail images (when available). The default value is false.
   bool     embedProperties        :  1; //!< Embed data properties (when available). The default value is true.
   bool     embedRGBWS             :  1; //!< Embed RGB working space parameters. The default value is true.
   bool     embedDisplayFunction   :  1; //!< Embed display function (aka STF) parameters. The default value is true.
   bool     embedColorFilterArray  :  1; //!< Embed color filter array (CFA) description. The default value is true.
   bool     embedProcessingHistory :  1; //!< Embed existing processing history as an XML document. The default value is false.
   bool     embedPreviewRects      :  1; //!< Embed existing preview rectangles and their identifiers. The default value is false.
   int      __rsv__                : 11; // Reserved for future extensions; must be zero.

   double   lowerRange;    //!< Lower limit to normalize read floating-point samples. The default value is 0.
   double   upperRange;    //!< Upper limit to normalize read floating-point samples. The default value is 1.

   double   xResolution;   //!< Horizontal resolution in pixels per resolution unit (0=unknown). The default value is 72.
   double   yResolution;   //!< Vertical resolution in pixels per resolution unit (0=unknown). The default value is 72.

   int      isoSpeed;      //!< ISO speed as specified in ISO 12232 (0=unknown)
   float    exposure;      //!< Exposure in seconds (0=unknown)
   float    aperture;      //!< Focal length divided by lens diameter (F/D) (0=unknown)
   float    focalLength;   //!< Focal length in millimeters (0=unknown)
   int      imageType;     //!< Frame or image type. See the ImageType namespace for possible values (0=unknown).
   int      cfaType;       //!< Color Filter Array type. See the CFAType namespace for possible values (0=none).

   /*!
    * Constructs an %ImageOptions structure with default values.
    */
   ImageOptions()
   {
      Reset();
   }

   /*!
    * Copy constructor.
    */
   ImageOptions( const ImageOptions& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ImageOptions& operator =( const ImageOptions& ) = default;

   /*!
    * Initializes all data members to default values.
    */
   void Reset()
   {
      bitsPerSample          = 16;
      ieeefpSampleFormat     = false;
      complexSample          = false;
      signedIntegers         = false;
      metricResolution       = false;
      readNormalized         = true;
      embedICCProfile        = true;
      embedThumbnail         = false;
      embedProperties        = true;
      embedRGBWS             = true;
      embedDisplayFunction   = true;
      embedColorFilterArray  = true;
      embedProcessingHistory = false;
      embedPreviewRects      = false;
      __rsv__                = 0;
      lowerRange             = 0;
      upperRange             = 1;
      xResolution            = 72;
      yResolution            = 72;
      isoSpeed               = 0;
      exposure               = 0;
      aperture               = 0;
      focalLength            = 0;
      imageType              = ImageType::Unknown;
      cfaType                = CFAType::None;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageOptions_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageOptions.h - Released 2025-02-21T12:13:32Z
