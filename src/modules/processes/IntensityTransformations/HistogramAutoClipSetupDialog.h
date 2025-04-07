//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// HistogramAutoClipSetupDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __HistogramAutoClipSetupDialog_h
#define __HistogramAutoClipSetupDialog_h

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/PushButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class HistogramAutoClipSetupDialog : public Dialog
{
public:

   HistogramAutoClipSetupDialog();

private:

   double   m_shadowsAutoClipping;    // fraction of total pixels
   double   m_highlightsAutoClipping; //

   VerticalSizer     Global_Sizer;
      GroupBox          ClippingParameters_GroupBox;
      VerticalSizer     ClippingParameters_Sizer;
         NumericControl    ShadowsAmount_NumericControl;
         NumericControl    HighlightsAmount_NumericControl;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __ClippingAmount_ValueUpdated( NumericEdit& sender, double value );
   void __Button_Click( Button& sender, bool checked );
   void __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HistogramAutoClipSetupDialog_h

// ----------------------------------------------------------------------------
// EOF HistogramAutoClipSetupDialog.h - Released 2025-04-07T08:53:56Z
