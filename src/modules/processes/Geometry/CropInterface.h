//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// CropInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CropInterface_h
#define __CropInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/ViewList.h>

#include "CropInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class CropInterface : public ProcessInterface
{
public:

   CropInterface();
   virtual ~CropInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;

private:

   CropInstance m_instance;

   // Sample source dimensions in pixels.
   int sourceWidth = 1000;
   int sourceHeight = 1000;

   // Current anchor point:
   // 0 1 2
   // 3 4 5
   // 6 7 8
   int anchor = 4; // center

   struct GUIData
   {
      GUIData( CropInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        CropMargins_SectionBar;
      Control           CropMargins_Control;
      VerticalSizer     CropMargins_Sizer;
         HorizontalSizer   CropMarginsTop_Sizer;
            NumericEdit       TopMargin_NumericEdit;
         HorizontalSizer   CropMarginsMiddle_Sizer;
            NumericEdit       LeftMargin_NumericEdit;
            VerticalSizer        AnchorButtons_Sizer;
               HorizontalSizer      AnchorRow1_Sizer;
                  ToolButton           TL_ToolButton;
                  ToolButton           TM_ToolButton;
                  ToolButton           TR_ToolButton;
               HorizontalSizer      AnchorRow2_Sizer;
                  ToolButton           ML_ToolButton;
                  ToolButton           MM_ToolButton;
                  ToolButton           MR_ToolButton;
               HorizontalSizer      AnchorRow3_Sizer;
                  ToolButton           BL_ToolButton;
                  ToolButton           BM_ToolButton;
                  ToolButton           BR_ToolButton;
            NumericEdit       RightMargin_NumericEdit;
         HorizontalSizer   CropMarginsBottom_Sizer;
            NumericEdit       BottomMargin_NumericEdit;

      SectionBar        Dimensions_SectionBar;
      Control           Dimensions_Control;
      VerticalSizer     Dimensions_Sizer;
         HorizontalSizer   DimensionsRow1_Sizer;
            Label             SourcePixels_Label;
            Label             TargetPixels_Label;
            Label             TargetPercent_Label;
            Label             TargetCentimeters_Label;
            Label             TargetInches_Label;
         HorizontalSizer   DimensionsRow2_Sizer;
            Label             Width_Label;
            NumericEdit       SourceWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPixels_NumericEdit;
            NumericEdit       TargetWidthPercent_NumericEdit;
            NumericEdit       TargetWidthCentimeters_NumericEdit;
            NumericEdit       TargetWidthInches_NumericEdit;
         HorizontalSizer   DimensionsRow3_Sizer;
            Label             Height_Label;
            NumericEdit       SourceHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPixels_NumericEdit;
            NumericEdit       TargetHeightPercent_NumericEdit;
            NumericEdit       TargetHeightCentimeters_NumericEdit;
            NumericEdit       TargetHeightInches_NumericEdit;
         HorizontalSizer   DimensionsRow4_Sizer;
            Label             SizeInfo_Label;

      SectionBar        Resolution_SectionBar;
      Control           Resolution_Control;
      VerticalSizer     Resolution_Sizer;
         HorizontalSizer   ResolutionRow1_Sizer;
            NumericEdit       HorizontalResolution_NumericEdit;
            NumericEdit       VerticalResolution_NumericEdit;
         HorizontalSizer   ResolutionRow2_Sizer;
            RadioButton       CentimeterUnits_RadioButton;
            RadioButton       InchUnits_RadioButton;
            CheckBox          ForceResolution_CheckBox;

      SectionBar        Mode_SectionBar;
      Control           Mode_Control;
      HorizontalSizer   Mode_Sizer;
         Label             CropMode_Label;
         ComboBox          CropMode_ComboBox;

      SectionBar        FillColor_SectionBar;
      Control           FillColor_Control;
      VerticalSizer     FillColor_Sizer;
         NumericControl    Red_NumericControl;
         NumericControl    Green_NumericControl;
         NumericControl    Blue_NumericControl;
         NumericControl    Alpha_NumericControl;
         Control           ColorSample_Control;

      Bitmap L_Bitmap, R_Bitmap, T_Bitmap, B_Bitmap, TL_Bitmap, TR_Bitmap, BL_Bitmap, BR_Bitmap;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateAnchors();
   void UpdateNumericControls();
   void UpdateFillColorControls();

   void e_ViewSelected( ViewList& sender, View& view );
   void e_ButtonClick( Button& sender, bool checked );
   void e_Anchor_ButtonClick( Button& sender, bool checked );
   void e_ValueUpdated( NumericEdit& sender, double value );
   void e_Width_ValueUpdated( NumericEdit& sender, double value );
   void e_Height_ValueUpdated( NumericEdit& sender, double value );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_Paint( Control& sender, const Rect& updateRect );
   void e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern CropInterface* TheCropInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CropInterface_h

// ----------------------------------------------------------------------------
// EOF CropInterface.h - Released 2025-04-07T08:53:55Z
