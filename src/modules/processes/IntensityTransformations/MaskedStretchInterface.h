//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// MaskedStretchInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __MaskedStretchInterface_h
#define __MaskedStretchInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/SpinBox.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "MaskedStretchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class MaskedStretchInterface : public ProcessInterface
{
public:

   MaskedStretchInterface();
   virtual ~MaskedStretchInterface();

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

   MaskedStretchInstance m_instance;

   struct GUIData
   {
      GUIData( MaskedStretchInterface& );

      VerticalSizer     Global_Sizer;
         NumericControl    TargetBackground_NumericControl;
         HorizontalSizer   NumberOfIterations_Sizer;
            Label             NumberOfIterations_Label;
            SpinBox           NumberOfIterations_SpinBox;
         HorizontalSizer   ClippingFraction_Sizer;
            NumericEdit       ClippingFraction_NumericEdit;
            ToolButton        ClippingFractionClear_ToolButton;
         HorizontalSizer   MaskType_Sizer;
            Label             MaskType_Label;
            ComboBox          MaskType_ComboBox;
         HorizontalSizer   BackgroundReferenceView_Sizer;
            Label             BackgroundReferenceView_Label;
            Edit              BackgroundReferenceView_Edit;
            ToolButton        BackgroundReferenceView_ToolButton;
         NumericControl    BackgroundLow_NumericControl;
         NumericControl    BackgroundHigh_NumericControl;

         SectionBar        ROI_SectionBar;
         Control           ROI_Control;
         VerticalSizer     ROI_Sizer;
            HorizontalSizer   ROIRow1_Sizer;
               Label             ROIX0_Label;
               SpinBox           ROIX0_SpinBox;
               Label             ROIY0_Label;
               SpinBox           ROIY0_SpinBox;
            HorizontalSizer   ROIRow2_Sizer;
               Label             ROIWidth_Label;
               SpinBox           ROIWidth_SpinBox;
               Label             ROIHeight_Label;
               SpinBox           ROIHeight_SpinBox;
               PushButton        ROISelectPreview_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ValueUpdated( NumericEdit& sender, double value );
   void __SpinValueUpdated( SpinBox& sender, int value );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __GetFocus( Control& sender );
   void __EditCompleted( Edit& sender );
   void __Click( Button& sender, bool checked );
   void __SectionBarCheck( SectionBar& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern MaskedStretchInterface* TheMaskedStretchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __MaskedStretchInterface_h

// ----------------------------------------------------------------------------
// EOF MaskedStretchInterface.h - Released 2025-04-07T08:53:56Z
