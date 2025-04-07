//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// AssignICCProfileProcess.cpp - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AssignICCProfileProcess.h"
#include "AssignICCProfileParameters.h"
#include "AssignICCProfileInstance.h"
#include "AssignICCProfileInterface.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/ICCProfile.h>
#include <pcl/File.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AssignICCProfileProcess* TheAssignICCProfileProcess = nullptr;

// ----------------------------------------------------------------------------

AssignICCProfileProcess::AssignICCProfileProcess()
{
   TheAssignICCProfileProcess = this;

   new ICCATargetProfile( this );
   new ICCAMode( this );
}

// ----------------------------------------------------------------------------

IsoString AssignICCProfileProcess::Id() const
{
   return "AssignICCProfile";
}

// ----------------------------------------------------------------------------

IsoString AssignICCProfileProcess::Category() const
{
   return "ColorManagement";
}

// ----------------------------------------------------------------------------

uint32 AssignICCProfileProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String AssignICCProfileProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

String AssignICCProfileProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/AssignICCProfile.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* AssignICCProfileProcess::DefaultInterface() const
{
   return TheAssignICCProfileInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* AssignICCProfileProcess::Create() const
{
   return new AssignICCProfileInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AssignICCProfileProcess::Clone( const ProcessImplementation& p ) const
{
   const AssignICCProfileInstance* instPtr = dynamic_cast<const AssignICCProfileInstance*>( &p );
   return (instPtr != nullptr) ? new AssignICCProfileInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: AssignICCProfile [<arg_list>] [<img_list>]"
"\n"
"\n-profile=<profile_id>"
"\n"
"\n      <profile_id> identifies the target ICC profile. The specified"
"\n      identifier must correspond to an installed color profile."
"\n"
"\n-filename=<file_name>"
"\n"
"\n      Specifies the target ICC profile by its file name. The specified file"
"\n      name must correspond to an installed ICC profile, and it must not"
"\n      include a directory. The system profile directories will always be"
"\n      used and cannot be changed."
"\n"
"\n-default[+|-]"
"\n"
"\n      When enabled, this option causes target image(s) to forget their"
"\n      current ICC profiles and assigns them the default RGB or grayscale"
"\n      profile, as per global color management settings."
"\n"
"\n-untag[+|-]"
"\n"
"\n      When enabled, this option causes target image(s) to forget their"
"\n      current ICC profiles, and leaves them untagged. The default profile"
"\n      (as per global color management settings) will be used to render the"
"\n      target image(s) on the screen."
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

int AssignICCProfileProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv,
                     ArgumentItemMode::AsViews,
                     ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   AssignICCProfileInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
         throw Error( "Unknown numeric argument: " + arg.Token() );
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "profile" )
         {
            instance.p_targetProfile = arg.StringValue();
            instance.p_targetProfile.Trim();
            if ( instance.p_targetProfile.IsEmpty() )
               throw Error( "Empty profile identifier: " + arg.Token() );
         }
         else if ( arg.Id() == "filename" )
         {
            String filename = arg.StringValue();
            filename.Trim();
            if ( filename.IsEmpty() )
               throw Error( "Empty file name: " + arg.Token() );

            instance.p_targetProfile.Clear();

            StringList dirs = ICCProfile::ProfileDirectories();
            for ( StringList::const_iterator i = dirs.Begin(); i != dirs.End(); ++i )
            {
               String path = *i + '/' + filename;
               if ( File::Exists( path ) )
               {
                  ICCProfile icc( path );
                  if ( icc.IsProfile() )
                  {
                     instance.p_targetProfile = icc.Description();
                     break;
                  }
               }
            }

            if ( instance.p_targetProfile.IsEmpty() )
               throw Error( "The specified file name does not correspond to a valid ICC profile: " + filename );
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "default" )
            instance.p_mode = arg.SwitchState() ? ICCAMode::AssignDefaultProfile : ICCAMode::AssignNewProfile;
         else if ( arg.Id() == "untag" )
            instance.p_mode = arg.SwitchState() ? ICCAMode::LeaveUntagged : ICCAMode::AssignNewProfile;
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "default" )
            instance.p_mode = ICCAMode::AssignDefaultProfile;
         else if ( arg.Id() == "untag" )
            instance.p_mode = ICCAMode::LeaveUntagged;
         else if ( arg.Id() == "-interface" )
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
      instance.LaunchOnCurrentWindow();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileProcess.cpp - Released 2025-04-07T08:53:55Z
