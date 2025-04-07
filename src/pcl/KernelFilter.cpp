//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/KernelFilter.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Algebra.h>
#include <pcl/KernelFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SeparableFilter KernelFilter::AsSeparableFilter( float tolerance ) const
{
   if ( IsEmpty() )
      return SeparableFilter();

   FSVD svd( coefficients );

   // Find the largest singular value
   int i = svd.IndexOfLargestSingularValue();

   // Scaling factor to preserve the original filter weight
   float k = Sqrt( svd.W[i] );

   // Column vector
   FVector v = svd.U.ColumnVector( i );
   v *= k;

   // Row vector
   FVector h = svd.V.ColumnVector( i );
   h *= k;

   // Product matrix
   FMatrix A = FMatrix::FromColumnVector( v ) * FMatrix::FromRowVector( h );

   // Verify separability
   const float* a = A.Begin();
   const float* b = coefficients.Begin();
   const float* c = coefficients.End();
   for ( ; b < c; ++a, ++b )
      if ( Abs( *a - *b ) > tolerance )
         return SeparableFilter();

   return SeparableFilter( h, v, filterName );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/KernelFilter.cpp - Released 2025-04-07T08:53:32Z
