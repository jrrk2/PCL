//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/PhaseMatrices.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_PhaseMatrices_h
#define __PCL_PhaseMatrices_h

/// \file pcl/PhaseMatrices.h

#include <pcl/Defs.h>

#include <pcl/Image.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * Calculation of the phase correlation matrix (PCM).
 *
 * \param[out] R  The resulting PCM as a single-channel complex image with the
 *                same dimensions of the source images \a A and \a B.
 *
 * \param      A  A single-channel complex image with the discrete Fourier
 *                transform of the reference image.
 *
 * \param      B  A single-channel complex image with the discrete Fourier
 *                transform of the working image.
 *
 * Both source images \a A and \a B must be nonempty and have the same
 * dimensions. If the result image \a R does not have the same geometry as the
 * source images, it is reallocated as necessary.
 *
 * \note Increments the status monitoring object of the result image \a R by
 * its area in square pixels.
 *
 * \ingroup phase_matrices
 */
template <class P1, class P2> inline
void PCL_FUNC PhaseCorrelationMatrix( GenericImage<P1>& R, const GenericImage<P2>& A, const GenericImage<P2>& B )
{
   const typename GenericImage<P2>::sample::component tiny( 1.0e-20 );

   size_type N = A.NumberOfPixels();

   if ( A.IsEmpty() || B.NumberOfPixels() != N )
      throw Error( "Invalid image geometry in PhaseCorrelationMatrix()" );

   if ( R.NumberOfPixels() != N )
      R.AllocateData( A.Width(), B.Width() );

   if ( R.Status().IsInitializationEnabled() )
      R.Status().Initialize( "Phase correlation matrix", N );

   const typename GenericImage<P2>::sample* a  = *A;
   const typename GenericImage<P2>::sample* b  = *B;
         typename GenericImage<P1>::sample* r  = *R;
         typename GenericImage<P1>::sample* rN = r + N;

   for ( ; r < rN; ++r, ++a, ++b, ++R.Status() )
   {
      typename GenericImage<P2>::sample num = *a * ~*b;
      *r = P1::ToSample( num/Max( tiny, Abs( num ) ) );
   }
}

/*!
 * Calculation of the cross power spectrum matrix (CPSM).
 *
 * \param[out] R  The resulting CPSM as a single-channel complex image with the
 *                same dimensions of the source images \a A and \a B.
 *
 * \param      A  A single-channel complex image with the discrete Fourier
 *                transform of the reference image.
 *
 * \param      B  A single-channel complex image with the discrete Fourier
 *                transform of the working image.
 *
 * Both source images \a A and \a B must be nonempty and have the same
 * dimensions. If the result image \a R does not have the same geometry as the
 * source images, it is reallocated as necessary.
 *
 * \note Increments the status monitoring object of the result image \a R by
 * its area in square pixels.
 *
 * \ingroup phase_matrices
 */
template <class P1, class P2> inline
void PCL_FUNC CrossPowerSpectrumMatrix( GenericImage<P1>& R, const GenericImage<P2>& A, const GenericImage<P2>& B )
{
   const typename GenericImage<P2>::sample::component tiny( 1.0e-20 );

   size_type N = A.NumberOfPixels();

   if ( A.IsEmpty() || B.NumberOfPixels() != N )
      throw Error( "Invalid image geometry in CrossPowerSpectrumMatrix()" );

   if ( R.NumberOfPixels() != N )
      R.AllocateData( A.Width(), B.Width() );

   if ( R.Status().IsInitializationEnabled() )
      R.Status().Initialize( "Cross power spectrum matrix", N );

   const typename GenericImage<P2>::sample* a  = *A;
   const typename GenericImage<P2>::sample* b  = *B;
         typename GenericImage<P1>::sample* r  = *R;
         typename GenericImage<P1>::sample* rN = r + N;

   for ( ; r < rN; ++r, ++a, ++b, ++R.Status() )
      *r = P1::ToSample( (*b * ~*a)/Max( tiny, Abs( *a ) * Abs( *b ) ) );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PhaseMatrices_h

// ----------------------------------------------------------------------------
// EOF pcl/PhaseMatrices.h - Released 2025-04-07T08:52:44Z
