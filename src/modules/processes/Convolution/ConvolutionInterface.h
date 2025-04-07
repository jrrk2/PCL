//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// ConvolutionInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ConvolutionInterface_h
#define __ConvolutionInterface_h

#include <pcl/Button.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TabBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>
#include <pcl/ToolButton.h>

#include "ConvolutionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ConvolutionInterface : public ProcessInterface
{
public:

   ConvolutionInterface();
   virtual ~ConvolutionInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   void RealTimePreviewUpdated( bool active ) override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;
   void SaveSettings() const override;
   void LoadSettings() override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageRenamed( const View& ) override;
   void ImageDeleted( const View& ) override;

private:

   ConvolutionInstance instance;

   class RealTimeThread : public Thread
   {
   public:

      Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const ConvolutionInstance&, int );

      void Run() override;

   private:

      ConvolutionInstance m_instance;
      int                 m_zoomLevel;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( ConvolutionInterface& );

      VerticalSizer  Global_Sizer;
         HorizontalSizer     Filter_Sizer;
            TabBox               FilterMode_TabBox;

               Control              Parametric_Control;
               VerticalSizer        Parametric_Sizer;
                  HorizontalSizer   Sigma_Sizer;
                     NumericEdit       Sigma_NumericEdit;
                     VerticalSizer     SigmaSliders_Sizer;
                        Slider            SigmaCoarse_Slider;
                        Slider            SigmaFine_Slider;
                  NumericControl       Shape_NumericControl;
                  NumericControl       AspectRatio_NumericControl;
                  NumericControl       RotationAngle_NumericControl;

               Control           Library_Control;
               VerticalSizer     Library_Sizer;
                  Label             LibraryPath_Label;
                  HorizontalSizer   LibraryPath_Sizer;
                     Edit              LibraryPath_Edit;
                     ToolButton        LibraryPath_ToolButton;
                  HorizontalSizer   LibraryButtons_Sizer;
                     PushButton        SaveAsLibrary_PushButton;
                     PushButton        NewLibrary_PushButton;
                     PushButton        DefaultLibrary_PushButton;
                  ComboBox          Filter_ComboBox;
                  CheckBox          RescaleHighPass_CheckBox;
                  HorizontalSizer   FilterButtons_Sizer;
                     PushButton        ViewFilterElements_PushButton;
                     PushButton        EditFilter_PushButton;
                     PushButton        NewFilter_PushButton;
                     PushButton        RemoveFilter_PushButton;

               Control              Image_Control;
               VerticalSizer        Image_Sizer;
                  Label                View_Label;
                  HorizontalSizer      View_Sizer;
                     Edit                 View_Edit;
                     ToolButton           View_ToolButton;

            VerticalSizer        FilterThumbnail_Sizer;
               Control              FilterThumbnail_Control;
               HorizontalSizer      FilterInfo_Sizer;
                  Label                FilterInfo_Label;
                  ToolButton           RenderFilter_ToolButton;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateCurrentModeControls();
   void UpdateParametricFilterControls();
   void UpdateLibraryFilterControls();
   void UpdateImageFilterControls();
   void UpdateRealTimePreview();
   void RegenerateFiltersComboBox( int selectedItemIndex );
   void RegenerateFiltersComboBox( const String& selectedFilterName );

   void __Filter_PageSelected( TabBox& sender, int pageIndex );
   void __Filter_ValueUpdated( NumericEdit& sender, double value );
   void __Filter_SliderUpdated( Slider& sender, int value );
   void __Filter_Paint( Control& sender, const Rect& updateRect );
   //void __Library_EditCompleted( Edit& sender );
   void __Library_ItemSelected( ComboBox& sender, int itemIndex );
   void __Library_Click( Button& sender, bool checked );
   void __Image_Click( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ConvolutionInterface* TheConvolutionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ConvolutionInterface_h

// ----------------------------------------------------------------------------
// EOF ConvolutionInterface.h - Released 2025-04-07T08:53:55Z
