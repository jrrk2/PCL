//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/OrthographicProjection.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/OrthographicProjection.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

bool OrthographicProjection::Project( DPoint& pW, const DPoint& pN ) const noexcept
{
   if ( pN.y < 0 || pN.y > 180 )
      return false;
   double r = m_r0 * Cos( Rad( pN.y ) );
   double sinTheta, cosTheta;
   SinCos( Rad( pN.x ), sinTheta, cosTheta );
   pW.x =  r*sinTheta - m_x0;
   pW.y = -r*cosTheta - m_y0;
   return true;
}

// ----------------------------------------------------------------------------

bool OrthographicProjection::Unproject( DPoint& pN, const DPoint& pW ) const noexcept
{
   double x0 = (pW.x + m_x0)/m_r0;
   double y0 = (pW.y + m_y0)/m_r0;
   double y02 = y0 * y0;
   double r2 = x0*x0 + y02;
   pN.x = (r2 != 0) ? Deg( ArcTan( x0, -y0 ) ) : 0.0;
   if ( r2 < 0 || r2 > 1 )
      return false;
   pN.y = Deg( (r2 < 0.5) ? ArcCos( Sqrt( r2 ) ) : ArcSin( Sqrt( 1 - r2 ) ) );
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/OrthographicProjection.cpp - Released 2025-04-07T08:53:32Z
