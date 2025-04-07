//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/HammerAitoffProjection.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_HammerAitoffProjection_h
#define __PCL_HammerAitoffProjection_h

/// \file pcl/HammerAitoffProjection.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class HammerAitoffProjection
 * \brief Hammer-Aitoff projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS HammerAitoffProjection : public ProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   HammerAitoffProjection() = default;

   /*!
    * Copy constructor.
    */
   HammerAitoffProjection( const HammerAitoffProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new HammerAitoffProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "AIT";
   }

   /*!
    * Returns the XISF serializable projection identifier.
    */
   IsoString Identifier() const override
   {
      return "HammerAitoff";
   };

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Hammer-Aitoff";
   }

protected:

   bool Project( DPoint& pW, const DPoint& pN ) const noexcept override;

   bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept override;

private:

   double m_Zmin = Const<double>::_1_sqrt2();
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_HammerAitoffProjection_h

// ----------------------------------------------------------------------------
// EOF pcl/HammerAitoffProjection.h - Released 2025-04-07T08:52:44Z
