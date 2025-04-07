//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/LinearFit.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/LinearFit.h>
#include <pcl/StatusMonitor.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static api_bool LinearFitCallback( void* p )
{
   try
   {
      ++(*reinterpret_cast<StatusMonitor*>( p ));
      return api_true;
   }
   catch ( ... )
   {
      return api_false;
   }
}

void LinearFit::Fit( double& a, double& b, double& adev,
                     const float* fx, const float* fy, size_type n, StatusMonitor* status )
{
   switch ( (*API->Numerical->LinearFitF)( &a, &b, &adev, fx, fy, n, status ? LinearFitCallback : nullptr, status ) )
   {
   case api_ok:
      return;
   default:
   case api_error:
      throw APIFunctionError( "LinearFitF" );
   case api_abort:
      throw ProcessAborted();
   }
}

void LinearFit::Fit( double& a, double& b, double& adev,
                     const double* fx, const double* fy, size_type n, StatusMonitor* status )
{
   switch ( (*API->Numerical->LinearFitD)( &a, &b, &adev, fx, fy, n, status ? LinearFitCallback : nullptr, status ) )
   {
   case api_ok:
      return;
   default:
   case api_error:
      throw APIFunctionError( "LinearFitD" );
   case api_abort:
      throw ProcessAborted();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/LinearFit.cpp - Released 2025-04-07T08:53:32Z
