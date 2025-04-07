//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// SampleFormatConversionProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "SampleFormatConversionProcess.h"
#include "SampleFormatConversionParameters.h"
#include "SampleFormatConversionInstance.h"
#include "SampleFormatConversionInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SampleFormatConversionProcess* TheSampleFormatConversionProcess = nullptr;

// ----------------------------------------------------------------------------

SampleFormatConversionProcess::SampleFormatConversionProcess()
{
   TheSampleFormatConversionProcess = this;

   new SampleFormatConversion( this );
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversionProcess::Id() const
{
   return "SampleFormatConversion";
}

// ----------------------------------------------------------------------------

IsoString SampleFormatConversionProcess::Category() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

uint32 SampleFormatConversionProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String SampleFormatConversionProcess::Description() const
{
   return

   "<html>"
   "<p>The SampleFormatConversion process converts images between the different sample formats supported by PixInsight: 8, 16 and "
   "32-bit integers, 32 and 64-bit floating point. The sample format defines a numerical type and range used internally to store "
   "and manipulate pixel data in memory.</p>"

   "<p>In general, the 8-bit integer format should be avoided to perform actual image processing, as far as possible, due to its limited "
   "range of just 256 discrete values. It can be useful, however, to manage huge image files, due to its low memory consumption.</p>"

   "<p>The 16-bit integer format uses the native range of most CCD and CMOS imaging devices and is quite appropriate for processing, but "
   "in general a 32-bit format is preferable. The 16-bit format is a good option to store relatively simple masks, since it requires "
   "just two bytes for each gray pixel value.</p>"

   "<p>The 32-bit integer format, along with the 64-bit floating-point format, is an excellent option for processing works requiring "
   "extremely large dynamic ranges, thanks to its wide data range of 2^32 discrete values. The 32-bit integer format can be somewhat "
   "slow with processes that perform intensive calculations.</p>"

   "<p>Floating-point formats provide high numerical accuracy, which is essential to avoid image degradation due to roundoff errors, "
   "especially during complex and extended processing works. The 32-bit floating-point format is probably the best-balanced option "
   "available in PixInsight. It provides an effective data range of 10^6 to 10^7 discrete levels in PixInsight, depending on the "
   "complexity of the applied operations.</p>"

   "<p>The 64-bit floating-point format provides a huge data range of 10^15 discrete levels, but it requires 24 bytes to store each "
   "pixel of a RGB color image, so it can be prohibitive for large images on 32-bit platforms. This format is indicated for complex "
   "processing works that must preserve extremely large dynamic ranges. For example, with the 64-bit floating-point format you can "
   "integrate a set of exposures from 1/1000 of a second to several seconds as a single image without any data loss. Or the whole "
   "brightness range and chromatic richness in the Great Orion Nebula, rendering from the most subtle details of the Trapezium area "
   "to the dimmest nebular tendrils on the regions between M43 and NGC 1977.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String SampleFormatConversionProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/SampleFormatConversion.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* SampleFormatConversionProcess::DefaultInterface() const
{
   return TheSampleFormatConversionInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* SampleFormatConversionProcess::Create() const
{
   return new SampleFormatConversionInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* SampleFormatConversionProcess::Clone( const ProcessImplementation& p ) const
{
   const SampleFormatConversionInstance* instPtr = dynamic_cast<const SampleFormatConversionInstance*>( &p );
   return (instPtr != nullptr) ? new SampleFormatConversionInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool SampleFormatConversionProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: SampleFormatConversion [<arg_list>] [<view_list>]"
"\n"
"\n-i<n>"
"\n"
"\n      Convert to an integer format. <n> specifies the number of bits per"
"\n      sample and must be one of: 8,16,32."
"\n"
"\n-f<n>"
"\n"
"\n      Convert to a floating point real format. <n> specifies the number of"
"\n      bits per sample and must be one of: 32,64."
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

int SampleFormatConversionProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews,
                     ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   SampleFormatConversionInstance instance( this );

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
      {
         if ( arg.Id() == "i8" )
            instance.sampleFormat = SampleFormatConversion::To8Bit;
         else if ( arg.Id() == "i16" )
            instance.sampleFormat = SampleFormatConversion::To16Bit;
         else if ( arg.Id() == "i32" )
            instance.sampleFormat = SampleFormatConversion::To32Bit;
         else if ( arg.Id() == "f32" )
            instance.sampleFormat = SampleFormatConversion::ToFloat;
         else if ( arg.Id() == "f64" )
            instance.sampleFormat = SampleFormatConversion::ToDouble;
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
// EOF SampleFormatConversionProcess.cpp - Released 2025-04-07T08:53:56Z
