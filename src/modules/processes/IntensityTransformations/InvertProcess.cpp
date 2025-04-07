//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// InvertProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "InvertProcess.h"
#include "InvertInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

InvertProcess* TheInvertProcess = nullptr;

// ----------------------------------------------------------------------------

InvertProcess::InvertProcess()
{
   TheInvertProcess = this;
}

// ----------------------------------------------------------------------------

IsoString InvertProcess::Id() const
{
   return "Invert";
}

// ----------------------------------------------------------------------------

IsoString InvertProcess::Category() const
{
   return "IntensityTransformations";
}

// ----------------------------------------------------------------------------

uint32 InvertProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String InvertProcess::Description() const
{
   return

   "<html>"
   "<p>Invert replaces pixel values with their inverse values. In the normalized "
   "real range (0=black, 1=white), the inverse of a pixel value <i>x</i> is given "
   "by 1&ndash;<i>x</i>. After inversion, black pixels become white and white pixels "
   "become black.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String InvertProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Invert.svg";
}

// ----------------------------------------------------------------------------

ProcessImplementation* InvertProcess::Create() const
{
   return new InvertInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* InvertProcess::Clone( const ProcessImplementation& p ) const
{
   const InvertInstance* instPtr = dynamic_cast<const InvertInstance*>( &p );
   return (instPtr != nullptr) ? new InvertInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF InvertProcess.cpp - Released 2025-04-07T08:53:56Z
