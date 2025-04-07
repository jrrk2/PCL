//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// ExponentialTransformationInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __ExponentialTransformationInterface_h
#define __ExponentialTransformationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>
#include <pcl/View.h>

#include "ExponentialTransformationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ExponentialTransformationInterface : public ProcessInterface
{
public:

   ExponentialTransformationInterface();
   virtual ~ExponentialTransformationInterface();

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

   ExponentialTransformationInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const ExponentialTransformationInstance& );

      void Run() override;

   private:

      ExponentialTransformationInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( ExponentialTransformationInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   Function_Sizer;
            Label             Function_Label;
            ComboBox          Function_ComboBox;
         NumericControl    Order_NumericControl;
         NumericControl    Smoothing_NumericControl;
         HorizontalSizer   LightnessMask_Sizer;
            CheckBox          LightnessMask_CheckBox;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateRealTimePreview();

   void __Function_ItemSelected( ComboBox& sender, int itemIndex );
   void __FunctionParameter_ValueUpdated( NumericEdit& sender, double value );
   void __LightnessMask_ButtonClick( Button&, bool );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ExponentialTransformationInterface* TheExponentialTransformationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExponentialTransformationInterface_h

// ----------------------------------------------------------------------------
// EOF ExponentialTransformationInterface.h - Released 2025-04-07T08:53:56Z
