//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// FITSHeaderProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "FITSHeaderProcess.h"
#include "FITSHeaderParameters.h"
#include "FITSHeaderInstance.h"
#include "FITSHeaderInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FITSHeaderProcess* TheFITSHeaderProcess = nullptr;

// ----------------------------------------------------------------------------

FITSHeaderProcess::FITSHeaderProcess()
{
   TheFITSHeaderProcess = this;

   new FITSKeywordTable( this );
   new FITSKeywordName( TheFITSKeywordTableParameter );
   new FITSKeywordValue( TheFITSKeywordTableParameter );
   new FITSKeywordComment( TheFITSKeywordTableParameter );
}

// ----------------------------------------------------------------------------

IsoString FITSHeaderProcess::Id() const
{
   return "FITSHeader";
}

// ----------------------------------------------------------------------------

IsoString FITSHeaderProcess::Category() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

uint32 FITSHeaderProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String FITSHeaderProcess::Description() const
{
   return

   "<html>"
   "<p>The FITSHeader process allows you to browse and edit FITS header keywords.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String FITSHeaderProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/FITSHeader.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* FITSHeaderProcess::DefaultInterface() const
{
   return TheFITSHeaderInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* FITSHeaderProcess::Create() const
{
   return new FITSHeaderInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* FITSHeaderProcess::Clone( const ProcessImplementation& p ) const
{
   const FITSHeaderInstance* instPtr = dynamic_cast<const FITSHeaderInstance*>( &p );
   return (instPtr != nullptr) ? new FITSHeaderInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool FITSHeaderProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: FITSHeader [<arg_list>] [<view_list>]"
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

int FITSHeaderProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews,
                     ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   FITSHeaderInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
         throw Error( "Unknown numeric argument: " + arg.Token() );
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
         throw Error( "Unknown argument: " + arg.Token() );
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            continue;
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

   if ( launchInterface )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentWindow();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSHeaderProcess.cpp - Released 2025-04-07T08:53:56Z
