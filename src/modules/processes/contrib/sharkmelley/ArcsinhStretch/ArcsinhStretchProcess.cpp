//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ArcsinhStretchInstance.h"
#include "ArcsinhStretchInterface.h"
#include "ArcsinhStretchParameters.h"
#include "ArcsinhStretchProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretchProcess* TheArcsinhStretchProcess = nullptr;

// ----------------------------------------------------------------------------

ArcsinhStretchProcess::ArcsinhStretchProcess()
{
   TheArcsinhStretchProcess = this;

   new ArcsinhStretch( this );
   new ArcsinhStretchBlackPoint( this );
   new ArcsinhStretchProtectHighlights( this );
   new ArcsinhStretchUseRgbws( this );
   new ArcsinhStretchPreviewClipped( this );
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchProcess::Id() const
{
   return "ArcsinhStretch";
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 ArcsinhStretchProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ArcsinhStretchProcess::Description() const
{
   return
   "<html>"
   "<p>ArcsinhStretch. Apply hyperbolic arcsine stretch to the image intensity while preserving the original color.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/ArcsinhStretch.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ArcsinhStretchProcess::DefaultInterface() const
{
   return TheArcsinhStretchInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ArcsinhStretchProcess::Create() const
{
   return new ArcsinhStretchInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ArcsinhStretchProcess::Clone( const ProcessImplementation& p ) const
{
   const ArcsinhStretchInstance* instPtr = dynamic_cast<const ArcsinhStretchInstance*>( &p );
   return (instPtr != nullptr) ? new ArcsinhStretchInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ArcsinhStretchProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ArcsinhStretch [<arg_list>] [<view_list>]"
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

int ArcsinhStretchProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   ArcsinhStretchInstance instance( this );

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
            throw Error( "No view(s) found: " + arg.Token() );

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
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchProcess.cpp - Released 2025-04-07T08:53:56Z
