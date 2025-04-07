//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard EphemerisGeneration Process Module Version 1.3.0
// ----------------------------------------------------------------------------
// EphemerisGeneratorProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard EphemerisGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "EphemerisGeneratorInstance.h"
#include "EphemerisGeneratorInterface.h"
#include "EphemerisGeneratorParameters.h"
#include "EphemerisGeneratorProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/ErrorHandler.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

EphemerisGeneratorProcess* TheEphemerisGeneratorProcess = nullptr;

// ----------------------------------------------------------------------------

EphemerisGeneratorProcess::EphemerisGeneratorProcess()
{
   TheEphemerisGeneratorProcess = this;

   new EGWorkingMode( this );
   new EGPosX( this );
   new EGPosY( this );
   new EGPosZ( this );
   new EGVelX( this );
   new EGVelY( this );
   new EGVelZ( this );
   new EGElemA( this );
   new EGElemQ( this );
   new EGElemE( this );
   new EGElemM( this );
   new EGElemT( this );
   new EGElemI( this );
   new EGElemO( this );
   new EGElemW( this );
   new EGEpochJD( this );
   new EGObjectId( this );
   new EGObjectName( this );
   new EGHDefined( this );
   new EGH( this );
   new EGGDefined( this );
   new EGG( this );
   new EGM1Defined( this );
   new EGM1( this );
   new EGK1Defined( this );
   new EGK1( this );
   new EGM2Defined( this );
   new EGM2( this );
   new EGK2Defined( this );
   new EGK2( this );
   new EGPCDefined( this );
   new EGPC( this );
   new EGB_VDefined( this );
   new EGB_V( this );
   new EGU_BDefined( this );
   new EGU_B( this );
   new EGI_RDefined( this );
   new EGI_R( this );
   new EGDDefined( this );
   new EGD( this );
   new EGA1Defined( this );
   new EGA1( this );
   new EGA2Defined( this );
   new EGA2( this );
   new EGA3Defined( this );
   new EGA3( this );
   new EGDTDefined( this );
   new EGDT( this );
   new EGDatabaseFilePath( this );
   new EGDatabaseFormatName( this );
   new EGObjects( this );
   new EGUseRegularExpressions( this );
   new EGExcludeCoreAsteroids( this );
   new EGFundamentalFilePath( this );
   new EGAsteroidsFilePath( this );
   new EGKBOsFilePath( this );
   new EGStartTimeJD( this );
   new EGEndTimeJD( this );
   new EGUseAsteroidPerturbers( this );
   new EGUseKBOPerturbers( this );
   new EGVelocityExpansions( this );
   new EGSeparateEarthMoonPerturbers( this );
   new EGRelativisticPerturbations( this );
   new EGFigureEffects( this );
   new EGNonGravitationalPerturbations( this );
   new EGOutputXEPHFile( this );
   new EGOutputXEPHFilePath( this );
   new EGOutputLogFile( this );
   new EGOverwriteExistingFiles( this );
   new EGDenseOutputToleranceFactor( this );
   new EGEphemerisToleranceFactor( this );
   new EGEphemerisMaxExpansionLength( this );
   new EGEphemerisMaxTruncationError( this );
}

// ----------------------------------------------------------------------------

IsoString EphemerisGeneratorProcess::Id() const
{
   return "EphemerisGenerator";
}

// ----------------------------------------------------------------------------

IsoString EphemerisGeneratorProcess::Category() const
{
   return "Ephemerides,Global";
}

// ----------------------------------------------------------------------------

uint32 EphemerisGeneratorProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String EphemerisGeneratorProcess::Description() const
{
   return
   "<html>"
   "</html>";
}

// ----------------------------------------------------------------------------

String EphemerisGeneratorProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/EphemerisGenerator.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* EphemerisGeneratorProcess::DefaultInterface() const
{
   return TheEphemerisGeneratorInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* EphemerisGeneratorProcess::Create() const
{
   return new EphemerisGeneratorInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* EphemerisGeneratorProcess::Clone( const ProcessImplementation& p ) const
{
   const EphemerisGeneratorInstance* instance = dynamic_cast<const EphemerisGeneratorInstance*>( &p );
   return (instance != nullptr) ? new EphemerisGeneratorInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static Array<TextDatabase::FormatDescription> s_formats;
static bool                                   s_initialized = false;
static bool                                   s_valid = false;

void EphemerisGeneratorProcess::InitializeDatabaseFormats()
{
   if ( !s_initialized )
   {
      s_initialized = true;

      String odfFilePath = PixInsightSettings::GlobalString( "Application/LibraryDirectory" ) + "/default.xodf";

      try
      {
         Console console;
         console.NoteLn( "<end><cbr><br>* EphemerisGenerator: Parsing object database format descriptions: <raw>" + odfFilePath + "</raw>" );

         if ( !File::Exists( odfFilePath ) )
            throw Error( "EphemerisGenerator: The database format description file does not exist: " + odfFilePath );

         s_formats = TextDatabase::FormatDescription::Parse( odfFilePath );
         s_valid = true;
         console.WriteLn( "<end><cbr>" + String( s_formats.Length() ) + " object database format(s) acquired.<br>" );
      }
      ERROR_HANDLER
   }
}

bool EphemerisGeneratorProcess::HasValidDatabaseFormats()
{
   return s_valid;
}

const Array<TextDatabase::FormatDescription>& EphemerisGeneratorProcess::DatabaseFormats()
{
   return s_formats;
}

const TextDatabase::FormatDescription& EphemerisGeneratorProcess::DefaultDatabaseFormat()
{
   return s_formats[0];
}

int EphemerisGeneratorProcess::FindDatabaseFormatByName( const String& name )
{
   for ( size_type i = 0; i < s_formats.Length(); ++i )
      if ( s_formats[i].name.CompareIC( name ) == 0 )
         return int( i );
   return -1;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF EphemerisGeneratorProcess.cpp - Released 2025-04-07T08:53:55Z
