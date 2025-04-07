//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Algebra.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void PCL_FUNC InPlaceGaussJordan( Matrix& A, Matrix& B )
{
   A.EnsureUnique();
   B.EnsureUnique();
   if ( (*API->Numerical->GaussJordanInPlaceD)( A.DataPtr(), B.DataPtr(), A.Rows(), B.Columns() ) == api_false )
      throw APIFunctionError( "GaussJordanInPlaceD" );
}

void PCL_FUNC InPlaceGaussJordan( FMatrix& A, FMatrix& B )
{
   A.EnsureUnique();
   B.EnsureUnique();
   if ( (*API->Numerical->GaussJordanInPlaceF)( A.DataPtr(), B.DataPtr(), A.Rows(), B.Columns() ) == api_false )
      throw APIFunctionError( "GaussJordanInPlaceF" );
}

// ----------------------------------------------------------------------------

void PCL_FUNC InPlaceSVDImplementation( Matrix& A, Vector& W, Matrix& V )
{
   A.EnsureUnique();
   int m = A.Rows();
   int n = A.Columns();
   W = Vector( n );
   V = Matrix( n, n );
   if ( (*API->Numerical->SVDInPlaceD)( A.DataPtr(), W.DataPtr(), V.DataPtr(), m, n ) == api_false )
      throw APIFunctionError( "SVDInPlaceD" );
}

void PCL_FUNC InPlaceSVDImplementation( FMatrix& A, FVector& W, FMatrix& V )
{
   A.EnsureUnique();
   int m = A.Rows();
   int n = A.Columns();
   W = FVector( n );
   V = FMatrix( n, n );
   if ( (*API->Numerical->SVDInPlaceF)( A.DataPtr(), W.DataPtr(), V.DataPtr(), m, n ) == api_false )
      throw APIFunctionError( "SVDInPlaceF" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Algebra.cpp - Released 2025-04-07T08:53:32Z
