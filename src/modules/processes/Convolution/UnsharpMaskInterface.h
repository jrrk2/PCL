//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 1.1.3
// ----------------------------------------------------------------------------
// UnsharpMaskInterface.h - Released 2025-04-07T08:53:55Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __UnsharpMaskInterface_h
#define __UnsharpMaskInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Control.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "UnsharpMaskInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class UnsharpMaskInterface : public ProcessInterface
{
public:

   UnsharpMaskInterface();
   virtual ~UnsharpMaskInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;

private:

   UnsharpMaskInstance instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const UnsharpMaskInstance& );

      void Run() override;

   private:

      UnsharpMaskInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( UnsharpMaskInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Filter_SectionBar;
      Control           Filter_Control;
      VerticalSizer     Filter_Sizer;
         HorizontalSizer   Sigma_Sizer;
            NumericEdit       Sigma_NumericEdit;
            VerticalSizer     SigmaSliders_Sizer;
               Slider            SigmaCoarse_Slider;
               Slider            SigmaFine_Slider;
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

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateFilterControls();
   void UpdateDeringingControls();
   void UpdateRangeControls();
   void UpdateRealTimePreview();

   void __Filter_ValueUpdated( NumericEdit& sender, double value );
   void __Filter_SliderUpdated( Slider& sender, int value );
   void __Target_ItemSelected( ComboBox& sender, int itemIndex );
   void __Deringing_Check( SectionBar& sender, bool checked );
   void __Deringing_ValueUpdated( NumericEdit& sender, double value );
   void __Deringing_Click( Button& sender, bool checked );
   void __Range_ValueUpdated( NumericEdit& sender, double value );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern UnsharpMaskInterface* TheUnsharpMaskInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __UnsharpMaskInterface_h

// ----------------------------------------------------------------------------
// EOF UnsharpMaskInterface.h - Released 2025-04-07T08:53:55Z
