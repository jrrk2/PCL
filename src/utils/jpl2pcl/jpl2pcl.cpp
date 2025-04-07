//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
//
// This file is part of the jpl2pcl ephemeris generation and testing utility.
//
// Copyright (c) 2017-2025 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/*
 * A command-line utility for conversion of JPL DE/LE ephemerides files in
 * ASCII format to PixInsight/PCL ephemeris data files in XEPH format.
 *
 * Written by Juan Conejero, PTeam.
 * Copyright (c) 2016-2021, Pleiades Astrophoto S.L.
 */

#include <pcl/EphemerisFile.h>
#include <pcl/JPLEphemeris.h>
#include <pcl/Random.h>

#include <iostream>
#include <wordexp.h>

using namespace pcl;

// ----------------------------------------------------------------------------

#define PROGRAM_NAME    "jpl2pcl"
#define PROGRAM_VERSION "1.35"
#define PROGRAM_YEAR    "2021"

#define TESTS_PER_DAY   4

#define SEPARATOR       IsoString( '-', 80 )

// ----------------------------------------------------------------------------

IsoString ObjectIdFromJPLEphemerisItem( int item )
{
   switch ( item )
   {
   case JPLEphemerisItem::Mercury:             return "Me";
   case JPLEphemerisItem::Venus:               return "Ve";
   case JPLEphemerisItem::EarthMoonBarycenter: return "EMB";
   case JPLEphemerisItem::Mars:                return "Ma";
   case JPLEphemerisItem::Jupiter:             return "Ju";
   case JPLEphemerisItem::Saturn:              return "Sa";
   case JPLEphemerisItem::Uranus:              return "Ur";
   case JPLEphemerisItem::Neptune:             return "Ne";
   case JPLEphemerisItem::Pluto:               return "Pl";
   case JPLEphemerisItem::Moon:                return "Mn";
   case JPLEphemerisItem::Sun:                 return "Sn";
   case JPLEphemerisItem::Nutations:           return "Nut";
   case JPLEphemerisItem::LunarLibration:      return "Lbr";
   case JPLEphemerisItem::Earth:               return "Ea";
   case JPLEphemerisItem::SSBMoon:             return "Mn";
   default:                                    return IsoString();
   }
}

// ----------------------------------------------------------------------------

IsoString OriginIdFromJPLEphemerisItem( int item )
{
   switch ( item )
   {
      case JPLEphemerisItem::Sun:
      case JPLEphemerisItem::Mercury:
      case JPLEphemerisItem::Venus:
      case JPLEphemerisItem::Earth:
      case JPLEphemerisItem::EarthMoonBarycenter:
      case JPLEphemerisItem::Mars:
      case JPLEphemerisItem::Jupiter:
      case JPLEphemerisItem::Saturn:
      case JPLEphemerisItem::Uranus:
      case JPLEphemerisItem::Neptune:
      case JPLEphemerisItem::Pluto:
      case JPLEphemerisItem::SSBMoon:
         return "SSB";

      case JPLEphemerisItem::Moon:
      case JPLEphemerisItem::Nutations:
         return "Ea";

      case JPLEphemerisItem::LunarLibration:
         return "Mn";

      default:
         return IsoString();
   }
}

// ----------------------------------------------------------------------------

IsoString ObjectNameFromJPLEphemerisItem( int item )
{
   switch ( item )
   {
      case JPLEphemerisItem::Mercury:             return "Mercury";
      case JPLEphemerisItem::Venus:               return "Venus";
      case JPLEphemerisItem::EarthMoonBarycenter: return "Earth-Moon barycenter";
      case JPLEphemerisItem::Mars:                return "Mars";
      case JPLEphemerisItem::Jupiter:             return "Jupiter";
      case JPLEphemerisItem::Saturn:              return "Saturn";
      case JPLEphemerisItem::Uranus:              return "Uranus";
      case JPLEphemerisItem::Neptune:             return "Neptune";
      case JPLEphemerisItem::Pluto:               return "Pluto";
      case JPLEphemerisItem::Moon:                return "Moon";
      case JPLEphemerisItem::Sun:                 return "Sun";
      case JPLEphemerisItem::Nutations:           return "Nutations";
      case JPLEphemerisItem::LunarLibration:      return "Lunar librations";
      case JPLEphemerisItem::Earth:               return "Earth";
      case JPLEphemerisItem::SSBMoon:             return "Moon (SSB)";
      default:                                    return IsoString();
   }
}

// ----------------------------------------------------------------------------

int InitialExpansionSpanForJPLEphemerisItem( int item )
{
   switch ( item )
   {
   case JPLEphemerisItem::Mercury:             return   35;
   case JPLEphemerisItem::Venus:               return  140;
   case JPLEphemerisItem::EarthMoonBarycenter: return  100;
   case JPLEphemerisItem::Mars:                return  250;
   case JPLEphemerisItem::Jupiter:             return  700;
   case JPLEphemerisItem::Saturn:              return 1000;
   case JPLEphemerisItem::Uranus:              return 3400;
   case JPLEphemerisItem::Neptune:             return 4500;
   case JPLEphemerisItem::Pluto:               return 5400;
   case JPLEphemerisItem::Moon:                return   20;
   case JPLEphemerisItem::Sun:                 return  150;
   case JPLEphemerisItem::Nutations:           return   30;
   case JPLEphemerisItem::LunarLibration:      return   30;
   case JPLEphemerisItem::Earth:               return   30;
   case JPLEphemerisItem::SSBMoon:             return   30;
   default:                                    return    0;
   }
}

// ----------------------------------------------------------------------------

double TruncationForJPLEphemerisItem( int item )
{
   switch ( item )
   {
   case JPLEphemerisItem::Moon:
      return 5.0e-04; // 50 cm
   case JPLEphemerisItem::SSBMoon:
      return 1.5e-03; // 1.5 m
   case JPLEphemerisItem::Nutations:
   case JPLEphemerisItem::LunarLibration:
      return Rad( 0.001/3600 ); // 1 mas;
   case JPLEphemerisItem::Mercury:
   case JPLEphemerisItem::Venus:
      return 2.7e-03; // 2.7 m
   default:
      return 1.0e-03; // 1 m
   }
}

// ----------------------------------------------------------------------------

File logFile;

void LogLn( const IsoString& text = IsoString(), bool showOnCout = true )
{
   if ( logFile.IsOpen() )
   {
      logFile.OutTextLn( text );
      logFile.Flush();
   }

   if ( showOnCout )
      std::cout << text << '\n';
}

void LogLn( const char* text, bool showOnCout = true )
{
   if ( logFile.IsOpen() )
   {
      logFile.OutTextLn( text );
      logFile.Flush();
   }

   if ( showOnCout )
      std::cout << text << '\n';
}

void LogLn( const String& text, bool showOnCout = true )
{
   IsoString text8 = text.ToUTF8();

   if ( logFile.IsOpen() )
   {
      logFile.OutTextLn( text8 );
      logFile.Flush();
   }

   if ( showOnCout )
      std::cout << text8 << '\n';
}

// ----------------------------------------------------------------------------

inline void SplitTime( int& jdi, double& jdf, int jdi1, double dt )
{
   jdi = jdi1 + TruncInt( dt );
   jdf = 0.5 + Frac( dt );
   if ( jdf >= 1 )
   {
      jdf -= 1;
      ++jdi;
   }
}

void MakeObjectData( SerializableEphemerisObjectData& object,
                     const JPLEphemeris& ephem, int startJDI, int endJDI, JPLEphemeris::item_index item, int order = 0 )
{
   if ( !ephem.IsItemAvailable( item ) )
      throw Error( String().Format( "Item %d not available in source JPL ephemerides.", int( item ) ) );

   int delta = InitialExpansionSpanForJPLEphemerisItem( item );

   // Required truncation error
   double truncation = 0.65*TruncationForJPLEphemerisItem( item ); // in km or radians
   if ( item != JPLEphemerisItem::Moon )
      if ( item != JPLEphemerisItem::LunarLibration )
         truncation /= ephem.ConstantValue( "AU" );

   LogLn( SEPARATOR );
   LogLn( "* Generating ephemeris data for " + ObjectNameFromJPLEphemerisItem( item ) );
   LogLn();

   // Maximum allowed expansion length
   const int maxLength = 120;

   int minDelta = int32_max;
   int maxDelta = 0;
   double maxError[ 3 ] = { 0, 0, 0 };
   int totalCoefficients = 0;

   for ( int jdi1 = startJDI, count = 0; jdi1 < endJDI; ++count )
   {
      int jdi2;
      int length = 25;
      double epsilon = truncation;
      ChebyshevFit T;

      for ( bool reduce = false, truncated = false;; )
      {
         jdi2 = jdi1 + delta;
         if ( jdi2 > endJDI )
         {
            jdi2 = endJDI;
            delta = jdi2 - jdi1;
         }

         T = ChebyshevFit( [ephem,jdi1,item,order]( double dt )
                           {
                              int jdi; double jdf; SplitTime( jdi, jdf, jdi1, dt );
                              Vector r( 3 );
                              if ( order == 0 )
                              {
                                 ephem.ComputeState( r, jdi, jdf, item );
                                 return r;
                              }
                              Vector r1( 3 );
                              ephem.ComputeState( r, r1, jdi, jdf, item );
                              return r1;
                           },
                           0, delta, 3, 2*length );

         T.Truncate( epsilon );
         if ( T.TruncatedLength() > length )
         {
            --delta;
            reduce = true;
         }
         else if ( !reduce && jdi2 < endJDI )
            ++delta;
         else
         {
            if ( truncated )
               LogLn( String().Format( "** Warning: Increased truncation error to %.3e", epsilon ) );
            break;
         }

         if ( delta == 0 )
         {
            // Fast movement: try with longer coefficient series.
            length += 10;
            if ( length > maxLength )
            {
               // Extremely fast movement: try a larger truncation error.
               // N.B. This should not happen under normal working conditions
               // with fundamental ephemerides.
               length = 25;
               epsilon *= 1.1;
               truncated = true;
            }

            delta = 12;
            reduce = false;
         }
      }

      if ( T.TruncationError() > epsilon )
         throw Error( String().Format( "Internal error: %d -> %d (%d) : nx=%d ny=%d nz=%d e=%.3e",
                        jdi1, jdi2, delta,
                        T.TruncatedLength( 0 ), T.TruncatedLength( 1 ), T.TruncatedLength( 2 ), T.TruncationError() ) );

      object.data[order] << SerializableEphemerisData( TimePoint( jdi1, 0.5 ), T );

      LogLn( String().Format( "%5d : %+10.1f -> %+10.1f (%5d) : %3d %3d %3d %.3e %.3e %.3e"
                              , count, jdi1+0.5, jdi2+0.5, delta
                              , T.TruncatedLength( 0 ), T.TruncatedLength( 1 ), T.TruncatedLength( 2 )
                              , T.TruncationError( 0 ), T.TruncationError( 1 ), T.TruncationError( 2 ) ) );

      for ( int i = 0; i < 3; ++i )
         if ( T.TruncationError( i ) > maxError[i] )
            maxError[i] = T.TruncationError( i );

      totalCoefficients += T.NumberOfTruncatedCoefficients();

      if ( delta < minDelta )
         if ( jdi2 < endJDI || count == 0 )
            minDelta = delta;
      if ( delta > maxDelta )
         maxDelta = delta;

      jdi1 = jdi2;
   }

   LogLn();
   LogLn( "Object ....................... " + ObjectNameFromJPLEphemerisItem( item ) );
   LogLn( "Derivative order ............. " + String( order ) );
   LogLn( "Total Chebyshev expansions ... " + String( object.data[order].Length() ) );
   LogLn( "Smallest time span ........... " + String( minDelta ) + " (days)" );
   LogLn( "Largest time span ............ " + String( maxDelta ) + " (days)" );
   LogLn( "Largest truncation errors .... " + String().Format( "%.3e  %.3e  %.3e  (%s)"
                                                         , maxError[0], maxError[1], maxError[2], order ? "au/day" : "au" ) );
   LogLn( "Total coefficients ........... " + String( totalCoefficients ) );
   LogLn();
}

// ----------------------------------------------------------------------------

SerializableEphemerisObjectData MakeObject( const JPLEphemeris& ephem,
                        int startJDI, int endJDI, JPLEphemeris::item_index item, bool withDerivative = false )
{
   SerializableEphemerisObjectData object( ObjectIdFromJPLEphemerisItem( item ),
                                           OriginIdFromJPLEphemerisItem( item ),
                                           ObjectNameFromJPLEphemerisItem( item ) );

   MakeObjectData( object, ephem, startJDI, endJDI, item, 0 );
   if ( withDerivative )
      MakeObjectData( object, ephem, startJDI, endJDI, item, 1 );

   return object;
}

// ----------------------------------------------------------------------------

void TestEphemerisFile( const String filePath, const JPLEphemeris& J, int testsPerDay )
{
   const char* objects[] = { "Me", "Ve", "EMB", "Ma", "Ju", "Sa", "Ur", "Ne", "Pl", "Mn", "Sn", "Ea", "Mn" };
   const JPLEphemeris::item_index jplItems[] = {
      JPLEphemerisItem::Mercury,
      JPLEphemerisItem::Venus,
      JPLEphemerisItem::EarthMoonBarycenter,
      JPLEphemerisItem::Mars,
      JPLEphemerisItem::Jupiter,
      JPLEphemerisItem::Saturn,
      JPLEphemerisItem::Uranus,
      JPLEphemerisItem::Neptune,
      JPLEphemerisItem::Pluto,
      JPLEphemerisItem::Moon,
      JPLEphemerisItem::Sun,
      JPLEphemerisItem::Earth,
      JPLEphemerisItem::SSBMoon };

   LogLn( SEPARATOR );
   LogLn( "* Testing ephemeris file: " );
   LogLn( filePath );
   LogLn();

   EphemerisFile E( filePath );

   TimePoint t0 = E.StartTime();
   TimePoint t1 = E.EndTime();
   double ts = t1 - t0;

   XorShift1024 R;

   for ( int i = 0; i < int( ItemsInArray( objects ) ); ++i )
   {
      LogLn( IsoString( objects[i] ) + " (" + OriginIdFromJPLEphemerisItem( jplItems[i] ) + ')' );

      EphemerisFile::Handle H( E, objects[i], OriginIdFromJPLEphemerisItem( jplItems[i] ) );

      Vector er( 0.0, 3 );
      Vector er1( 3 );
      Vector er2( 3 );
      Vector ev( 0.0, 3 );
      Vector ev1( 3 );
      Vector ev2( 3 );
      Array<TimePoint> ert( 3 );
      Array<TimePoint> evt( 3 );

      for ( int j = 0, N = testsPerDay*int( t1-t0 ); j < N; ++j )
      {
         TimePoint t( t0 + R()*ts );
         Vector r1( 3 ), r2( 3 );
         Vector v1( 3 ), v2( 3 );
         J.ComputeState( r1, v1, t, jplItems[i] );
         H.ComputeState( r2, v2, t );

         for ( int k = 0; k < 3; ++k )
         {
            double dr = Abs( r2[k] - r1[k] );
            if ( dr > er[k] )
            {
               er[k] = dr;
               er1[k] = r1[k];
               er2[k] = r2[k];
               ert[k] = t;
            }
            double dv = Abs( v2[k] - v1[k] );
            if ( dv > ev[k] )
            {
               ev[k] = dv;
               ev1[k] = v1[k];
               ev2[k] = v2[k];
               evt[k] = t;
            }
         }
      }

      LogLn( "Position errors    : " + IsoString().Format( "%.3e  %.3e  %.3e", er[0], er[1], er[2] ) );
      LogLn( IsoString().Format( "JPL: %+.12e  %+.12e  %+.12e", er1[0], er1[1], er1[2] ) );
      LogLn( IsoString().Format( "EPH: %+.12e  %+.12e  %+.12e", er2[0], er2[1], er2[2] ) );
      LogLn( ert[0].ToIsoString() + "  " + ert[1].ToIsoString() + "  " + ert[2].ToIsoString() );
      LogLn( "Velocity errors    : " + IsoString().Format( "%.3e  %.3e  %.3e", ev[0], ev[1], ev[2] ) );
      LogLn( IsoString().Format( "JPL: %+.12e  %+.12e  %+.12e", ev1[0], ev1[1], ev1[2] ) );
      LogLn( IsoString().Format( "EPH: %+.12e  %+.12e  %+.12e", ev2[0], ev2[1], ev2[2] ) );
      LogLn( evt[0].ToIsoString() + "  " + evt[1].ToIsoString() + "  " + evt[2].ToIsoString() );
      LogLn();
   }
}

// ----------------------------------------------------------------------------

void SayHello()
{
   std::cout <<
   "\nPixInsight JPL DE/LE Ephemeris to XEPH Format Conversion Utility - " PROGRAM_NAME " version " PROGRAM_VERSION
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
"\nOptional arguments:"
"\n"
"\n   -t=<file> | --test=<file>"
"\n"
"\n      <file> is the path to a JPL DE/LE ephemeris test file in ASCII format."
"\n      If this argument is specified, the JPL ephemerides will be validated"
"\n      against the specified <file> prior to generation of XEPH data."
"\n"
"\n   -o=<dir> | --output-dir=<dir>"
"\n"
"\n      <dir> is the path to a directory where all output files will be created."
"\n      If not specified, all output will be generated on the current directory."
"\n"
"\n   -t0=<jdi> | --start=<jdi>"
"\n"
"\n      The lower bound of the time span covered by the generated XEPH file."
"\n      <jdi> is the integer part of a Julian date. If this argument is not"
"\n      specified, the lower bound of the covered time span will be determined by"
"\n      the specified set of ephemeris data files."
"\n"
"\n   -t1=<jdi> | --end=<jdi>"
"\n"
"\n      The upper bound of the time span covered by the generated XEPH file."
"\n      <jdi> is the integer part of a Julian date. If this argument is not"
"\n      specified, the upper bound of the covered time span will be determined by"
"\n      the specified set of ephemeris data files."
"\n"
"\n   --without-ssb-moon"
"\n"
"\n      Do not include the Moon with respect to the solar system barycenter."
"\n      This is useful for numerical integrations; when these are of no interest,"
"\n      excluding this object can save a significant amount of disk space."
"\n"
"\n   --with-nutations"
"\n"
"\n      Include nutation angles in the generated XEPH file. Nutations are not"
"\n      included by default."
"\n"
"\n   --with-librations"
"\n"
"\n      Include lunar libration angles in the generated XEPH file. Librations are"
"\n      not included by default."
"\n"
"\n   -T | --test-only"
"\n"
"\n      Do not generate a new XEPH file. Only perform a test by comparing"
"\n      position and velocity coordinates between values calculated at random"
"\n      time points directly from JPL DE/LE ephemeris files and the XEPH file"
"\n      (which must exist, for example, after a previous execution of this"
"\n      program)."
"\n"
"\n   -N=<n> | --test-count=<n>"
"\n"
"\n      Number of random tests per day performed on average. The default value is"
"\n      4 tests per day. The valid range is [1,100]."
"\n"
"\n   --help"
"\n"
"\n      Show this help text and exit."
"\n"
"\nExample:"
"\n"
"\n   " PROGRAM_NAME " --header=/jpl/ascii/de438/header.438\\"
"\n   --data=/jpl/ascii/de438/asc* --test=/jpl/ascii/de438/testpo.438\\"
"\n   --output-dir=/jpl/xeph"
"\n"
"\n   Output file names are always generated automatically from DE/LE version"
"\n   numbers and time bounds. In the example above, the output files would be:"
"\n"
"\n   /jpl/xeph/DE438+2287184+2688976.xeph"
"\n   /jpl/xeph/DE438+2287184+2688976.log"
"\n"
"\n   This program always generates a log plain text file (.log) with information"
"\n   on working parameters, a summary of the loaded data, and exhaustive test"
"\n   results."
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
      String     outputDir = "./";
      int        startJDI = int32_min;
      int        endJDI = int32_max;
      int        testsPerDay = 4;
      bool       withSSBMoon = true;
      bool       withNutations = false;
      bool       withLibrations = false;
      bool       testOnly = false;

      StringList args;
      for ( int i = 1; i < argc; ++i )
         args.Add( String::UTF8ToUTF16( argv[i] ) );

      for ( const Argument& arg : ExtractArguments( args, ArgumentItemMode::NoItems ) )
      {
         if ( arg.IsNumeric() )
         {
            if ( arg.Id() == "t0" || arg.Id() == "-start" )
               startJDI = TruncInt( arg.NumericValue() );
            else if ( arg.Id() == "t1" || arg.Id() == "-end" )
               endJDI = TruncInt( arg.NumericValue() );
            else if ( arg.Id() == "N" || arg.Id() == "-test-count" )
               testsPerDay = Range( TruncInt( arg.NumericValue() ), 1, 100 );
            else
               throw Error( "Unknown numeric argument: " + arg.Token() );
         }
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
            else if ( arg.Id() == "o" || arg.Id() == "-output-dir" )
               outputDir = arg.StringValue();
            else
               throw Error( "Unknown string argument: " + arg.Token() );
         }
         else if ( arg.IsSwitch() )
         {
            throw Error( "Unknown switch argument: " + arg.Token() );
         }
         else if ( arg.IsLiteral() )
         {
            if ( arg.Id() == "T" || arg.Id() == "-test-only" )
               testOnly = true;
            else if ( arg.Id() == "-without-ssb-moon" )
               withSSBMoon = false;
            else if ( arg.Id() == "-with-nutations" )
               withNutations = true;
            else if ( arg.Id() == "-with-librations" )
               withLibrations = true;
            else if ( arg.Id() == "-help" )
            {
               ShowHelp();
               std::cout << '\n';
               return 0;
            }
            else
               throw Error( "Unknown argument: " + arg.Token() );
         }
      }

      if ( headerFilePath.IsEmpty() )
         throw Error( "No JPL ephemeris header file specified." );
      if ( dataFilePaths.IsEmpty() )
         throw Error( "No JPL ephemeris data files have been specified." );

      std::cout << '\n' << "* Loading JPL ephemeris header: " << headerFilePath << '\n';

      JPLEphemeris ephem( headerFilePath );

      dataFilePaths.Sort();
      for ( auto dataFilePath : dataFilePaths )
      {
         std::cout << "* Loading JPL ephemeris data: " << dataFilePath << '\n';
         ephem.AddData( dataFilePath );
      }
      std::cout << '\n';

      if ( endJDI < startJDI )
         Swap( startJDI, endJDI );
      startJDI = Max( TruncInt( ephem.StartJD() ), startJDI );
      endJDI = Min( endJDI, TruncInt( ephem.EndJD() ) );
      if ( startJDI == endJDI )
         throw Error( "Empty time interval defined." );

      if ( !outputDir.EndsWith( '/' ) )
         outputDir << '/';

      String outputFileName;
      if ( ephem.DENumber() == ephem.LENumber() )
         outputFileName.Format( "DE%d%+d%+d", ephem.DENumber(), startJDI, endJDI );
      else
         outputFileName.Format( "DE%dLE%d%+d%+d", ephem.DENumber(), ephem.LENumber(), startJDI, endJDI );

      String ephFilePath = outputDir + outputFileName + ".xeph";

      String logFilePath = outputDir + outputFileName;
      if ( testOnly )
         logFilePath += "-test";
      logFilePath += ".log";
      logFile = File::CreateFileForWriting( logFilePath );

      LogLn( "Command line: " + String().ToSpaceSeparated( args ), false/*showOnCout*/ );
      LogLn();

      LogLn( SEPARATOR );
      LogLn( "* JPL ephemeris header:" );
      LogLn( headerFilePath );
      LogLn();
      LogLn( "* JPL ephemeris data files:" );
      for ( auto dataFilePath : dataFilePaths )
         LogLn( dataFilePath );
      LogLn();
      LogLn( ephem.Summary() );
      LogLn();

      if ( !testFilePath.IsEmpty() )
      {
         LogLn( SEPARATOR );
         LogLn( "* Testing JPL ephemerides: " + testFilePath );
         if ( !ephem.Test( testFilePath ) )
         {
            LogLn( "*** Test failed!" );
            logFile.Close();
            return 1;
         }
         LogLn( "* Test passed." );
      }
      LogLn();

      if ( !testOnly )
      {
         LogLn( SEPARATOR );
         LogLn( "* Generating output ephemeris file:" );
         LogLn( ephFilePath );
         LogLn();

         SerializableEphemerisObjectDataList objects;

         // Mandatory objects
         objects << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Mercury, true/*withDerivative*/ )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Venus, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::EarthMoonBarycenter, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Mars, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Jupiter, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Saturn, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Uranus, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Neptune, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Pluto, true )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Moon )
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Sun, true )
                    // Earth is synthesized from EMB and Moon
                 << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Earth, true );

         // Optional Moon with respect to solar system barycenter - useful for numerical integrations
         if ( withSSBMoon )
            objects << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::SSBMoon, true );

         // Optional nutations
         if ( withNutations )
            if ( ephem.IsItemAvailable( JPLEphemerisItem::Nutations ) )
               objects << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::Nutations );
            else
               LogLn( "** Warning: Nutations not available in source JPL ephemerides." );

         // Optional lunar librations
         if ( withLibrations )
            if ( ephem.IsItemAvailable( JPLEphemerisItem::LunarLibration ) )
               objects << MakeObject( ephem, startJDI, endJDI, JPLEphemerisItem::LunarLibration );
            else
               LogLn( "** Warning: Librations not available in source JPL ephemerides." );

         EphemerisMetadata metadata;
         metadata.creatorApplication = String( PROGRAM_NAME ) + " utility software version " + PROGRAM_VERSION;
         metadata.title = String().Format( "JPL DE%d/LE%d Ephemerides", ephem.DENumber(), ephem.LENumber() );
//       metadata.briefDescription
         metadata.description = String().Format( "PixInsight Fundamental Solar System Ephemerides / JPL DE%d/LE%d", ephem.DENumber(), ephem.LENumber() );
         metadata.organizationName = "Pleiades Astrophoto S.L.";
         metadata.authors = "PTeam";
         metadata.copyright = String().Format( "Copyright (C) 2018-%d, Pleiades Astrophoto S.L.", TimePoint::Now().Year() );

         EphemerisConstantList constants = ephem.Constants();

         EphemerisFile::Serialize( ephFilePath,
                                   TimePoint( startJDI, 0.5 ), TimePoint( endJDI, 0.5 ),
                                   objects, metadata, constants );
      }

      TestEphemerisFile( ephFilePath, ephem, testsPerDay );

      logFile.Close();

      return 0;
   }

   ERROR_HANDLER

   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/jpl2pcl.cpp - Released 2025-04-07T09:17:52Z
