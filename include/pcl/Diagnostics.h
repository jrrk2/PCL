//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/Diagnostics.h - Released 2025-04-07T08:52:44Z
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

#ifndef __PCL_Diagnostics_h
#define __PCL_Diagnostics_h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// Definitions and macros for implementation of diagnostics code.
// ----------------------------------------------------------------------------
// ###### THIS HEADER IS FOR INTERNAL USE EXCLUSIVELY - PLEASE IGNORE IT ######
// ----------------------------------------------------------------------------

bool PCL_FUNC IsConsoleDiagnostics();

void PCL_FUNC SetConsoleDiagnostics( bool set = true );

inline void SetGUIDiagnostics( bool set = true )
{
   SetConsoleDiagnostics( !set );
}

} // pcl

extern "C" void PCL_FUNC __PCLAssertFail( int, const char*, const char*, int );

#define __PCL_ASSERTION     0
#define __PCL_PRECONDITION  1
#define __PCL_CHECK         2

// ----------------------------------------------------------------------------
// The value of __PCL_DIAGNOSTICS_LEVEL controls generation of diagnostics
// code, as follows:
//    0 = no diagnostics code is generated (the default value)
//    1 = CHECK code only
//    2 = both CHECK and PRECONDITION code
// ----------------------------------------------------------------------------

#ifndef __PCL_DIAGNOSTICS_LEVEL
#define __PCL_DIAGNOSTICS_LEVEL 0
#endif

// ----------------------------------------------------------------------------
// Precondition diagnostics macro.
// This macro must be used when the arguments of a function are required to
// satisfy a given condition p. The default behavior, under diagnostics mode,
// is to terminate program execution if p doesn't hold.
// ----------------------------------------------------------------------------

#if     __PCL_DIAGNOSTICS_LEVEL > 1
#define PCL_PRECONDITION( __p ) \
   ((__p) ? (void)0 : \
   (void)__PCLAssertFail( __PCL_PRECONDITION, #__p, __FILE__, __LINE__ ));
#else
#define PCL_PRECONDITION( __p )
#endif

// ----------------------------------------------------------------------------
// Check diagnostics macro.
// This macro should be used at any point where execution cannot continue if a
// given condition p is not satisfied, except when a precondition applies. The
// default behavior is to terminate program execution if p doesn't hold.
// ----------------------------------------------------------------------------

#if     __PCL_DIAGNOSTICS_LEVEL > 0
#define PCL_CHECK( __p ) \
   ((__p) ? (void)0 : \
   (void)__PCLAssertFail( __PCL_CHECK, #__p, __FILE__, __LINE__ ));
#else
#define PCL_CHECK( __p )
#endif

// ----------------------------------------------------------------------------

#endif  // __PCL_Diagnostics_h

// ----------------------------------------------------------------------------
// EOF pcl/Diagnostics.h - Released 2025-04-07T08:52:44Z
