//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// ArcsinhStretchInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2021 Mark Shelley
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ArcsinhStretchInterface_h
#define __ArcsinhStretchInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Frame.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "ArcsinhStretchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ArcsinhStretchInterface : public ProcessInterface
{
public:

   ArcsinhStretchInterface();
   virtual ~ArcsinhStretchInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool WantsRealTimePreviewNotifications() const override;
   void RealTimePreviewOwnerChanged(ProcessInterface& iface) override;
   void RealTimePreviewUpdated(bool active) override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate(const UInt16Image&, const View&, const Rect&, int zoomLevel) const override;
   bool GenerateRealTimePreview(UInt16Image&, const View&, const Rect&, int zoomLevel, String& info) const override;

private:

   ArcsinhStretchInstance instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const ArcsinhStretchInstance& );

      void Run() override;

   private:

      ArcsinhStretchInstance m_instance;
   };

   struct GUIData
   {
      GUIData( ArcsinhStretchInterface& );

      VerticalSizer     Global_Sizer;
         NumericControl    Stretch_NumericControl;
         VerticalSizer     BlackPoint_Sizer;
            NumericControl    BlackPoint_NumericControl;
            HorizontalSizer   FineAdjust_Sizer;
               Slider            FineAdjust_Slider;
         HorizontalSizer   ParameterProtectHighlights_Sizer;
            CheckBox          ParameterProtectHighlights_CheckBox;
         HorizontalSizer   ParameterUseRgbws_Sizer;
            CheckBox          ParameterUseRgbws_CheckBox;
         GroupBox          PreviewControlsGroupBox;
         HorizontalSizer   PreviewControls_Sizer;
            PushButton        EstimateBlackPoint_Button;
            CheckBox          ParameterPreviewClipped_CheckBox;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;
   mutable AutoPointer<RealTimeThread> m_realTimeThread;
   mutable double m_preview_blackpoint_level = 0;

   void UpdateControls();
   void UpdateSliderControls();
   void UpdateRealTimePreview();

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __SliderValueUpdated( Slider& sender, int value );
   void __ItemClicked( Button& sender, bool checked );
   void __UpdateRealTimePreview_Timer( Timer& );
   void __FineAdjustSliderEnter( Control& sender );
   void __FineAdjustSliderLeave( Control& sender );
   void __FineAdjustSliderGetFocus( Control& sender );
   void __FineAdjustSliderLoseFocus( Control& sender );
   void __FineAdjustSliderMousePress( Control& sender, const pcl::Point &pos, int button, unsigned buttons, unsigned modifiers );
   void __FineAdjustSliderMouseRelease( Control& sender, const pcl::Point &pos, int button, unsigned buttons, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ArcsinhStretchInterface* TheArcsinhStretchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ArcsinhStretchInterface_h

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchInterface.h - Released 2025-04-07T08:53:56Z
