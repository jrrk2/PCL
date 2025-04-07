//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 1.3.5
// ----------------------------------------------------------------------------
// StatisticsInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __StatisticsInterface_h
#define __StatisticsInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>
#include <pcl/ViewList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class StatisticsInterface : public ProcessInterface
{
public:

   StatisticsInterface();
   virtual ~StatisticsInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void TrackViewUpdated( bool active ) override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool WantsImageNotifications() const override;
   void ImageUpdated( const View& ) override;
   void ImageFocused( const View& ) override;
   void ImageDeleted( const View& ) override;
   bool WantsViewPropertyNotifications() const override;
   void ViewPropertyDeleted( const View& v, const IsoString& property ) override;
   void SaveSettings() const override;
   void LoadSettings() override;

private:

   bool       m_doCount = true;
   UI64Vector m_count;

   bool       m_doMean = true;
   DVector    m_mean;

   bool       m_doModulus = false;
   DVector    m_modulus;

   bool       m_doNorm = false;
   DVector    m_norm;

   bool       m_doSumOfSquares = false;
   DVector    m_sumOfSquares;

   bool       m_doMeanOfSquares = false;
   DVector    m_meanOfSquares;

   bool       m_doMedian = true;
   DVector    m_median;

   bool       m_doVariance = false;
   DVector    m_variance;

   bool       m_doStdDev = false;
   DVector    m_stdDev;

   bool       m_doAvgDev = true;
   DVector    m_avgDev;

   bool       m_doMAD = true;
   DVector    m_MAD;

   bool       m_doBWMV = false;
   DVector    m_BWMV;

   bool       m_doPBMV = false;
   DVector    m_PBMV;

   bool       m_doSn = false;
   DVector    m_Sn;

   bool       m_doQn = false;
   DVector    m_Qn;

   bool       m_doMinimum = true;
   DVector    m_minimum;

   bool       m_doMinimumPos = false;
   IMatrix    m_minimumPos;

   bool       m_doMaximum = true;
   DVector    m_maximum;

   bool       m_doMaximumPos = false;
   IMatrix    m_maximumPos;

   int        m_rangeBits = 0; // 0=normalized

   struct GUIData
   {
      GUIData( StatisticsInterface& );

      VerticalSizer  Global_Sizer;
         ViewList       AllViews_ViewList;
         HorizontalSizer   Format_Sizer;
            ComboBox          Range_ComboBox;
            CheckBox          Scientific_CheckBox;
            CheckBox          Normalized_CheckBox;
            CheckBox          Unclipped_CheckBox;
            ToolButton        ToText_ToolButton;
            ToolButton        Options_ToolButton;
         TreeBox        Data_TreeBox;
   };

   GUIData* GUI = nullptr;

   bool ViewPropertyRequired( const IsoString& property ) const;

   String ToText() const;

   void CalculateStatistics();
   void UpdateControls();

   void __ViewList_ViewSelected( ViewList& sender, View& view );
   void __Button_Click( Button& sender, bool checked );
   void __ComboBox_ItemSelected( ComboBox& sender, int itemIndex );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
   friend class StatisticsOptionsDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern StatisticsInterface* TheStatisticsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StatisticsInterface_h

// ----------------------------------------------------------------------------
// EOF StatisticsInterface.h - Released 2025-04-07T08:53:56Z
