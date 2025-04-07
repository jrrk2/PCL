//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/OrthographicProjection.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_OrthographicProjection_h
#define __PCL_OrthographicProjection_h

/// \file pcl/OrthographicProjection.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class OrthographicProjection
 * \brief Orthographic projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS OrthographicProjection : public ProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   OrthographicProjection()
   {
      m_theta0 = 90;
   }

   /*!
    * Copy constructor.
    */
   OrthographicProjection( const OrthographicProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new OrthographicProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "SIN";
   }

   /*!
    * Returns the XISF serializable projection identifier.
    */
   IsoString Identifier() const override
   {
      return "Orthographic";
   };

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Orthographic";
   }

   /*!
    *
    */
   bool CheckBrokenLine( const DPoint& cp1, const DPoint& cp2 ) const noexcept override
   {
      return DistanceFast( m_sph.CelestialToNative( cp1 ), m_sph.CelestialToNative( cp2 ) ) < 150;
   }

protected:

   bool Project( DPoint& pW, const DPoint& pN ) const noexcept override;
   bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept override;

private:

   double m_r0 = Const<double>::deg();
   double m_x0 = 0;
   double m_y0 = 0;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_OrthographicProjection_h

// ----------------------------------------------------------------------------
// EOF pcl/OrthographicProjection.h - Released 2025-04-07T08:52:44Z
