//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "ConvolutionProcess.h"
#include "ConvolutionParameters.h"
#include "ConvolutionInstance.h"
#include "ConvolutionInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ConvolutionProcess* TheConvolutionProcess = nullptr;

// ----------------------------------------------------------------------------

ConvolutionProcess::ConvolutionProcess()
{
   TheConvolutionProcess = this;

   new ConMode( this );
   new ConSigma( this );
   new ConShape( this );
   new ConAspectRatio( this );
   new ConRotationAngle( this );
   new ConSourceCode( this );
   new ConRescaleHighPass( this );
   new ConViewId( this );
}

// ----------------------------------------------------------------------------

IsoString ConvolutionProcess::Id() const
{
   return "Convolution";
}

// ----------------------------------------------------------------------------

IsoString ConvolutionProcess::Category() const
{
   return "Convolution";
}

// ----------------------------------------------------------------------------

uint32 ConvolutionProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ConvolutionProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String ConvolutionProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Convolution.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* ConvolutionProcess::DefaultInterface() const
{
   return TheConvolutionInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvolutionProcess::Create() const
{
   return new ConvolutionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ConvolutionProcess::Clone( const ProcessImplementation& p ) const
{
   const ConvolutionInstance* instPtr = dynamic_cast<const ConvolutionInstance*>( &p );
   return (instPtr != nullptr) ? new ConvolutionInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool ConvolutionProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

// ### TODO

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: Convolution [<arg_list>] [<view_list>]"
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

int ConvolutionProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments = ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   ConvolutionInstance instance( this );

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
// EOF ConvolutionProcess.cpp - Released 2025-04-07T08:53:55Z
