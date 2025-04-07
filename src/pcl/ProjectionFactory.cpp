//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/ProjectionFactory.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/GnomonicProjection.h>
#include <pcl/HammerAitoffProjection.h>
#include <pcl/MercatorProjection.h>
#include <pcl/OrthographicProjection.h>
#include <pcl/PlateCarreeProjection.h>
#include <pcl/ProjectionFactory.h>
#include <pcl/ZenithalProjections.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::CreateByWCSCode( const IsoString& wcsCode, double ra, double dec )
{
   ProjectionBase* projection = nullptr;
   bool needsInitialization = true;
   switch ( wcsCode.Trimmed().CaseFolded().Hash32() )
   {
   case 0x5e1ebd44: // TAN
      projection = new GnomonicProjection( ra, dec );
      needsInitialization = false;
      break;
   case 0x1cafa6e6: // STG
      projection = new StereographicProjection();
      break;
   case 0x9f2b1ebc: // CAR
      projection = new PlateCarreeProjection();
      break;
   case 0xecd36fef: // MER
      projection = new MercatorProjection();
      break;
   case 0xaa395586: // AIT
      projection = new HammerAitoffProjection();
      break;
   case 0x97c16049: // ZEA
      projection = new ZenithalEqualAreaProjection();
      break;
   case 0x6e069940: // SIN
      projection = new OrthographicProjection();
      break;
   default:
      throw Error( "ProjectionFactory: Invalid/unsupported WCS projection code \'" + wcsCode + '\'' );
   }
   if ( needsInitialization )
      projection->InitFromRefPoint( ra, dec );
   return projection;
}

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::CreateByIdentifier( const IsoString& identifier, double ra, double dec )
{
   ProjectionBase* projection = nullptr;
   bool needsInitialization = true;
   switch ( identifier.Trimmed().Hash32() )
   {
   case 0x4418268d: // Gnomonic
      projection = new GnomonicProjection( ra, dec );
      needsInitialization = false;
      break;
   case 0xc92ad8b1: // Stereographic
      projection = new StereographicProjection();
      break;
   case 0x43a51256: // PlateCarree
      projection = new PlateCarreeProjection();
      break;
   case 0x2edafe59: // Mercator
      projection = new MercatorProjection();
      break;
   case 0x50df063f: // HammerAitoff
      projection = new HammerAitoffProjection();
      break;
   case 0xbbb316b7: // ZenithalEqualArea
      projection = new ZenithalEqualAreaProjection();
      break;
   case 0xe5ca11ab: // Orthographic
      projection = new OrthographicProjection();
      break;
   default:
      throw Error( "ProjectionFactory: Invalid/unsupported projection identifier \'" + identifier + '\'' );
   }
   if ( needsInitialization )
      projection->InitFromRefPoint( ra, dec );
   return projection;
}

// ----------------------------------------------------------------------------

ProjectionBase* ProjectionFactory::Create( const WCSKeywords& wcs )
{
   IsoString ptype1 = wcs.ctype1.Substring( 5, 3 );
   IsoString ptype2 = wcs.ctype2.Substring( 5, 3 );
   if ( ptype1 != ptype2 )
      throw Error( "ProjectionFactory::Create(): Unsupported WCS coordinates: axes with different projections." );
   return CreateByWCSCode( ptype1, wcs.crval1, wcs.crval2 );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProjectionFactory.cpp - Released 2025-04-07T08:53:32Z
