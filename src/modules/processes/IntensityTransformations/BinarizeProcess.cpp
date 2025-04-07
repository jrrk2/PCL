//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "BinarizeProcess.h"
#include "BinarizeParameters.h"
#include "BinarizeInstance.h"
#include "BinarizeInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BinarizeProcess* TheBinarizeProcess = nullptr;

// ----------------------------------------------------------------------------

BinarizeProcess::BinarizeProcess()
{
   TheBinarizeProcess = this;

   // Instantiate process parameters
   new BinarizeLevelR( this );
   new BinarizeLevelG( this );
   new BinarizeLevelB( this );
   new BinarizeIsGlobal( this );
}

// ----------------------------------------------------------------------------

IsoString BinarizeProcess::Id() const
{
   return "Binarize";
}

// ----------------------------------------------------------------------------

IsoString BinarizeProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 BinarizeProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String BinarizeProcess::Description() const
{
   return

   "<html>"
   "<p>Binarize sets every pixel value below a given threshold to pure black, "
   "and every pixel value above the same threshold to pure white.</p>"

   "<p>The user may set the threshold value either globally (as a unique value "
   "for all nominal channels in the image), or as a set of individual values on "
   "a per-channel basis.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String BinarizeProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Binarize.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* BinarizeProcess::DefaultInterface() const
{
   return TheBinarizeInterface;
}
// ----------------------------------------------------------------------------

ProcessImplementation* BinarizeProcess::Create() const
{
   return new BinarizeInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* BinarizeProcess::Clone( const ProcessImplementation& p ) const
{
   const BinarizeInstance* instPtr = dynamic_cast<const BinarizeInstance*>( &p );
   return (instPtr != nullptr) ? new BinarizeInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool BinarizeProcess::NeedsValidation() const
{
   return false;
}

// ----------------------------------------------------------------------------

bool BinarizeProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"\nUsage: Binarize [<arg_list>] [<view_list>]"
"\n"
"\n-t[<ch>]=<n> | -threshold[<ch>]=<n>"
"\n"
"\n      <n> is the binarization threshold, 0 <= n <= 1."
"\n"
"\n      The optional <ch> suffix selects a channel to apply the corresponding"
"\n      threshold. <ch> can be one of:"
"\n"
"\n      R|K : red/gray channel"
"\n      G   : green channel"
"\n      B   : blue channel"
"\n"
"\n      If no <ch> suffix is specified, values apply equally to the red(gray),"
"\n      green and blue channels of the target views."
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

// ----------------------------------------------------------------------------

static double ArgumentThresholdValue( const Argument& arg )
{
   if ( arg.NumericValue() < 0 || arg.NumericValue() > 1 )
      throw Error( "Binarization threshold out of range: " + arg.Token() );
   return arg.NumericValue();
}

// ----------------------------------------------------------------------------

int BinarizeProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
      ExtractArguments( argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards );

   BinarizeInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
      {
         if ( arg.Id() == "t" || arg.Id() == "threshold" )
         {
            instance.p_level[0] = instance.p_level[1] = instance.p_level[2] = ArgumentThresholdValue( arg );
            instance.p_global = true;
         }
         else if ( arg.Id() == "tR" || arg.Id() == "thresholdR" )
         {
            instance.p_level[0] = ArgumentThresholdValue( arg );
            instance.p_global = false;
         }
         else if ( arg.Id() == "tG" || arg.Id() == "thresholdG" )
         {
            instance.p_level[1] = ArgumentThresholdValue( arg );
            instance.p_global = false;
         }
         else if ( arg.Id() == "tB" || arg.Id() == "thresholdB" )
         {
            instance.p_level[2] = ArgumentThresholdValue( arg );
            instance.p_global = false;
         }
         else
            throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
         throw Error( "Unknown string argument: " + arg.Token() );
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
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
      instance.LaunchOnCurrentView();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BinarizeProcess.cpp - Released 2025-04-07T08:53:56Z
