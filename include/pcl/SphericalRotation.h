//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/SphericalRotation.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_SphericalRotation_h
#define __PCL_SphericalRotation_h

/// \file pcl/SphericalRotation.h

#include <pcl/Defs.h>

#include <pcl/Optional.h>
#include <pcl/Point.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SphericalRotation
 * \brief Rotation in spherical coordinates.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS SphericalRotation
{
public:

   constexpr static double Tolerance = 1.0e-05;

   /*!
    * Default constructor.
    */
   SphericalRotation() = default;

   /*!
    * Copy constructor.
    */
   SphericalRotation( const SphericalRotation& ) = default;

   /*!
    * Construction with explicit initialization.
    *
    * \param lon0    Celestial longitude of the reference point in degrees.
    *
    * \param lat0    Celestial latitude of the reference point in degrees.
    *
    * \param phi0    Native projection longitude of the reference point in
    *                degrees.
    *
    * \param theta0  Native projection latitude of the reference point in
    *                degrees.
    *
    * \param phip    Native projection longitude of the north celestial pole in
    *                degrees.
    *
    * \param thetap  Native projection latitude of the north celestial pole in
    *                degrees. This parameter is optional. If not specified, 90
    *                degrees will be assumed.
    */
   SphericalRotation( double lon0, double lat0, double phi0, double theta0, double phip,
                      const Optional<double>& thetap );

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SphericalRotation& operator =( const SphericalRotation& ) = default;

   /*!
    * Returns the celestial coordinates corresponding to the specified location
    * in projection native coordinates.
    */
   DPoint NativeToCelestial( const DPoint& np ) const;

   /*!
    * Returns the projection native coordinates corresponding to the specified
    * location in celestial coordinates.
    */
   DPoint CelestialToNative( const DPoint& cp ) const;

private:

   double m_latpole;
   double m_alphaP;
   double m_deltaP;
   double m_phiP;
   double m_cosdeltaP;
   double m_sindeltaP;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SphericalRotation_h

// ----------------------------------------------------------------------------
// EOF pcl/SphericalRotation.h - Released 2025-04-07T08:52:44Z
