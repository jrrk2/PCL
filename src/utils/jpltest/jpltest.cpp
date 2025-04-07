//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
//
// This file is part of the JPL planetary ephemeris test utility.
//
// Copyright (c) 2016-2025 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/*
 * A command line utility to test the pcl::JPLEphemeris class.
 *
 * This program is useful to verify the integrity of JPL ephemeris calculation
 * routines implemented in PCL.
 *
 * Written by Juan Conejero, PTeam.
 * Copyright (c) 2016-2018, Pleiades Astrophoto S.L.
 */

#include <pcl/JPLEphemeris.h>

#include <wordexp.h>
#include <iostream>

using namespace pcl;

// ----------------------------------------------------------------------------

#define PROGRAM_NAME    "jpltest"
#define PROGRAM_VERSION "1.0"
#define PROGRAM_YEAR    "2018"

// ----------------------------------------------------------------------------

void SayHello()
{
   std::cout <<
"\nPixInsight JPL DE/LE Ephemeris Test Utility - " PROGRAM_NAME " version " PROGRAM_VERSION
"\nCopyright (c) 2016-" PROGRAM_YEAR " Pleiades Astrophoto S.L."
"\n";
}

// ----------------------------------------------------------------------------

void ShowHelp()
{
   std::cout <<
"\nMandatory arguments:"
"\n"
"\n   -h=<file> | --header=<file>"
"\n"
"\n      <file> is the path to a JPL DE/LE ephemeris header file in ASCII format"
"\n"
"\n   -d=<file> | --data=<file>"
"\n"
"\n      <file> is the path to a JPL DE/LE ephemeris coefficients file in ASCII"
"\n      format. Wildcards are allowed."
"\n"
"\n   -t=<file> | --test=<file>"
"\n"
"\n      <file> is the path to a JPL DE/LE ephemeris test file in ASCII format."
"\n"
"\nOptional arguments:"
"\n"
"\n   --help"
"\n"
"\n      Show this help text and exit."
"\n"
"\nExample:"
"\n"
"\n   " PROGRAM_NAME " --header=/jpl/ascii/de438/header.438\\"
"\n   --data=/jpl/ascii/de438/asc*\\
"\n   --test=/jpl/ascii/de438/testpo.438"
"\n";
}

// ----------------------------------------------------------------------------

int main( int argc, const char* argv[] )
{
   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
      SayHello();

      String     headerFilePath;
      String     testFilePath;
      StringList dataFilePaths;
      {
         StringList args;
         for ( int i = 1; i < argc; ++i )
            args.Add( String::UTF8ToUTF16( argv[i] ) );

         for ( const Argument& arg : ExtractArguments( args, ArgumentItemMode::NoItems ) )
         {
            if ( arg.IsNumeric() )
               throw Error( "Unknown numeric argument: " + arg.Token() );
            else if ( arg.IsString() )
            {
               if ( arg.Id() == "h" || arg.Id() == "-header" )
                  headerFilePath = arg.StringValue();
               else if ( arg.Id() == "t" || arg.Id() == "-test" )
                  testFilePath = arg.StringValue();
               else if ( arg.Id() == "d" || arg.Id() == "-data" )
               {
                  IsoString fileSpec = arg.StringValue().ToUTF8();
                  wordexp_t p;
                  if ( wordexp( fileSpec.c_str(), &p, 0 ) != 0 )
                     throw Error( "Unable to parse file specification: '" + arg.Token() + '\'' );
                  for ( unsigned i = 0; i < p.we_wordc; ++i )
                     dataFilePaths << String::UTF8ToUTF16( p.we_wordv[i] );
                  wordfree( &p );
               }
               else
                  throw Error( "Unknown string argument: " + arg.Token() );
            }
            else if ( arg.IsSwitch() )
               throw Error( "Unknown switch argument: " + arg.Token() );
            else if ( arg.IsLiteral() )
            {
               if ( arg.Id() == "-help" )
               {
                  ShowHelp();
                  std::cout << '\n';
                  return 0;
               }
               else
                  throw Error( "Unknown argument: " + arg.Token() );
            }
         }
      }

      if ( headerFilePath.IsEmpty() )
         throw Error( "No JPL ephemeris header file specified." );
      if ( dataFilePaths.IsEmpty() )
         throw Error( "No JPL ephemeris data files have been specified." );
      if ( testFilePath.IsEmpty() )
         throw Error( "No JPL ephemeris test file specified." );

      std::cout << '\n' << "* Loading JPL ephemeris header: " << headerFilePath << '\n';

      JPLEphemeris ephem( headerFilePath );

      dataFilePaths.Sort();
      for ( auto dataFilePath : dataFilePaths )
      {
         std::cout << "* Loading JPL ephemeris data: " << dataFilePath << '\n';
         ephem.AddData( dataFilePath );
      }
      std::cout << '\n' << ephem.Summary() << '\n' << '\n';

      if ( !ephem.Test( testFilePath ) )
      {
         std::cout << "*** Test failed!\n";
         return 1;
      }

      std::cout << "* Test passed.\n";
      return 0;
   }

   ERROR_HANDLER
   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/jpltest.cpp - Released 2025-04-07T09:18:11Z
