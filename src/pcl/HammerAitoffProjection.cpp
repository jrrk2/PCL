//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/HammerAitoffProjection.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/HammerAitoffProjection.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

bool HammerAitoffProjection::Project( DPoint& pW, const DPoint& pN ) const noexcept
{
   double sinTheta, cosTheta;
   SinCos( Rad( pN.y ), sinTheta, cosTheta );
   double sinPhi2, cosPhi2;
   SinCos( Rad( pN.x/2 ), sinPhi2, cosPhi2 );
   double gamma = Deg( Sqrt( 2/(1 + cosTheta*cosPhi2) ) );
   pW.x = 2 * gamma * cosTheta * sinPhi2;
   pW.y = gamma * sinTheta;
   return true;
}

// ----------------------------------------------------------------------------

bool HammerAitoffProjection::Unproject( DPoint& pN, const DPoint& pW ) const noexcept
{
   double X = Const<double>::pi() * pW.x / 720;
   double Y = Const<double>::pi() * pW.y / 360;
   double Z = Sqrt( 1 - X*X - Y*Y );
   if ( Z < m_Zmin )
      return false;
   pN.x = Deg( 2*ArcTan( 2*Z*X, 2*Z*Z - 1 ) );
   pN.y = Deg( ArcSin( Rad( pW.y*Z ) ) );
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/HammerAitoffProjection.cpp - Released 2025-04-07T08:53:32Z
