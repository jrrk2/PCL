//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// B3EProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "B3EProcess.h"
#include "B3EParameters.h"
#include "B3EInstance.h"
#include "B3EInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

B3EProcess* TheB3EProcess = nullptr;

// ----------------------------------------------------------------------------

B3EProcess::B3EProcess()
{
   TheB3EProcess = this;

   new B3EInputViewId1( this );
   new B3EInputViewId2( this );
   new B3EInputCenter1( this );
   new B3EInputCenter2( this );
   new B3EOutputCenter( this );
   new B3EIntensityUnits( this );
   new B3ESyntheticImage( this );
   new B3EThermalMap( this );
   new B3EOutOfRangeMask( this );
   new B3ESyntheticImageViewId( this );
   new B3EThermalMapViewId( this );
   new B3EOutOfRangeMaskViewId( this );

   // Background Calibration 1
   new B3ESubstractBackground1( this );
   new B3EBackgroundReferenceViewId1( this );
   new B3EBackgroundLow1( this );
   new B3EBackgroundHigh1( this );
   new B3EBackgroundUseROI1( this );
   new B3EBackgroundROIX01( this );
   new B3EBackgroundROIY01( this );
   new B3EBackgroundROIX11( this );
   new B3EBackgroundROIY11( this );
   new B3EOutputBackgroundReferenceMask1( this );

   // Background Calibration 2
   new B3ESubstractBackground2( this );
   new B3EBackgroundReferenceViewId2( this );
   new B3EBackgroundLow2( this );
   new B3EBackgroundHigh2( this );
   new B3EBackgroundUseROI2( this );
   new B3EBackgroundROIX02( this );
   new B3EBackgroundROIY02( this );
   new B3EBackgroundROIX12( this );
   new B3EBackgroundROIY12( this );
   new B3EOutputBackgroundReferenceMask2( this );
}

// ----------------------------------------------------------------------------

IsoString B3EProcess::Id() const
{
   return "B3Estimator";
}

// ----------------------------------------------------------------------------

IsoString B3EProcess::Category() const
{
   return "Flux";
}

// ----------------------------------------------------------------------------

uint32 B3EProcess::Version() const
{
   return 0x100; // required
}

// ----------------------------------------------------------------------------

String B3EProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String B3EProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/B3E.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* B3EProcess::DefaultInterface() const
{
   return TheB3EInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* B3EProcess::Create() const
{
   return new B3EInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* B3EProcess::Clone( const ProcessImplementation& p ) const
{
   const B3EInstance* instPtr = dynamic_cast<const B3EInstance*>( &p );
   return (instPtr != nullptr) ? new B3EInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool B3EProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool B3EProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: B3Estimator [<arg_list>] [<view_list>]"
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int B3EProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   B3EInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         // These are standard parameters that all processes should provide.
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            throw;
         }

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface || arguments.IsEmpty() )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF B3EProcess.cpp - Released 2025-04-07T08:53:55Z
