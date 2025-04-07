//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// pcl/JulianDay.cpp - Released 2025-04-07T08:53:32Z
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

#include <pcl/Math.h>

/*
 * Julian Date Conversion Algorithms
 *
 * Based on:
 * Meeus, Jean (1991), Astronomical Algorithms, Willmann-Bell, Inc., ch. 7.
 *
 * We have modified the original Meeus' algorithms to support negative Julian
 * dates.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

void PCL_FUNC CalendarTimeToJD( int& jdi, double& jdf, int year, int month, int day, double dayf ) noexcept
{
   if ( month <= 2 )
   {
      --year;
      month += 12;
   }

   jdi = TruncInt( Floor( 365.25*(year + 4716) ) ) + TruncInt( 30.6001*(month + 1) ) + day - 1524;
   jdf = dayf - 0.5;

   if ( jdi > 0 && jdf < 0  )
   {
      jdf += 1;
      --jdi;
   }
   else if ( jdi < 0 && jdf > 0 )
   {
      jdf -= 1;
      ++jdi;
   }

   if ( jdi > 2299160 || jdi == 2299160 && jdf >= 0.5 )
   {
      int a = TruncInt( 0.01*year );
      jdi += 2 - a + (a >> 2);
   }
}

// ----------------------------------------------------------------------------

void PCL_FUNC JDToCalendarTime( int& year, int& month, int& day, double& dayf, int jdi, double jdf ) noexcept
{
   dayf = jdf + 0.5;
   if ( dayf >= 1 )
   {
      dayf -= 1;
      ++jdi;
   }
   else if ( dayf < 0 )
   {
      dayf += 1;
      --jdi;
   }

   if ( jdi > 2299160 )
   {
      int a = TruncInt( (jdi - 1867216.25)/36524.25 );
      jdi += 1 + a - (a >> 2);
   }

   int B = jdi + 1524;
   int C = TruncInt( (B - 122.1)/365.25 );

   if ( jdi < -1401 )  // -1400.5 = -4716 feb 29
      --C;

   int D = TruncInt( Floor( 365.25*C ) );
   int E = TruncInt( (B - D)/30.6001 );

   day = B - D - TruncInt( 30.6001*E );
   month = E - ((E < 14) ? 1 : 13);
   year = C - ((month > 2) ? 4716 : 4715);
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/JulianDay.cpp - Released 2025-04-07T08:53:32Z
