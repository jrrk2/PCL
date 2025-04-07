//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// AdaptiveStretchCurveGraphInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __AdaptiveStretchCurveGraphInterface_h
#define __AdaptiveStretchCurveGraphInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Control.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

#include "AdaptiveStretchInstance.h" // for StretchCurve declaration

namespace pcl
{

// ----------------------------------------------------------------------------

class AdaptiveStretchCurveGraphInterface : public ProcessInterface
{
public:

   AdaptiveStretchCurveGraphInterface();
   virtual ~AdaptiveStretchCurveGraphInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   using ProcessInterface::Launch;
   void SaveSettings() const override;
   void LoadSettings() override;

   void UpdateGraph( const StretchCurve& curve );

private:

   struct GUIData
   {
      GUIData( AdaptiveStretchCurveGraphInterface& );

      VerticalSizer     Global_Sizer;
         Control           CurveGraph_Control;
         HorizontalSizer   Options_Sizer;
            ComboBox          Size_ComboBox;
            ToolButton        Render_ToolButton;
            ToolButton        Edit_ToolButton;
   };

   GUIData* GUI = nullptr;

   /*
    * Parameters
    */
   int    m_width           = 400;
   int    m_height          = 400;
   RGBA   m_backgroundColor = 0xFFFFFFFF; // white
   RGBA   m_curveColor      = 0xFFFF0000; // red
   RGBA   m_gridColor       = 0xFFD0D0D0; // light gray
   RGBA   m_axisColor       = 0xFF000000; // black
   String m_fontFace        = "Helvetica";
   int    m_fontSize        = 12;
   int    m_tickSize        = 5;
   int    m_margin          = 15;

   /*
    * Working data
    */
   StretchCurve m_curve;
   Rect         m_curveRect   = 0;
   Bitmap       m_gridBitmap  = Bitmap::Null();
   Bitmap       m_curveBitmap = Bitmap::Null();

   void UpdateControls();
   void Resize( int width, int height );

   void GenerateGraphGrid();
   void GenerateGraphCurve();

   void __Paint( Control& sender, const pcl::Rect& updateRect );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __Click( Button& sender, bool checked );

   void __Hide( Control& );
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AdaptiveStretchCurveGraphInterface* TheAdaptiveStretchCurveGraphInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AdaptiveStretchCurveGraphInterface_h

// ----------------------------------------------------------------------------
// EOF AdaptiveStretchCurveGraphInterface.h - Released 2025-04-07T08:53:56Z
