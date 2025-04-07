//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 1.0.5
// ----------------------------------------------------------------------------
// RestorationFilterInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RestorationFilterInterface_h
#define __RestorationFilterInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TabBox.h>
#include <pcl/ToolButton.h>

#include "RestorationFilterInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RestorationFilterInterface : public ProcessInterface
{
public:

   RestorationFilterInterface();
   virtual ~RestorationFilterInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   RestorationFilterInstance instance;

   struct GUIData
   {
      GUIData( RestorationFilterInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        PSF_SectionBar;
      Control           PSF_Control;
      HorizontalSizer     PSF_Sizer;
         TabBox               PSFMode_TabBox;
            Control              Parametric_Control;
            VerticalSizer        Parametric_Sizer;
               NumericControl       ParametricSigma_NumericControl;
               NumericControl       ParametricShape_NumericControl;
               NumericControl       ParametricRatio_NumericControl;
               NumericControl       ParametricAngle_NumericControl;
            Control              Motion_Control;
            VerticalSizer        Motion_Sizer;
               NumericControl       MotionLength_NumericControl;
               NumericControl       MotionAngle_NumericControl;
            Control              PSFImage_Control;
            VerticalSizer        PSFImage_Sizer;
               Label                PSFImage_Label;
               HorizontalSizer      PSFImageEdit_Sizer;
               Edit                    PSFImage_Edit;
               ToolButton              PSFImage_Button;
         VerticalSizer        PSFDraw_Sizer;
            Control              PSFDraw_Control;
            Label                PSFDraw_Label;

      SectionBar        Noise_SectionBar;
      Control           Noise_Control;
      HorizontalSizer   Noise_Sizer;
         NumericEdit       Noise_NumericControl;
         SpinBox           NoiseFine_SpinBox;
         Label             NoiseCoarse_Label;
         SpinBox           NoiseCoarse_SpinBox;

      SectionBar        Filter_SectionBar;
      Control           Filter_Control;
      VerticalSizer     Filter_Sizer;
         HorizontalSizer   Algorithm_Sizer;
            Label             Algorithm_Label;
            ComboBox          Algorithm_ComboBox;
         NumericControl    Amount_NumericControl;
         HorizontalSizer   Target_Sizer;
            Label             Target_Label;
            ComboBox          Target_ComboBox;

      SectionBar        Deringing_SectionBar;
      Control           Deringing_Control;
      VerticalSizer     Deringing_Sizer;
         NumericControl    DeringingDark_NumericControl;
         NumericControl    DeringingBright_NumericControl;
         HorizontalSizer   OutputDeringingMaps_Sizer;
            CheckBox          OutputDeringingMaps_CheckBox;

      SectionBar        Range_SectionBar;
      Control           Range_Control;
      VerticalSizer     Range_Sizer;
         NumericControl    RangeLow_NumericControl;
         NumericControl    RangeHigh_NumericControl;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdatePSFControls();
   void UpdateNoiseControls();
   void UpdateFilterControls();
   void UpdateDeringingControls();
   void UpdateRangeControls();

   void __PSF_PageSelected( TabBox& sender, int pageIndex );
   void __PSF_ValueUpdated( NumericEdit& sender, double value );
   void __PSF_EditCompleted( Edit& sender );
   void __PSF_Click( Button& sender, bool checked );
   void __PSF_Paint( Control& sender, const Rect& updateRect );
   void __Noise_ValueUpdated( NumericEdit& sender, double value );
   void __Noise_SpinValueUpdated( SpinBox& sender, int value );
   void __Filter_ItemSelected( ComboBox& sender, int itemIndex );
   void __Filter_ValueUpdated( NumericEdit& sender, double value );
   void __Target_ItemSelected( ComboBox& sender, int itemIndex );
   void __Deringing_Check( SectionBar& sender, bool checked );
   void __Deringing_ValueUpdated( NumericEdit& sender, double value );
   void __Deringing_Click( Button& sender, bool checked );
   void __Range_ValueUpdated( NumericEdit& sender, double value );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RestorationFilterInterface* TheRestorationFilterInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RestorationFilterInterface_h

// ----------------------------------------------------------------------------
// EOF RestorationFilterInterface.h - Released 2025-04-07T08:53:56Z
