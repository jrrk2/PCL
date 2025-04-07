//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Annotation Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// AnnotationInterface.h - Released 2025-04-07T08:53:56Z
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

#ifndef __AnnotationInterface_h
#define __AnnotationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/CheckBox.h>
#include <pcl/ColorComboBox.h>
#include <pcl/Edit.h>
#include <pcl/FontComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "AnnotationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

namespace DraggingType
{
   enum dragging_type
   {
      None,
      Text,
      Leader,
      Both
   };
}

class AnnotationInterface : public ProcessInterface
{
public:

   AnnotationInterface();
   virtual ~AnnotationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsDynamicInterface() const override;
   void ExitDynamicMode() override;
   void Execute() override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers ) override;
   void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers ) override;
   bool RequiresDynamicUpdate( const View&, const DRect& ) const override;
   void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const override;
   void LoadSettings() override;
   void SaveSettings() const override;

private:

   AnnotationInstance instance;

   View* view = nullptr;          // dynamic target
   bool annotationPlaced = false; // flag whether annotation is placed in active view
   bool leaderPlaced = false;     // flag whether leader is placed in active view
   Rect textRect = 0;             // actual position of the annotation text in the image
   Rect leaderRect = 0;           // actual position of the annotation leader in the image
   Rect totalRect = 0;            // union of text and leader rect
   Rect invalidateRect = 0;       // rectangle to invalidate in next UpdateView call
   DraggingType::dragging_type dragging = DraggingType::None; // actual dragging mode
   int lastX = -1, lastY = -1;    // last position of the mouse while dragging
   mutable int relPosX = 0, relPosY = 0; // cached relative position of the annotation within bitmap
   mutable Bitmap annotationBmp = Bitmap::Null(); // cached annotation bitmap
   mutable Bitmap screenBmp = Bitmap::Null();     // cached screen bitmap

   struct GUIData
   {
      GUIData( AnnotationInterface& );

      VerticalSizer Global_Sizer;
      GroupBox Content_GroupBox;
      HorizontalSizer AnnotationText_Sizer;
      Label AnnotationText_Label;
      Edit AnnotationText_Edit;
      CheckBox AnnotationShowLeader_CheckBox;
      GroupBox Font_GroupBox;
      VerticalSizer Font_Sizer;
      HorizontalSizer AnnotationFont_Sizer;
      Label AnnotationFont_Label;
      FontComboBox AnnotationFont_ComboBox;
      Label AnnotationFontSize_Label;
      SpinBox AnnotationFontSize_SpinBox;
      HorizontalSizer AnnotationFontOptions_Sizer;
      CheckBox AnnotationFontBold_CheckBox;
      CheckBox AnnotationFontItalic_CheckBox;
      CheckBox AnnotationFontUnderline_CheckBox;
      CheckBox AnnotationFontShadow_CheckBox;
      HorizontalSizer AnnotationColor_Sizer;
      Label AnnotationColor_Label;
      ColorComboBox AnnotationColor_ComboBox;
      Control AnnotationColor_CustomColorSample;
      NumericControl AnnotationOpacity_NumericControl;
   };

   GUIData* GUI = nullptr;

   // Destroy working bitmaps
   void ClearBitmaps();

   // synchronize control values with current instance
   void UpdateControls();

   // redraw current invalidate rectangle
   void UpdateView();

   // update text and leader rectangles
   void UpdateAnnotationRect( bool reset = false );

   // place leader to default position (if not yet placed)
   void PlaceLeaderDefault();

   // Event Handlers
   void __EditTextUpdated( Edit& sender, const String& text );
   void __FontComboBoxFontSelected( FontComboBox& sender, const String& font );
   void __FontSizeSpinBoxValueUpdated( SpinBox& sender, int value );
   void __FontOptionCheckBoxChecked( Button& sender, Button::check_state state );
   void __ColorComboBoxColorSelected( ColorComboBox& sender, RGBA color );
   void __ColorSample_Paint( Control& sender, const Rect& );
   void __ColorSample_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __OpacityUpdated( NumericEdit& sender, double value );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AnnotationInterface* TheAnnotationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __AnnotationInterface_h

// ----------------------------------------------------------------------------
// EOF AnnotationInterface.h - Released 2025-04-07T08:53:56Z
