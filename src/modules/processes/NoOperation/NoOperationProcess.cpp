//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoOperation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// NoOperationProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoOperation PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "NoOperationInstance.h"
#include "NoOperationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NoOperationProcess* TheNoOperationProcess = nullptr;

// ----------------------------------------------------------------------------

NoOperationProcess::NoOperationProcess()
{
   TheNoOperationProcess = this;
}

// ----------------------------------------------------------------------------

IsoString NoOperationProcess::Id() const
{
   return "NoOperation";
}

// ----------------------------------------------------------------------------

IsoString NoOperationProcess::Category() const
{
   return IsoString(); // No specific category
}

// ----------------------------------------------------------------------------

uint32 NoOperationProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String NoOperationProcess::Description() const
{
   return

   "<html>"
   "<p>As its name suggests, NoOperation does exactly nothing. This is indeed the simplest possible "
   "process on the PixInsight platform</p>"

   "<p>The purpose of NoOperation, however, is not so simple. Process instances are the main actors in "
   "PixInsight, since they are responsible for all of the actual image processing work. However, "
   "there are special situations in which one does not want to modify any image, but despite that, "
   "a process has to be used <i>in formal terms</i> for a given task to be accomplished.</p>"

   "<p>A notable example of such situations happens with ImageContainer. ImageContainer can be used to "
   "perform a batch format conversion for a set of image files. In this case, one simply specifies "
   "an output filename template like, for example:</p>"

   "<p>&amp;filename;.jpg</p>"

   "<p>to convert every input image file (in any supported file format) to the JPEG format.</p>"

   "<p>However, for ImageContainer to work, execution of a process instance is always required. In this "
   "case, NoOperation can be used to execute ImageContainer without applying any actual process to the "
   "input images, which are simply read from disk and written in the JPEG format (which corresponds to "
   "the .jpg file extension).</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String NoOperationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/NoOperation.svg";
}

// ----------------------------------------------------------------------------

bool NoOperationProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

ProcessImplementation* NoOperationProcess::Create() const
{
   return new NoOperationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* NoOperationProcess::Clone( const ProcessImplementation& p ) const
{
   const NoOperationInstance* instPtr = dynamic_cast<const NoOperationInstance*>( &p );
   return (instPtr != nullptr) ? new NoOperationInstance( this ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoOperationProcess.cpp - Released 2025-04-07T08:53:56Z
