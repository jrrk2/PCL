//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationInstance.h - Released 2025-04-07T08:53:56Z
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

#ifndef __AnnotationInstance_h
#define __AnnotationInstance_h

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class AnnotationInstance : public ProcessImplementation
{
public:

   AnnotationInstance( const MetaProcess* );
   AnnotationInstance( const AnnotationInstance& );

   void Assign( const ProcessImplementation& ) override;
   bool CanExecuteOn( const View&, pcl::String& whyNot ) const override;
   bool ExecuteOn( View& ) override;
   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

private:

   String annotationText;            // annotation text
   String annotationFont;            // annotation font face
   uint8 annotationFontSize;         // annotation font size in points
   pcl_bool annotationFontBold;      // flag whether font is bold
   pcl_bool annotationFontItalic;    // flag whether font is italic
   pcl_bool annotationFontUnderline; // flag whether font is underlined
   pcl_bool annotationFontShadow;    // flag whether font has shadow
   RGBA annotationColor;             // annotation color
   int32 annotationPositionX;        // annotation text position
   int32 annotationPositionY;
   pcl_bool annotationShowLeader;    // flag whether to show leader line
   int32 annotationLeaderPositionX;  // leader endpoint position
   int32 annotationLeaderPositionY;
   uint8 annotationOpacity;          // annotation opacity

   friend class AnnotationEngine;
   friend class AnnotationProcess;
   friend class AnnotationInterface;
   friend class AnnotationRenderer;
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __AnnotationInstance_h

// ----------------------------------------------------------------------------
// EOF AnnotationInstance.h - Released 2025-04-07T08:53:56Z
