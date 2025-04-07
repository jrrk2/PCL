//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AdaptiveStretchInterface_h
#define __AdaptiveStretchInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "ExponentialNumericControl.h"
#include "AdaptiveStretchInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class AdaptiveStretchInterface : public ProcessInterface
{
public:

   AdaptiveStretchInterface();
   virtual ~AdaptiveStretchInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;

private:

   AdaptiveStretchInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image  m_image;
      StretchCurve m_curve;

      RealTimeThread( const View& );

      void Reset( const UInt16Image&, const AdaptiveStretchInstance& );

      void Run() override;

   private:

      AdaptiveStretchInstance m_instance;
      const View&             m_view;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( AdaptiveStretchInterface& );

      VerticalSizer  Global_Sizer;

         ExponentialNumericControl  NoiseThreshold_NumericControl;
         HorizontalSizer            Protection_Sizer;
            ExponentialNumericControl  Protection_NumericControl;
            CheckBox                   UseProtection_CheckBox;
         NumericEdit                MaxCurvePoints_NumericEdit;
         HorizontalSizer            PlotCurveGraph_Sizer;
            CheckBox                   PlotCurveGraph_CheckBox;

         SectionBar                 ROI_SectionBar;
         Control                    ROI_Control;
         VerticalSizer              ROI_Sizer;
            HorizontalSizer            ROIRow1_Sizer;
               NumericEdit                ROIX0_NumericEdit;
               NumericEdit                ROIY0_NumericEdit;
            HorizontalSizer            ROIRow2_Sizer;
               NumericEdit                ROIWidth_NumericEdit;
               NumericEdit                ROIHeight_NumericEdit;
               PushButton                 ROISelectPreview_Button;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateRealTimePreview();
   void OpenCurveGraph() const;
   void CloseCurveGraph() const;
   void UpdateCurveGraph( const StretchCurve& ) const;

   void __ValueUpdated( NumericEdit& sender, double value );
   void __Click( Button& sender, bool checked );
   void __SectionBarCheck( SectionBar& sender, bool checked );
   void __Hide( Control& );
   void __UpdateRealTimePreview_Timer( Timer& );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AdaptiveStretchInterface* TheAdaptiveStretchInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveStretchInterface_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchInterface.h - Released 2025-04-07T08:53:56Z
