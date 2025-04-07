//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
//
// This file is part of the ChebyshevFit test utility.
//
// Copyright (c) 2016-2025 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/*
 * A command line utility to test the pcl::ChebyshevFit template class.
 *
 * Usage: chbtest (no arguments for now)
 *
 * Written by Juan Conejero, PTeam.
 * Copyright (c) 2016, Pleiades Astrophoto S.L.
 */

#include <pcl/ChebyshevFit.h>

//#include <wordexp.h>
#include <iostream>

using namespace pcl;

// ----------------------------------------------------------------------------

/*
 * The test function
 */
static double f( double x )
{
   return 2*x*x + 3*x*x*x - x*Sin( x ) + Cos( 2*x );
}

/*
 * The derivative of f(x)
 */
static double fd( double x )
{
   return x*(4 + 9*x) - x*Cos( x ) - Sin( x ) - 2*Sin( 2*x );
}

/*
 * The indefinite integral of f(x)
 */
static double fi( double x )
{
   return 1.0/12 * x*x*x*(8 + 9*x) - Sin( x ) + Cos( x )*(x + Sin( x ));
}

int main( int argc, const char* argv[] )
{
   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
      // Fitting interval.
      const double x1 = -10.0;
      const double x2 = +10.0;

      // Number of coefficients.
      const int n = 50;

      // Error bound.
      const double eps = 1.0e-08;

      // Construct Chebyshev expansions for the function, its derivative and
      // its indefinite integral.
      ScalarChebyshevFit T( f, x1, x2, n );
      ScalarChebyshevFit Td = T.Derivative();
      ScalarChebyshevFit Ti = T.Integral();

      std::cout << IsoString().Format( "n = %d  e = %.3g\n", T.Length(), T.TruncationError() );

      // Truncate the expansions to a given error bound.
      T.Truncate( eps );
      Td.Truncate( eps );
      Ti.Truncate( eps );
      std::cout << IsoString().Format( "Tn  = %d  Te  = %.3g\n", T.TruncatedLength(), T.TruncationError() );
      std::cout << IsoString().Format( "Tdn = %d  Tde = %.3g\n", Td.TruncatedLength(), Td.TruncationError() );
      std::cout << IsoString().Format( "Tin = %d  Tie = %.3g\n", Ti.TruncatedLength(), Ti.TruncationError() );

      // Set the arbitrary constant of integration to vanish the indefinite
      // integral at x = x1.
      double C = fi( x1 );
      std::cout << IsoString().Format( "C = %+.15e\n", C );

      // Tabulate values for the function, its derivative and integral, with
      // approximation errors.
      for ( int i = 0, numTests = 20; i <= numTests; ++i )
      {
         // Evaluation point.
         double x   = x1 + i*(x2 - x1)/numTests;

         // Function and its analytic derivative and integral.
         double y   = f( x );
         double yd  = fd( x );
         double yi  = fi( x ) - C;

         // Approximated function, derivative and integral.
         double yT  = T.Evaluate( x );
         double yTd = Td.Evaluate( x );
         double yTi = Ti.Evaluate( x );

//          1         2         3
// 1234567890123456789012345678901234
//      +7.000 +1.122537831027176e+03
         std::cout << IsoString().Format( "%+11.3f %+.15e %+.15e %+.15e\n"
                                                "%+34.15e %+.15e %+.15e\n"
                                                 "%22.3e %22.3e %22.3e\n",
                                          x, y, yd, yi,
                                          yT, yTd, yTi,
                                          Abs( yT - y ), Abs( yTd - yd ), Abs( yTi - yi ) );
      }

      return 0;
   }
   ERROR_HANDLER
   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/chbtest.cpp - Released 2025-04-07T09:17:23Z
