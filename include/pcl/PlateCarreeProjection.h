//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PlateCarreeProjection.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_PlateCarreeProjection_h
#define __PCL_PlateCarreeProjection_h

/// \file pcl/PlateCarreeProjection.h

#include <pcl/Defs.h>

#include <pcl/ProjectionBase.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PlateCarreeProjection
 * \brief Plate-Carree projection system
 *
 * \ingroup astrometry_support
 */
class PCL_CLASS PlateCarreeProjection: public ProjectionBase
{
public:

   /*!
    * Default constructor.
    */
   PlateCarreeProjection() = default;

   /*!
    * Copy constructor.
    */
   PlateCarreeProjection( const PlateCarreeProjection& ) = default;

   /*!
    * Returns a dynamically allocated duplicate of this object.
    */
   ProjectionBase* Clone() const override
   {
      return new PlateCarreeProjection( *this );
   }

   /*!
    * Returns the WCS projection identifier for this projection system.
    */
   IsoString ProjCode() const override
   {
      return "CAR";
   }

   /*!
    * Returns the XISF serializable projection identifier.
    */
   IsoString Identifier() const override
   {
      return "PlateCarree";
   };

   /*!
    * Returns the readable name of this projection system.
    */
   IsoString Name() const override
   {
      return "Plate-Carree";
   }

protected:

   bool Project( DPoint& pW, const DPoint& pN ) const noexcept override
   {
      pW = pN;
      return true;
   }

   bool Unproject( DPoint& pN, const DPoint& pW ) const noexcept override
   {
      pN = pW;
      return true;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PlateCarreeProjection_h

// ----------------------------------------------------------------------------
// EOF pcl/PlateCarreeProjection.h - Released 2025-04-07T08:52:44Z
