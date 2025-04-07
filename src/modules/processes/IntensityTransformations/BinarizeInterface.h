//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// BinarizeInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BinarizeInterface_h
#define __BinarizeInterface_h

#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "BinarizeInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class BinarizeInterface : public ProcessInterface
{
public:

   BinarizeInterface();
   virtual ~BinarizeInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void RealTimePreviewUpdated( bool active ) override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, const Rect&, int zoomLevel ) const override;
   bool GenerateRealTimePreview( UInt16Image&, const View&, const Rect&, int zoomLevel, String& info ) const override;
   bool WantsReadoutNotifications() const override;
   void UpdateReadout( const View&, const pcl::DPoint&, double R, double G, double B, double A ) override;

private:

   BinarizeInstance m_instance;

   class RealTimeThread : public Thread
   {
   public:

      UInt16Image m_image;

      RealTimeThread();

      void Reset( const UInt16Image&, const BinarizeInstance& );

      void Run() override;

   private:

      BinarizeInstance m_instance;
   };

   mutable RealTimeThread* m_realTimeThread = nullptr;

   struct GUIData
   {
      GUIData( BinarizeInterface& );

      VerticalSizer     Global_Sizer;
         GroupBox        Parameters_GroupBox;
         VerticalSizer   Parameters_Sizer;
            HorizontalSizer   Mode_Sizer;
               RadioButton       GlobalK_RadioButton;
               RadioButton       RGB_RadioButton;
            HorizontalSizer   Threshold_Sizer;
               VerticalSizer     Level_Sizer;
                  NumericControl    V0_NumericControl;
                  NumericControl    V1_NumericControl;
                  NumericControl    V2_NumericControl;
               Control           ColorSample_Control;

      Timer UpdateRealTimePreview_Timer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateValues( Edit&, Slider&, double );
   void UpdateRealTimePreview();

   void __Mode_Click( Button& sender, bool checked );
   void __LevelValues_ValueUpdated( NumericEdit& sender, double value );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __UpdateRealTimePreview_Timer( Timer& );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BinarizeInterface* TheBinarizeInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BinarizeInterface_h

// ----------------------------------------------------------------------------
// EOF BinarizeInterface.h - Released 2025-04-07T08:53:56Z
