//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// BlinkProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "BlinkProcess.h"
#include "BlinkInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BlinkProcess* TheBlinkProcess = nullptr;

// ----------------------------------------------------------------------------

BlinkProcess::BlinkProcess()
{
   TheBlinkProcess = this;
}

// ----------------------------------------------------------------------------

IsoString BlinkProcess::Id() const
{
   return "Blink";
}

// ----------------------------------------------------------------------------

IsoString BlinkProcess::Category() const
{
   return "ImageInspection";
}

// ----------------------------------------------------------------------------

String BlinkProcess::Description() const
{
   return
   "<html>"
   "<p>The Blink tool shows a set of images like a movie to help compare images "
   "visually and sort them by their quality or other criteria.</p>"
   "<p>Blink also produces reports with image statistics and FITS keywords, and "
   "can also generate video files for selected regions with optional automatic "
   "screen stretch functions applied.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String BlinkProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Blink.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* BlinkProcess::DefaultInterface() const
{
   return TheBlinkInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BlinkProcess::Create() const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

ProcessImplementation* BlinkProcess::Clone( const ProcessImplementation& ) const
{
   return nullptr;
}

// ----------------------------------------------------------------------------

bool BlinkProcess::IsAssignable() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BlinkProcess.cpp - Released 2025-04-07T08:53:56Z
