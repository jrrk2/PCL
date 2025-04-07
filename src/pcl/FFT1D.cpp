//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/FFT1D.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/FFT1D.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int FFT1DBase::OptimizedLength( int n, fcomplex* )
{
   return int( (*API->Numerical->FFTComplexOptimizedLengthF)( n ) );
}

int FFT1DBase::OptimizedLength( int n, dcomplex* )
{
   return int( (*API->Numerical->FFTComplexOptimizedLengthD)( n ) );
}

int FFT1DBase::OptimizedLength( int n, float* )
{
   return int( (*API->Numerical->FFTRealOptimizedLengthF)( n ) );
}

int FFT1DBase::OptimizedLength( int n, double* )
{
   return int( (*API->Numerical->FFTRealOptimizedLengthD)( n ) );
}

void* FFT1DBase::Create( int n, fcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexTransformF" );
   return handle;
}

void* FFT1DBase::Create( int n, dcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexTransformD" );
   return handle;
}

void* FFT1DBase::Create( int n, float* )
{
   void* handle = (*API->Numerical->FFTCreateRealTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealTransformF" );
   return handle;
}

void* FFT1DBase::Create( int n, double* )
{
   void* handle = (*API->Numerical->FFTCreateRealTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealTransformD" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, fcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexInverseTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexInverseTransformF" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, dcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexInverseTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexInverseTransformD" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, float* )
{
   void* handle = (*API->Numerical->FFTCreateRealInverseTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealInverseTransformF" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, double* )
{
   void* handle = (*API->Numerical->FFTCreateRealInverseTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealInverseTransformD" );
   return handle;
}

void FFT1DBase::Destroy( void* handle )
{
   if ( (*API->Numerical->FFTDestroyTransform)( handle ) == api_false )
      throw APIFunctionError( "FFTDestroyTransform" );
}

void FFT1DBase::Transform( void* handle, fcomplex* y, const fcomplex* x )
{
   if ( (*API->Numerical->FFTComplexTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTComplexTransformF" );
}

void FFT1DBase::Transform( void* handle, dcomplex* y, const dcomplex* x )
{
   if ( (*API->Numerical->FFTComplexTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTComplexTransformD" );
}

void FFT1DBase::InverseTransform( void* handle, fcomplex* y, const fcomplex* x )
{
   if ( (*API->Numerical->FFTComplexInverseTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTComplexInverseTransformF" );
}

void FFT1DBase::InverseTransform( void* handle, dcomplex* y, const dcomplex* x )
{
   if ( (*API->Numerical->FFTComplexInverseTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTComplexInverseTransformD" );
}

void FFT1DBase::Transform( void* handle, fcomplex* y, const float* x )
{
   if ( (*API->Numerical->FFTRealTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealTransformF" );
}

void FFT1DBase::Transform( void* handle, dcomplex* y, const double* x )
{
   if ( (*API->Numerical->FFTRealTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealTransformD" );
}

void FFT1DBase::InverseTransform( void* handle, float* y, const fcomplex* x )
{
   if ( (*API->Numerical->FFTRealInverseTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealInverseTransformF" );
}

void FFT1DBase::InverseTransform( void* handle, double* y, const dcomplex* x )
{
   if ( (*API->Numerical->FFTRealInverseTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealInverseTransformD" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FFT1D.cpp - Released 2025-04-07T08:53:32Z
