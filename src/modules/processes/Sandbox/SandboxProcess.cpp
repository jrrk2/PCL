//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SandboxInstance.h"
#include "SandboxInterface.h"
#include "SandboxParameters.h"
#include "SandboxProcess.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxProcess* TheSandboxProcess = nullptr;

// ----------------------------------------------------------------------------

SandboxProcess::SandboxProcess()
{
   TheSandboxProcess = this;

   /*
    * Instantiate process parameters.
    */
   new SandboxParameterOne( this );
   new SandboxParameterTwo( this );
   new SandboxParameterThree( this );
   new SandboxParameterFour( this );
   new SandboxParameterFive( this );
}

// ----------------------------------------------------------------------------

IsoString SandboxProcess::Id() const
{
   return "Sandbox";
}

// ----------------------------------------------------------------------------

IsoString SandboxProcess::Category() const
{
   /*
    * Comma-separated list of categories to which this process belongs.
    * Return an empty IsoString() if no category is applicable to this process,
    * which is *not recommended*.
    */
   return IsoString();
}

// ----------------------------------------------------------------------------

uint32 SandboxProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SandboxProcess::Description() const
{
   return
   "<html>"
   "<p>"
   "Sandbox is just a starting point for development of new PixInsight "
   "modules. It is an empty module that does nothing but to provide the basic "
   "structure of a module with a process, a few parameters, and a simple "
   "process interface."
   "</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String SandboxProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Sandbox.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SandboxProcess::DefaultInterface() const
{
   return TheSandboxInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SandboxProcess::Create() const
{
   return new SandboxInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SandboxProcess::Clone( const ProcessImplementation& p ) const
{
   const SandboxInstance* instance = dynamic_cast<const SandboxInstance*>( &p );
   return (instance != nullptr) ? new SandboxInstance( *instance ) : nullptr;
}

// ----------------------------------------------------------------------------

bool SandboxProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Sandbox [<arg_list>] [<view_list>]"
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

int SandboxProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   SandboxInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( const Argument& arg : arguments )
   {
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
// EOF SandboxProcess.cpp - Released 2025-04-07T08:53:56Z
