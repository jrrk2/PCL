//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/APIException.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_API_APIException_h
#define __PCL_API_APIException_h

#include <pcl/Defs.h>

#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### NOTE
 * The following exception classes may be thrown across different DSOs, which
 * requires a typeinfo lookup upon exception catching. For this reason these
 * classes, despite being for internal use exclusively, must be exported with
 * the PCL_CLASS macro.
 */

// ----------------------------------------------------------------------------

class PCL_CLASS APIError : public Error
{
public:

   APIError()
   {
      GetAPIErrorCode();
   }

   APIError( const pcl::APIError& ) = default;

   APIError( const String& s )
      : Error( s )
   {
      GetAPIErrorCode();
   }

   String FormatInfo() const override;

   String Caption() const override
   {
      return "PixInsight API Error";
   }

private:

   uint32 apiErrorCode = 0;

   void GetAPIErrorCode();
};

// ----------------------------------------------------------------------------

class PCL_CLASS APIFunctionError : public pcl::APIError
{
public:

   APIFunctionError( const pcl::APIFunctionError& x )
      : pcl::APIError( x )
   {
   }

   APIFunctionError( const String& funcId )
      : pcl::APIError( funcId + "(): API function error" )
   {
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_API_APIException_h

// ----------------------------------------------------------------------------
// EOF pcl/APIException.h - Released 2025-04-07T08:52:44Z
