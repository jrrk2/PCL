//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// RotationInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RotationInterface_h
#define __RotationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "RotationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RotationInterface : public ProcessInterface
{
public:

   RotationInterface();
   virtual ~RotationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;

private:

   RotationInstance m_instance;

   struct GUIData
   {
      GUIData( RotationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Rotation_SectionBar;
      Control           Rotation_Control;
      HorizontalSizer   Rotation_Sizer;
         VerticalSizer     RotationLeft_Sizer;
            NumericEdit       Angle_NumericEdit;
            HorizontalSizer   Clockwise_Sizer;
               Label             Clockwise_Label;
               CheckBox          Clockwise_CheckBox;
            HorizontalSizer   OptimizeFast_Sizer;
               Label             OptimizeFast_Label;
               CheckBox          OptimizeFast_CheckBox;
         Control           Dial_Control;

      SectionBar        Interpolation_SectionBar;
      Control           Interpolation_Control;
      VerticalSizer     Interpolation_Sizer;
         HorizontalSizer   Algorithm_Sizer;
            Label             Algorithm_Label;
            ComboBox          Algorithm_ComboBox;
         NumericEdit       ClampingThreshold_NumericEdit;
         NumericEdit       Smoothness_NumericEdit;
         HorizontalSizer   GammaCorrection_Sizer;
            CheckBox          GammaCorrection_CheckBox;

      SectionBar        FillColor_SectionBar;
      Control           FillColor_Control;
      VerticalSizer     FillColor_Sizer;
         NumericControl    Red_NumericControl;
         NumericControl    Green_NumericControl;
         NumericControl    Blue_NumericControl;
         NumericControl    Alpha_NumericControl;
         Control           ColorSample_Control;
   };

   GUIData* GUI = nullptr;

   bool m_dragging = false; // dragging the mouse cursor on the angle dial

   void UpdateControls();
   void UpdateNumericControls();
   void UpdateFillColorControls();

   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_ButtonClick( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_MouseMove( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void e_MousePress( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const Point& pos, int button, unsigned buttons, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RotationInterface* TheRotationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RotationInterface_h

// ----------------------------------------------------------------------------
// EOF RotationInterface.h - Released 2025-04-07T08:53:55Z
