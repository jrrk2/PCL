//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/MercatorProjection.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/MercatorProjection.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

bool MercatorProjection::Project( DPoint& pW, const DPoint& pN ) const noexcept
{
   double c1 = Tan( Rad( (pN.y + 90)/2 ) );
   if ( c1 <= 0 )
      return false;
   pW.x = pN.x - m_x0;
   pW.y = m_r0 * Ln( c1 ) - m_y0;
   return true;
}

// ----------------------------------------------------------------------------

bool MercatorProjection::Unproject(DPoint& pN, const DPoint& pW) const noexcept
{
   pN.x = pW.x + m_x0;
   pN.y = Deg( 2*ArcTan( Exp( (pW.y + m_y0) / m_r0 ) ) ) - 90;
   return true;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MercatorProjection.cpp - Released 2025-04-07T08:53:32Z
