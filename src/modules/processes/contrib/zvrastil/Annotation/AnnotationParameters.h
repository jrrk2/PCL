//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationParameters.h - Released 2025-04-07T08:53:56Z
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

#ifndef __AnnotationParameters_h
#define __AnnotationParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

class AnnotationText : public MetaString
{
public:

   AnnotationText( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String DefaultValue() const override;
};

extern AnnotationText* TheAnnotationTextParameter;

// ----------------------------------------------------------------------------

class AnnotationFont : public MetaString
{
public:

   AnnotationFont( MetaProcess* );

   IsoString Id() const override;
   size_type MinLength() const override;
   String DefaultValue() const override;
};

extern AnnotationFont* TheAnnotationFontParameter;

// ----------------------------------------------------------------------------

class AnnotationFontSize : public MetaUInt8
{
public:

   AnnotationFontSize( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AnnotationFontSize* TheAnnotationFontSizeParameter;

// ----------------------------------------------------------------------------

class AnnotationFontBold : public MetaBoolean
{
public:

   AnnotationFontBold( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontBold* TheAnnotationFontBoldParameter;

// ----------------------------------------------------------------------------

class AnnotationFontItalic : public MetaBoolean
{
public:

   AnnotationFontItalic( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontItalic* TheAnnotationFontItalicParameter;

// ----------------------------------------------------------------------------

class AnnotationFontUnderline : public MetaBoolean
{
public:

   AnnotationFontUnderline( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontUnderline* TheAnnotationFontUnderlineParameter;

// ----------------------------------------------------------------------------

class AnnotationFontShadow : public MetaBoolean
{
public:

   AnnotationFontShadow( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationFontShadow* TheAnnotationFontShadowParameter;

// ----------------------------------------------------------------------------

class AnnotationColor : public MetaUInt32
{
public:

   AnnotationColor( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AnnotationColor* TheAnnotationColorParameter;

// ----------------------------------------------------------------------------

class AnnotationPositionX : public MetaInt32
{
public:

   AnnotationPositionX( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationPositionX* TheAnnotationPositionXParameter;

// ----------------------------------------------------------------------------

class AnnotationPositionY : public MetaInt32
{
public:

   AnnotationPositionY( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationPositionY* TheAnnotationPositionYParameter;

// ----------------------------------------------------------------------------

class AnnotationShowLeader : public MetaBoolean
{
public:

   AnnotationShowLeader( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationShowLeader* TheAnnotationShowLeaderParameter;

// ----------------------------------------------------------------------------

class AnnotationLeaderPositionX : public MetaInt32
{
public:

   AnnotationLeaderPositionX( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationLeaderPositionX* TheAnnotationLeaderPositionXParameter;

// ----------------------------------------------------------------------------

class AnnotationLeaderPositionY : public MetaInt32
{
public:

   AnnotationLeaderPositionY( MetaProcess* );

   IsoString Id() const override;
};

extern AnnotationLeaderPositionY* TheAnnotationLeaderPositionYParameter;

// ----------------------------------------------------------------------------

class AnnotationOpacity : public MetaUInt8
{
public:

   AnnotationOpacity( MetaProcess* );

   IsoString Id() const override;
   double DefaultValue() const override;
};

extern AnnotationOpacity* TheAnnotationOpacityParameter;

// ----------------------------------------------------------------------------

PCL_END_LOCAL

} // namespace pcl

#endif // __AnnotationParameters_h

// ----------------------------------------------------------------------------
// EOF AnnotationParameters.h - Released 2025-04-07T08:53:56Z
