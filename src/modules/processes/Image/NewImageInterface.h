//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// NewImageInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NewImageInterface_h
#define __NewImageInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "NewImageInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class NewImageInterface : public ProcessInterface
{
public:

   NewImageInterface();
   virtual ~NewImageInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const pcl::DPoint&, double R, double G, double B, double A ) override;

private:

   NewImageInstance instance;

   struct GUIData
   {
      GUIData( NewImageInterface& );

      VerticalSizer     Global_Sizer;
         GroupBox          ImageParameters_GroupBox;
         VerticalSizer     ImageParameters_Sizer;
            HorizontalSizer   Identifier_Sizer;
               Label             Identifier_Label;
               Edit              Identifier_Edit;
            HorizontalSizer   SampleFormat_Sizer;
               Label             SampleFormat_Label;
               ComboBox          SampleFormat_ComboBox;
            HorizontalSizer   ColorSpace_Sizer;
               Label             ColorSpace_Label;
               RadioButton       RGBColor_RadioButton;
               RadioButton       Grayscale_RadioButton;
         HorizontalSizer   Geometry_Sizer;
            VerticalSizer     GeometryLeft_Sizer;
               HorizontalSizer   Width_Sizer;
                  Label             Width_Label;
                  SpinBox           Width_SpinBox;
               HorizontalSizer   Height_Sizer;
                  Label             Height_Label;
                  SpinBox           Height_SpinBox;
               HorizontalSizer   Channels_Sizer;
                  Label             Channels_Label;
                  SpinBox           Channels_SpinBox;
            VerticalSizer     GeometryMiddle_Sizer;
               Label             SizeInfo_Label;
            VerticalSizer     GeometryRight_Sizer;
               PushButton        SetAsActiveImage_Button;
         GroupBox          InitialValues_GroupBox;
         HorizontalSizer   InitialValues_Sizer;
            VerticalSizer     InitialValuesLeft_Sizer;
               NumericControl    V0_NumericControl;
               NumericControl    V1_NumericControl;
               NumericControl    V2_NumericControl;
               NumericControl    VA_NumericControl;
            Control           ColorSample_Control;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateInitialValue( Edit&, Slider&, double );
   void UpdateSizeInfo();

   void __Identifier_GetFocus( Control& sender );
   void __Identifier_EditCompleted( Edit& sender );
   void __SampleFormat_ItemSelected( ComboBox& sender, int itemIndex );
   void __ColorSpace_Click( Button& sender, bool checked );
   void __Geometry_ValueUpdated( SpinBox& sender, int value );
   void __SetAsActiveImage_Click( Button& sender, bool checked );
   void __InitialValue_ValueUpdated( NumericEdit& sender, double value );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   void SetAsImage( const ImageWindow& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NewImageInterface* TheNewImageInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NewImageInterface_h

// ----------------------------------------------------------------------------
// EOF NewImageInterface.h - Released 2025-04-07T08:53:56Z
