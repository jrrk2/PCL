//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceLogger.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NetworkServiceLogger_h
#define __NetworkServiceLogger_h

#include <pcl/TimePoint.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct NetworkServiceLogger
{
   virtual ~NetworkServiceLogger() noexcept( false )
   {
   }

   virtual void Begin() = 0;
   virtual void End() = 0;
   virtual void Section( const String& ) = 0;
   virtual void Line( const String& ) = 0;
   virtual void Warning( const String& ) = 0;
   virtual void Error( const String& ) = 0;

   virtual void SetStatus( const String& )
   {
   }

   static IsoString TimestampPrefix()
   {
      return TimePoint::Now().ToIsoString( "[%Y-%M-%D %h:%m:%s] " );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceLogger_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceLogger.h - Released 2025-04-07T08:53:56Z
