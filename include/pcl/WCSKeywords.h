//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/WCSKeywords.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_WCSKeywords_h
#define __PCL_WCSKeywords_h

/// \file pcl/WCSKeywords.h

#include <pcl/Defs.h>

#include <pcl/FITSHeaderKeyword.h>
#include <pcl/Optional.h>
#include <pcl/Property.h>
#include <pcl/String.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

class LinearTransformation;

/*!
 * \class WCSKeywords
 * \brief A structure for management of WCS FITS header keywords and associated
 * image properties.
 *
 * \b References
 *
 * M. R. Calabretta, E. W. Greisen (2002) <em>Representations of World
 * Coordinates in FITS</em>, Astronomy & Astrophysics, 395, 1061-1075,
 * 1077-1122.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS WCSKeywords
{
public:

   Optional<double> objctra;   //!< Right ascension coordinate of the center of the image in degrees.
   Optional<double> objctdec;  //!< Declination coordinate of the center of the image in degrees.
   IsoString        radesys;   //!< Reference system of celestial coordinates.
   Optional<double> equinox;   //!< Equinox in years for the celestial coordinate system (not applicable if radesys = ICRS | GCRS).
   Optional<double> dateobs;   //!< Observation start time as a Julian date, UTC timescale.
   Optional<double> dateend;   //!< Observation end time as a Julian date, UTC timescale.
   Optional<double> longobs;   //!< Geodetic longitude of the observation location in degrees. Reckoned positive east of the reference meridian.
   Optional<double> latobs;    //!< Geodetic latitude of the observation location in degrees. Reckoned positive north of the equator.
   Optional<double> altobs;    //!< Geodetic height of the observation location in meters.
   Optional<double> focallen;  //!< Focal length in millimeters.
   Optional<double> xpixsz;    //!< Pixel size in micrometers.
   Optional<double> crval1;    //!< WCS coordinate at the reference point, X axis.
   Optional<double> crval2;    //!< WCS coordinate at the reference point, Y axis.
   Optional<double> crpix1;    //!< Image coordinate of the reference point on the X axis.
   Optional<double> crpix2;    //!< Image coordinate of the reference point on the Y axis.
   Optional<double> cd1_1;     //!< Rotation and scaling matrix:  CDELT1 * cos(CROTA2)
   Optional<double> cd1_2;     //!< Rotation and scaling matrix: -CDELT2 * sin(CROTA2)
   Optional<double> cd2_1;     //!< Rotation and scaling matrix:  CDELT1 * sin(CROTA2)
   Optional<double> cd2_2;     //!< Rotation and scaling matrix:  CDELT2 * cos(CROTA2)
   Optional<double> cdelt1;    //!< Plate scale in degrees per pixel at the reference pixel, X axis.
   Optional<double> cdelt2;    //!< Plate scale in degrees per pixel at the reference pixel, Y axis.
   Optional<double> crota1;    //!< Rotation angle around the reference pixel in degrees. Same as CROTA2, unused.
   Optional<double> crota2;    //!< Rotation angle around the reference pixel in degrees.
   IsoString        ctype1;    //!< Coordinate type and projection, X axis.
   IsoString        ctype2;    //!< Coordinate type and projection, Y axis.
   Optional<double> pv1_1;     //!< Native longitude of the reference point in degrees.
   Optional<double> pv1_2;     //!< Native latitude of the reference point in degrees.
   Optional<double> lonpole;   //!< Native longitude of the celestial north pole in degrees.
   Optional<double> latpole;   //!< Native latitude of the celestial north pole in degrees.

   /*!
    * Default constructor. Constructs an uninitialized/undefined %WCSKeywords
    * object.
    */
   WCSKeywords() = default;

   /*!
    * Copy constructor.
    */
   WCSKeywords( const WCSKeywords& ) = default;

   /*!
    * Constructs a %WCSKeywords object by extracting data from the specified
    * arrays of image \a properties and %FITS \a keywords.
    *
    * Metadata items available as image properties take precedence over
    * equivalent %FITS keywords.
    */
   WCSKeywords( const PropertyArray& properties, const FITSKeywordArray& keywords = FITSKeywordArray() )
   {
      Read( properties, keywords );
   }

   /*!
    * Extracts WCS metadata from the specified arrays of image \a properties
    * and %FITS \a keywords.
    *
    * If some metadata items are not included in the \a properties and
    * \a keywords arrays, the previously defined items, if any, will be
    * preserved.
    *
    * Metadata items available as image properties take precedence over
    * equivalent %FITS keywords.
    */
   void Read( const PropertyArray& properties, const FITSKeywordArray& keywords = FITSKeywordArray() );

   /*!
    * Generates a dynamic array of XISF properties equivalent to the metadata
    * currently stored in this object. The returned array includes the
    * following properties when the corresponding items are defined:
    *
    * <pre>
    * Observation:Center:RA
    * Observation:Center:Dec
    * Observation:CelestialReferenceSystem
    * Observation:Equinox
    * Observation:Time:Start
    * Observation:Time:End
    * Observation:Location:Longitude
    * Observation:Location:Latitude
    * Observation:Location:Elevation
    * Instrument:Telescope:FocalLength
    * Instrument:Sensor:XPixelSize
    * Instrument:ExposureTime
    * PCL:AstrometricSolution:ProjectionSystem
    * PCL:AstrometricSolution:ReferenceCelestialCoordinates
    * PCL:AstrometricSolution:ReferenceImageCoordinates
    * PCL:AstrometricSolution:ReferenceNativeCoordinates
    * PCL:AstrometricSolution:CelestialPoleNativeCoordinates
    * PCL:AstrometricSolution:LinearTransformationMatrix
    * </pre>
    */
   PropertyArray ToProperties() const;

   /*!
    *
    */
   bool ExtractWorldTransformation( LinearTransformation& transIW, int imageHeight );

private:

   bool m_fromFITS = false;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_WCSKeywords_h

// ----------------------------------------------------------------------------
// EOF pcl/WCSKeywords.h - Released 2025-04-07T08:52:44Z
