//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/LanczosInterpolation.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/AutoLock.h>
#include <pcl/LanczosInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Real Lanczos LUT data requires a maximum of about 190 KiB.
 * Integer Lanczos LUT data requires a maximum of 3 MiB.
 */
static double** PCL_Lanczos_LUT[]     = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
static float*   PCL_Lanczos_Int_LUT[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

static void ValidateLanczosLUTOrder( int n )
{
   if ( n < 1 || n > 8 )
      throw Error( "Unsupported Lanczos LUT order. Must be 1 <= n <= 8." );
}

// ----------------------------------------------------------------------------

/*
 * Sinc function for x >= 0
 */
static double Sinc( double x )
{
   x *= Const<double>::pi();
   return (x > 1.0e-07) ? Sin( x )/x : 1.0;
}

/*
 * Evaluate Lanczos function of nth order at 0 <= x < n
 */
static double Lanczos( double x, int n )
{
   if ( x < 0 )
      x = -x;
   if ( x < n )
      return Sinc( x ) * Sinc( x/n );
   return 0;
}

// ----------------------------------------------------------------------------

/*
 * Thread-safe LUT initialization routines.
 */

const double** PCL_FUNC PCL_InitializeLanczosRealLUT( int n )
{
   ValidateLanczosLUTOrder( n );

   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );

      double**& LUT = PCL_Lanczos_LUT[n-1];
      if ( LUT == nullptr )
      {
         LUT = new double*[ 2*n ];
         for ( int j = -n + 1, k = 0; j <= n; ++j, ++k )
         {
            LUT[k] = new double[ __PCL_LANCZOS_LUT_REAL_RESOLUTION ];
            for ( int i = 0; i < __PCL_LANCZOS_LUT_REAL_RESOLUTION; ++i )
               LUT[k][i] = Lanczos( j - double( i )/__PCL_LANCZOS_LUT_REAL_RESOLUTION, n );
         }
      }
      return const_cast<const double**>( reinterpret_cast<double**>( LUT ) );
   }
}

const float* PCL_FUNC PCL_InitializeLanczosIntLUT( int n )
{
   ValidateLanczosLUTOrder( n );

   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );

      float*& LUT = PCL_Lanczos_Int_LUT[n-1];
      if ( LUT == nullptr )
      {
         LUT = new float[ n*__PCL_LANCZOS_LUT_INT_RESOLUTION + 1 ];
         for ( int i = 0, k = 0; i < n; ++i )
            for ( int j = 0; j < __PCL_LANCZOS_LUT_INT_RESOLUTION; ++j, ++k )
               LUT[k] = Lanczos( i + double( j )/__PCL_LANCZOS_LUT_INT_RESOLUTION, n );
         LUT[n*__PCL_LANCZOS_LUT_INT_RESOLUTION] = 0;
      }
      return reinterpret_cast<const float*>( LUT );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/LanczosInterpolation.cpp - Released 2025-04-07T08:53:32Z
