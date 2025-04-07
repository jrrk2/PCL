//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/GnomonicProjection.cpp - Released 2025-04-07T08:53:32Z
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

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

bool GnomonicProjection::Direct( DPoint& pW, const DPoint& pRD ) const noexcept
{
   double sinRA, cosRA;
   SinCos( Rad( pRD.x ) - m_ra0, sinRA, cosRA );
   double sinDec, cosDec;
   SinCos( Rad( pRD.y ), sinDec, cosDec );
   double A = cosDec * cosRA;
   double cosD = m_sinDec0*sinDec + m_cosDec0*A;
   if ( cosD < 1.0e-5 ) // distance > 89.999 deg ?
      return false;
   double F = Const<double>::deg()/cosD;
   pW.x = F * cosDec * sinRA;
   pW.y = F * (m_cosDec0*sinDec - m_sinDec0*A);
   return true;
}

// ----------------------------------------------------------------------------

bool GnomonicProjection::Inverse( DPoint& pRD, const DPoint& pW ) const noexcept
{
   double X = Rad( pW.x );
   double Y = Rad( pW.y );
   double sinD, cosD;
   SinCos( ArcTan( Sqrt( X*X + Y*Y ) ), sinD, cosD );
   double sinB, cosB;
   SinCos( ArcTan( X, -Y ), sinB, cosB );
   pRD.x = Deg( m_ra0 + ArcTan( sinD*sinB, m_sinDec0*sinD*cosB + m_cosDec0*cosD ) );
   pRD.y = Deg( ArcSin( m_sinDec0*cosD - m_cosDec0*sinD*cosB ) );
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/GnomonicProjection.cpp - Released 2025-04-07T08:53:32Z
