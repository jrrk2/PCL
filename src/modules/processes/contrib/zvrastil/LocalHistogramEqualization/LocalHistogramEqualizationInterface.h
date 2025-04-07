//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard LocalHistogramEqualization Process Module Version 1.0.0
// ----------------------------------------------------------------------------
// LocalHistogramEqualizationInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard LocalHistogramEqualization PixInsight module.
//
// Copyright (c) 2011-2021 Zbynek Vrastil
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __LocalHistogramEqualizationInterface_h
#define __LocalHistogramEqualizationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "LocalHistogramEqualizationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class LocalHistogramEqualizationInterface : public ProcessInterface
{
public:

   LocalHistogramEqualizationInterface();
   virtual ~LocalHistogramEqualizationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;

private:

   LocalHistogramEqualizationInstance instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const LocalHistogramEqualizationInstance& );

      void Run() override;

   private:

      LocalHistogramEqualizationInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( LocalHistogramEqualizationInterface& );

      VerticalSizer Global_Sizer;
      NumericControl Radius_NumericControl;
      HorizontalSizer HistogramBins_Sizer;
      Label HistogramBins_Label;
      ComboBox HistogramBins_ComboBox;
      NumericControl SlopeLimit_NumericControl;
      NumericControl Amount_NumericControl;
      HorizontalSizer CircularKernel_Sizer;
      CheckBox CircularKernel_CheckBox;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateRealTimePreview();

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __ItemClicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern LocalHistogramEqualizationInterface* TheLocalHistogramEqualizationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __LocalHistogramEqualizationInterface_h

// ----------------------------------------------------------------------------
// EOF LocalHistogramEqualizationInterface.h - Released 2025-04-07T08:53:56Z
