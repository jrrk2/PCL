//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ProjectionFactory.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ProjectionFactory_h
#define __PCL_ProjectionFactory_h

/// \file pcl/ProjectionFactory.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ProjectionFactory
 * \brief Selectable projection system
 *
 * %ProjectionFactory implements a projection system that can be selected upon
 * construction among all of the projections implemented in the PixInsight/PCL
 * platform.
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ProjectionFactory
{
public:

   /*!
    * Creates a new projection for the specified WCS projection \a code and
    * origin equatorial coordinates \a ra and \a dec in degrees.
    *
    * The following WCS projection codes are supported in current PCL versions:
    *
    * TAN\n
    * STG\n
    * CAR\n
    * MER\n
    * AIT\n
    * ZEA\n
    * SIN
    *
    * The specified WCS projection \a code is case insensitive and can contain
    * leading and trailing spaces.
    */
   static ProjectionBase* CreateByWCSCode( const IsoString& code, double ra, double dec );

   static ProjectionBase* CreateByWCSCode( const IsoString::ustring_base& code, double ra, double dec )
   {
      return CreateByWCSCode( IsoString( code ), ra, dec );
   }

   /*!
    * Creates a new projection for the specified projection \a identifier and
    * origin equatorial coordinates \a ra and \a dec in degrees.
    *
    * The following projection identifiers are supported in current PCL versions:
    *
    * Gnomonic\n
    * Stereographic\n
    * PlateCarree\n
    * Mercator\n
    * HammerAitoff\n
    * ZenithalEqualArea\n
    * Orthographic
    *
    * The specified projection \a identifier is case sensitive and can contain
    * leading and trailing spaces.
    */
   static ProjectionBase* CreateByIdentifier( const IsoString& identifier, double ra, double dec );

   static ProjectionBase* CreateByIdentifier( const IsoString::ustring_base& identifier, double ra, double dec )
   {
      return CreateByIdentifier( IsoString( identifier ), ra, dec );
   }

   /*!
    * Creates a new projection with data acquired from the specified WCS
    * keywords.
    */
   static ProjectionBase* Create( const WCSKeywords& wcs );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ProjectionFactory_h

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionFactory.h - Released 2025-04-07T08:52:44Z
