//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationParameters.cpp - Released 2025-04-07T08:53:56Z
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

#include "AnnotationParameters.h"

#include "pcl/Color.h"
#include "pcl/Font.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AnnotationText*            TheAnnotationTextParameter = nullptr;
AnnotationFont*            TheAnnotationFontParameter = nullptr;
AnnotationFontSize*        TheAnnotationFontSizeParameter = nullptr;
AnnotationFontBold*        TheAnnotationFontBoldParameter = nullptr;
AnnotationFontItalic*      TheAnnotationFontItalicParameter = nullptr;
AnnotationFontUnderline*   TheAnnotationFontUnderlineParameter = nullptr;
AnnotationFontShadow*      TheAnnotationFontShadowParameter = nullptr;
AnnotationColor*           TheAnnotationColorParameter = nullptr;
AnnotationPositionX*       TheAnnotationPositionXParameter = nullptr;
AnnotationPositionY*       TheAnnotationPositionYParameter = nullptr;
AnnotationShowLeader*      TheAnnotationShowLeaderParameter = nullptr;
AnnotationLeaderPositionX* TheAnnotationLeaderPositionXParameter = nullptr;
AnnotationLeaderPositionY* TheAnnotationLeaderPositionYParameter = nullptr;
AnnotationOpacity*         TheAnnotationOpacityParameter = nullptr;

// ----------------------------------------------------------------------------

AnnotationText::AnnotationText( MetaProcess* P )
   : MetaString( P )
{
   TheAnnotationTextParameter = this;
}

IsoString AnnotationText::Id() const
{
   return "annotationText";
}

size_type AnnotationText::MinLength() const
{
   return 0;
}

String AnnotationText::DefaultValue() const
{
   return "Annotation";
}

// ----------------------------------------------------------------------------

AnnotationFont::AnnotationFont( MetaProcess* P )
   : MetaString( P )
{
   TheAnnotationFontParameter = this;
}

IsoString AnnotationFont::Id() const
{
   return "annotationFont";
}

size_type AnnotationFont::MinLength() const
{
   return 0;
}

String AnnotationFont::DefaultValue() const
{
   return FontFamily::FamilyToFace( FontFamily::Default );
}

// ----------------------------------------------------------------------------

AnnotationFontSize::AnnotationFontSize( MetaProcess* P )
   : MetaUInt8( P )
{
   TheAnnotationFontSizeParameter = this;
}

IsoString AnnotationFontSize::Id() const
{
   return "annotationFontSize";
}

double AnnotationFontSize::DefaultValue() const
{
   return 12;
}

// ----------------------------------------------------------------------------

AnnotationFontBold::AnnotationFontBold( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontBoldParameter = this;
}

IsoString AnnotationFontBold::Id() const
{
   return "annotationFontBold";
}

// ----------------------------------------------------------------------------

AnnotationFontItalic::AnnotationFontItalic( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontItalicParameter = this;
}

IsoString AnnotationFontItalic::Id() const
{
   return "annotationFontItalic";
}

// ----------------------------------------------------------------------------

AnnotationFontUnderline::AnnotationFontUnderline( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontUnderlineParameter = this;
}

IsoString AnnotationFontUnderline::Id() const
{
   return "annotationFontUnderline";
}

// ----------------------------------------------------------------------------

AnnotationFontShadow::AnnotationFontShadow( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationFontShadowParameter = this;
}

IsoString AnnotationFontShadow::Id() const
{
   return "annotationFontShadow";
}

// ----------------------------------------------------------------------------

AnnotationColor::AnnotationColor( MetaProcess* P )
   : MetaUInt32( P )
{
   TheAnnotationColorParameter = this;
}

IsoString AnnotationColor::Id() const
{
   return "annotationColor";
}

double AnnotationColor::DefaultValue() const
{
   return RGBAColor( 192, 192, 192 ); //Silver
}

// ----------------------------------------------------------------------------

AnnotationPositionX::AnnotationPositionX( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationPositionXParameter = this;
}

IsoString AnnotationPositionX::Id() const
{
   return "annotationPositionX";
}

// ----------------------------------------------------------------------------

AnnotationPositionY::AnnotationPositionY( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationPositionYParameter = this;
}

IsoString AnnotationPositionY::Id() const
{
   return "annotationPositionY";
}

// ----------------------------------------------------------------------------

AnnotationShowLeader::AnnotationShowLeader( MetaProcess* P )
   : MetaBoolean( P )
{
   TheAnnotationShowLeaderParameter = this;
}

IsoString AnnotationShowLeader::Id() const
{
   return "annotationShowLeader";
}

// ----------------------------------------------------------------------------

AnnotationLeaderPositionX::AnnotationLeaderPositionX( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationLeaderPositionXParameter = this;
}

IsoString AnnotationLeaderPositionX::Id() const
{
   return "annotationLeaderPositionX";
}

// ----------------------------------------------------------------------------

AnnotationLeaderPositionY::AnnotationLeaderPositionY( MetaProcess* P )
   : MetaInt32( P )
{
   TheAnnotationLeaderPositionYParameter = this;
}

IsoString AnnotationLeaderPositionY::Id() const
{
   return "annotationLeaderPositionY";
}

// ----------------------------------------------------------------------------

AnnotationOpacity::AnnotationOpacity( MetaProcess* P )
   : MetaUInt8( P )
{
   TheAnnotationOpacityParameter = this;
}

IsoString AnnotationOpacity::Id() const
{
   return "annotationOpacity";
}

double AnnotationOpacity::DefaultValue() const
{
   return 255;
}

// ----------------------------------------------------------------------------

} // namespace pcl

// ----------------------------------------------------------------------------
// EOF AnnotationParameters.cpp - Released 2025-04-07T08:53:56Z
