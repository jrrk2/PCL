//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ScreenTransferFunctionInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ScreenTransferFunctionInterface_h
#define __ScreenTransferFunctionInterface_h

#include <pcl/Atomic.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "ScreenTransferFunctionInstance.h"
#include "STFSliders.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class STFAutoStretchDialog;

class ScreenTransferFunctionInterface : public ProcessInterface
{
public:

   enum working_mode { EditMode, ZoomInMode, ZoomOutMode, PanMode };
   enum readout_mode { NoReadout, BlackPointReadout, MidtonesReadout, WhitePointReadout };

   ScreenTransferFunctionInterface();
   virtual ~ScreenTransferFunctionInterface();

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
   void ImageFocused( const View& ) override;
   void ImageUpdated( const View& ) override;
   void ImageSTFEnabled( const View& ) override;
   void ImageSTFDisabled( const View& ) override;
   void ImageSTFUpdated( const View& ) override;
   void ImageRenamed( const View& ) override;
   void ImageDeleted( const View& ) override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const DPoint&, double R, double G, double B, double A ) override;

   working_mode WorkingMode() const
   {
      return m_mode;
   }

   void SetWorkingMode( working_mode );

   void LinkRGBChannels( bool linked );

   void ComputeAutoStretch( View&, bool boost = false );

   void ApplyTo( View& ) const;
   void ApplyToCurrentView() const;

private:

   ScreenTransferFunctionInstance m_instance;
   working_mode                   m_mode             = EditMode;
   readout_mode                   m_readoutMode      = NoReadout;
   bool                           m_rgbLinked        = true;
   STFAutoStretchDialog*          m_autoAdjustDialog = nullptr;

   PCL_CLASS_REENTRANCY_GUARD

   struct GUIData
   {
      GUIData( ScreenTransferFunctionInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   STF_Sizer[ 4 ];
            ToolButton        LinkRGB_ToolButton;
            ToolButton        Zoom11_ToolButton;
            ToolButton        Auto_ToolButton;
            ToolButton        Edit_ToolButton;
            ToolButton        EditMode_ToolButton;
            ToolButton        ZoomInMode_ToolButton;
            ToolButton        ZoomOutMode_ToolButton;
            ToolButton        PanMode_ToolButton;
            Label             ChannelId_Label[ 4 ];
            STFSliders        Sliders_Control[ 4 ];
            ToolButton        Reset_ToolButton[ 4 ];
            ToolButton        ReadoutShadows_ToolButton;
            ToolButton        ReadoutMidtones_ToolButton;
            ToolButton        ReadoutHighlights_ToolButton;
            ToolButton        STFEnabled_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateTitle( const View& );
   void UpdateControls();
   void UpdateFirstChannelLabelText();
   void UpdateSTFEnabledButton( const View& );

   void __SliderValueUpdated( STFSliders& sender, int channel, int item,
                              double value, unsigned modifiers, bool final );

   void __SliderRangeUpdated( STFSliders& sender, int channel,
                              double v0, double v1, unsigned modifiers );

   void __ModeButtonClick( Button& sender, bool checked );
   void __ResetButtonClick( Button& sender, bool checked );
   void __ReadoutButtonClick( Button& sender, bool checked );
   void __STFEnabledButtonClick( Button& sender, bool checked );

   void __STFAutoStretch_MouseRelease( Control& sender, const pcl::Point& pos,
                                       int button, unsigned buttons, unsigned modifiers );

   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ScreenTransferFunctionInterface* TheScreenTransferFunctionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ScreenTransferFunctionInterface_h

// ----------------------------------------------------------------------------
// EOF ScreenTransferFunctionInterface.h - Released 2025-04-07T08:53:56Z
