//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 1.2.2
// ----------------------------------------------------------------------------
// RGBWorkingSpaceInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RGBWorkingSpaceInterface_h
#define __RGBWorkingSpaceInterface_h

#include <pcl/CheckBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ViewList.h>

#include "RGBWorkingSpaceInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RGBWorkingSpaceInterface : public ProcessInterface
{
public:

   RGBWorkingSpaceInterface();
   virtual ~RGBWorkingSpaceInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void TrackViewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& v ) override;
   void ImageFocused( const View& v ) override;
   void ImageRGBWSUpdated( const View& v ) override;
   bool WantsGlobalNotifications() const override;
   void GlobalRGBWSUpdated() override;

private:

   RGBWorkingSpaceInstance m_instance;

   struct GUIData
   {
      GUIData( RGBWorkingSpaceInterface& );

      VerticalSizer     Global_Sizer;

      ViewList          AllImages_ViewList;

      SectionBar        LuminanceCoefficients_SectionBar;
      Control           LuminanceCoefficients_Control;
      VerticalSizer     LuminanceCoefficients_Sizer;
         NumericControl    RY_NumericControl;
         NumericControl    GY_NumericControl;
         NumericControl    BY_NumericControl;

      SectionBar        ChromaticityCoordinates_SectionBar;
      Control           ChromaticityCoordinates_Control;
      VerticalSizer     ChromaticityCoordinates_Sizer;
         HorizontalSizer   xyTitle_Sizer;
            Label             RxyTitle_Label;
            Label             GxyTitle_Label;
            Label             BxyTitle_Label;
         HorizontalSizer   xValues_Sizer;
            Label             x_Label;
            NumericEdit       Rx_NumericEdit;
            NumericEdit       Gx_NumericEdit;
            NumericEdit       Bx_NumericEdit;
         HorizontalSizer   yValues_Sizer;
            Label             y_Label;
            NumericEdit       Ry_NumericEdit;
            NumericEdit       Gy_NumericEdit;
            NumericEdit       By_NumericEdit;

      SectionBar        Gamma_SectionBar;
      Control           Gamma_Control;
      VerticalSizer     Gamma_Sizer;
         NumericControl    Gamma_NumericControl;
         HorizontalSizer      sRGBGamma_Sizer;
            CheckBox          sRGBGamma_CheckBox;

      Control           BottomSectionSeparator_Control;

      HorizontalSizer   LoadDefaultRGBWS_Sizer;
         PushButton        LoadDefaultRGBWS_PushButton;
         PushButton        NormalizeY_PushButton;

      HorizontalSizer   LoadGlobalRGBWS_Sizer;
         PushButton        LoadGlobalRGBWS_PushButton;
         CheckBox          ApplyGlobalRGBWS_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __LuminanceCoefficient_ValueUpdated( NumericEdit& sender, double value );
   void __ChromaticityCoordinate_ValueUpdated( NumericEdit& sender, double value );
   void __Gamma_ValueUpdated( NumericEdit& sender, double value );
   void __NormalizeY_ButtonClick( Button& sender, bool checked );
   void __sRGBGamma_ButtonClick( Button& sender, bool checked );
   void __LoadRGBWS_ButtonClick( Button& sender, bool checked );
   void __ApplyGlobalRGBWS_ButtonClick( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RGBWorkingSpaceInterface* TheRGBWorkingSpaceInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RGBWorkingSpaceInterface_h

// ----------------------------------------------------------------------------
// EOF RGBWorkingSpaceInterface.h - Released 2025-04-07T08:53:55Z
