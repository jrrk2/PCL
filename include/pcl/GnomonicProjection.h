//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/GnomonicProjection.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_GnomonicProjection_h
#define __PCL_GnomonicProjection_h

/// \file pcl/GnomonicProjection.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GnomonicProjection
 * \brief Gnomonic projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS GnomonicProjection : public ProjectionBase
{
public:

   /*!
    * Constructs a default %GnomonicProjection object with the specified
    * origin equatorial coordinates \a ra0 and \a dec0 in degrees.
    */
   GnomonicProjection( double ra0, double dec0 )
   {
      m_ra0 = Rad( ra0 );
      m_dec0 = Rad( dec0 );
      SinCos( m_dec0, m_sinDec0, m_cosDec0 );
      m_theta0 = 90;
   }

   /*!
    * Copy constructor.
    */
   GnomonicProjection( const GnomonicProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new GnomonicProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "TAN";
   }

   /*!
    * Returns the XISF serializable projection identifier.
    */
   IsoString Identifier() const override
   {
      return "Gnomonic";
   };

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Gnomonic";
   }

   /*!
    * Transforms from celestial coordinates to world coordinates.
    */
   bool Direct( DPoint& pW, const DPoint& pRD ) const noexcept override;

   /*!
    * Transforms from world coordinates to celestial coordinates.
    */
   bool Inverse( DPoint& pRD, const DPoint& pW ) const noexcept override;

   /*!
    *
    */
   bool CheckBrokenLine( const DPoint& cp1, const DPoint& cp2 ) const noexcept override
   {
      DPoint gp1, gp2;
      return Direct( gp1, cp1 ) && Direct( gp2, cp2 ) &&
            (gp1.x - gp2.x)*(gp1.x - gp2.x) + (gp1.y - gp2.y)*(gp1.y - gp2.y) < 45*45;
   }

protected:

   bool Project( DPoint& pW, const DPoint& pN ) const noexcept override
   {
      return false;
   }

   bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept override
   {
      return false;
   }

private:

   double m_sinDec0;
   double m_cosDec0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_GnomonicProjection_h

// ----------------------------------------------------------------------------
// EOF pcl/GnomonicProjection.h - Released 2025-04-07T08:52:44Z
