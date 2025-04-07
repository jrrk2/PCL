//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Diagnostics.cpp - Released 2025-04-07T08:53:32Z
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

#include <stdlib.h> // for exit()

#include <iostream>

#include <pcl/Diagnostics.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static bool s_useConsole = false;

// ----------------------------------------------------------------------------

bool PCL_FUNC IsConsoleDiagnostics()
{
   return s_useConsole;
}

// ----------------------------------------------------------------------------

void PCL_FUNC SetConsoleDiagnostics( bool b )
{
   s_useConsole = b;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

extern "C"
void __PCLAssertFail(
   int __type, const char* __cond, const char* __file, int __line )
{
   pcl::IsoString msg;

   msg << "Diagnostics Level " << pcl::IsoString( __PCL_DIAGNOSTICS_LEVEL ) << ": ";

   const char* typeMsg;

   switch ( __type )
   {
   case __PCL_PRECONDITION :
      typeMsg = "Precondition violated"; break;
   case __PCL_CHECK :
      typeMsg = "Check failed"; break;
   default :
      typeMsg = "Assertion not satisfied"; break;
   }

   msg << typeMsg << ":\n"
       << __cond
       << "\nFile: " << __file << ", Line: " << pcl::IsoString( __line )
       << "\nProgram terminated.";

#define caption   "PCL Diagnostics Event: "

   if ( pcl::s_useConsole )
   {
      std::cerr << '\n' << caption << msg << '\n';
      exit( 1 );
   }
   else
      throw pcl::FatalError( caption + msg );

#undef caption
}

// ----------------------------------------------------------------------------
// EOF pcl/Diagnostics.cpp - Released 2025-04-07T08:53:32Z
