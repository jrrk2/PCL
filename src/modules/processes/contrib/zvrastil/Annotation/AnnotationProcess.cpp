//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationProcess.cpp - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Annotation PixInsight module.
//
// Copyright (c) 2010-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#include "AnnotationProcess.h"
#include "AnnotationInstance.h"
#include "AnnotationInterface.h"
#include "AnnotationParameters.h"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationProcess* TheAnnotationProcess = nullptr;

// ----------------------------------------------------------------------------

AnnotationProcess::AnnotationProcess()
{
   TheAnnotationProcess = this;

   new AnnotationText( this );
   new AnnotationFont( this );
   new AnnotationFontSize( this );
   new AnnotationFontBold( this );
   new AnnotationFontItalic( this );
   new AnnotationFontUnderline( this );
   new AnnotationFontShadow( this );
   new AnnotationColor( this );
   new AnnotationPositionX( this );
   new AnnotationPositionY( this );
   new AnnotationShowLeader( this );
   new AnnotationLeaderPositionX( this );
   new AnnotationLeaderPositionY( this );
   new AnnotationOpacity( this );
}

// ----------------------------------------------------------------------------

IsoString AnnotationProcess::Id() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

IsoString AnnotationProcess::Category() const
{
   return "Painting";
}

// ----------------------------------------------------------------------------

uint32 AnnotationProcess::Version() const
{
   return 0x101;
}

// ----------------------------------------------------------------------------

String AnnotationProcess::Description() const
{
   return
   "<html>"
   "<p>Annotation is an interactive text rendering procedure implemented as a "
   "dynamic PixInsight process. Annotation can only be executed on images, not on previews. "
   "Annotation renders single line of text with selected font and color. Optionally, it also "
   "renders a leader line from text to object. It is designed mainly to add your name and copyright "
   "to the image or to annotate objects on the image.</p>"

   "<p>To start an Annotation session, double-click the corresponding entry on the "
   "Process Explorer; the Annotation interface window will be shown and activated. "
   "Then click on the target image to show the dynamic preview of the annotation. "
   "Now, you can edit the annotation text and properties. Using left mouse button, "
   "you can do fine positioning of the text and leader by dragging. If you hold Ctrl "
   "key during the dragging, text and leader are dragged simultaneously. Otherwise, "
   "only selected part of the annotation is dragged.</p>"

   "<p>By pressing Apply button on the Annotation window, annotation is rendered to the image "
   "pixels. Next click on the image creates new annotation preview in the image.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

String AnnotationProcess::IconImageSVGFile() const
{
   return "@module_icons_dir/Annotation.svg";
}

// ----------------------------------------------------------------------------

ProcessInterface* AnnotationProcess::DefaultInterface() const
{
   return TheAnnotationInterface;
}

// ----------------------------------------------------------------------------

ProcessImplementation* AnnotationProcess::Create() const
{
   return new AnnotationInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* AnnotationProcess::Clone( const ProcessImplementation& p ) const
{
   const AnnotationInstance* instPtr = dynamic_cast<const AnnotationInstance*>( &p );
   return (instPtr != nullptr) ? new AnnotationInstance( *instPtr ) : nullptr;
}

// ----------------------------------------------------------------------------

bool AnnotationProcess::CanProcessCommandLines() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF AnnotationProcess.cpp - Released 2025-04-07T08:53:56Z
