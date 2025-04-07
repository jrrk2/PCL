//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ZenithalProjections.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_ZenithalProjections_h
#define __PCL_ZenithalProjections_h

/// \file pcl/ZenithalProjections.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ZenithalProjectionBase
 * \brief Base class of zenithal projection systems
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ZenithalProjectionBase : public ProjectionBase
{
protected:

   /*!
    * Default constructor.
    */
   ZenithalProjectionBase()
   {
      m_theta0 = 90;
   }

   /*!
    * Copy constructor.
    */
   ZenithalProjectionBase( const ZenithalProjectionBase& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   virtual ProjectionBase* Clone() const override = 0;

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   virtual IsoString ProjCode() const override = 0;

   /*!
    * Returns the XISF serializable projection identifier.
    */
   virtual IsoString Identifier() const override = 0;

   /*!
    * Returns the readable name of this projection system.
    */
   virtual IsoString Name() const override = 0;

protected:

   /*!
    * Transforms from world coordinates to native spherical coordinates.
    */
   bool Project( DPoint& pW, const DPoint& pN ) const noexcept override
   {
      double rTheta = GetRTheta( pN );
      double sinTheta, cosTheta;
      SinCos( Rad( pN.x ), sinTheta, cosTheta );
      pW.x =  rTheta * sinTheta;
      pW.y = -rTheta * cosTheta;
      return true;
   }

   /*!
    * Transforms from native spherical coordinates to world coordinates.
    */
   bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept override
   {
      pN.x = Deg( ArcTan( pW.x, -pW.y ) );
      pN.y = GetTheta( Sqrt( pW.x*pW.x + pW.y*pW.y ) );
      return true;
   }

   virtual double GetRTheta( const DPoint& np ) const = 0;
   virtual double GetTheta( double rTheta ) const = 0;
};

// ----------------------------------------------------------------------------

/*!
 * \class ZenithalEqualAreaProjection
 * \brief Zenithal equal-area projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS ZenithalEqualAreaProjection: public ZenithalProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   ZenithalEqualAreaProjection() = default;

   /*!
    * Copy constructor.
    */
   ZenithalEqualAreaProjection( const ZenithalEqualAreaProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new ZenithalEqualAreaProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "ZEA";
   }

   /*!
    * Returns the XISF serializable projection identifier.
    */
   IsoString Identifier() const override
   {
      return "ZenithalEqualArea";
   };

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Zenithal Equal Area";
   }

   /*!
    *
    */
   bool CheckBrokenLine( const DPoint& cp1, const DPoint& cp2 ) const noexcept override
   {
      DPoint np1 = m_sph.CelestialToNative( cp1 );
      DPoint np2 = m_sph.CelestialToNative( cp2 );
      return Min( Mod( Abs( np1.x - np2.x - 360 ), 360.0 ),
                  Mod( Abs( np1.x - np2.x + 360 ), 360.0 ) ) < 180*Sin( Rad( 45 + (np1.y + np2.y)/2/2 ) );
   }

private:

   double GetRTheta( const DPoint& np ) const override
   {
      return 2 * Deg( Sin( Rad( (90 - np.y)/2 ) ) );
   }

   double GetTheta( double rTheta ) const override
   {
      return 90 - 2*Deg( ArcSin( Rad( rTheta )/2 ) );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class StereographicProjection
 * \brief Stereographic projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS StereographicProjection: public ZenithalProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   StereographicProjection() = default;

   /*!
    * Copy constructor.
    */
   StereographicProjection( const StereographicProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new StereographicProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "STG";
   }

   /*!
    * Returns the XISF serializable projection identifier.
    */
   IsoString Identifier() const override
   {
      return "Stereographic";
   };

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Stereographic";
   }

   /*!
    *
    */
   bool CheckBrokenLine(const DPoint& cp1, const DPoint& cp2) const noexcept override
   {
      return true;
   }

private:

   double GetRTheta( const DPoint& np ) const override
   {
      return 2 * Deg( Tan( Rad( (90 - np.y)/2 ) ) );
   }

   double GetTheta( double rTheta ) const override
   {
      return 90 - 2*Deg( ArcTan( Rad( rTheta )/2 ) );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ZenithalProjections_h

// ----------------------------------------------------------------------------
// EOF pcl/ZenithalProjections.h - Released 2025-04-07T08:52:44Z
