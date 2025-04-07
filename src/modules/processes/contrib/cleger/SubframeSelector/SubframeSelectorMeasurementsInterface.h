//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 1.9.2
// ----------------------------------------------------------------------------
// SubframeSelectorMeasurementsInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2025 Cameron Leger
// Copyright (c) 2020-2025 Juan Conejero, PTeam
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef e_SubframeSelectorMeasurementsInterface_h
#define e_SubframeSelectorMeasurementsInterface_h

#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ToolButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/TreeBox.h>

#include "GraphWebView.h"
#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class MeasurementNode;

class SubframeSelectorMeasurementsInterface : public ProcessInterface
{
public:

   SubframeSelectorMeasurementsInterface( SubframeSelectorInstance& );
   virtual ~SubframeSelectorMeasurementsInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   using ProcessInterface::Launch;

   void Cleanup();

private:

   SubframeSelectorInstance& m_instance;

   struct GUIData
   {
      GUIData( SubframeSelectorMeasurementsInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        MeasurementTable_SectionBar;
      Control           MeasurementTable_Control;
      VerticalSizer     MeasurementTable_Sizer;
         HorizontalSizer   MeasurementsTable_Top_Sizer;
            VerticalSizer     MeasurementsTable_TopLeft_Sizer;
               HorizontalSizer   MeasurementsTable_TopLeft1_Sizer;
                  ComboBox          MeasurementsTable_SortingProperty_ComboBox;
                  ComboBox          MeasurementsTable_SortingMode_ComboBox;
               HorizontalSizer   MeasurementsTable_TopLeft2_Sizer;
                  Label             MeasurementsTable_Quantities_Label;
            VerticalSizer     MeasurementsTable_TopRight_Sizer;
               HorizontalSizer   MeasurementsTable_TopRight1_Sizer;
                  ToolButton        MeasurementsTable_ToggleApproved_ToolButton;
                  ToolButton        MeasurementsTable_ToggleLocked_ToolButton;
                  ToolButton        MeasurementsTable_Invert_ToolButton;
               HorizontalSizer   MeasurementsTable_TopRight2_Sizer;
                  ToolButton        MeasurementsTable_Remove_ToolButton;
                  ToolButton        MeasurementsTable_Reset_ToolButton;
                  ToolButton        MeasurementsTable_ExportCSV_ToolButton;
         TreeBox           MeasurementTable_TreeBox;

      SectionBar        MeasurementGraph_SectionBar;
      Control           MeasurementGraph_Control;
      VerticalSizer     MeasurementGraph_Sizer;
         HorizontalSizer   MeasurementGraph_Top_Sizer;
            ComboBox          MeasurementGraph_Property_ComboBox;
            ComboBox          MeasurementGraph_AuxProperty_ComboBox;
            ToolButton        MeasurementGraph_ExportPDF_ToolButton;
         HorizontalSizer   MeasurementGraph_Bottom_Sizer;
            GraphWebView      MeasurementGraph_WebView;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateMeasurementQuantity();
   void UpdateMeasurementNode( MeasurementNode* );
   void UpdateMeasurementTable();
   void UpdateMeasurementGraph();

   MeasurementNode* MeasurementNodeByItem( const MeasureItem* ) const;
   MeasureItem* MeasureItemByGraphIndex( int index ) const;

   void ExportCSV() const;
   void ExportPDF() const;

   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_ButtonClick( Button& sender, bool checked );
   void e_GraphApprove( GraphWebView &sender, int measureIndex );
   void e_GraphUnlock( GraphWebView &sender, int measureIndex );
   void e_ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
   friend class SubframeSelectorInstance;
   friend class SubframeSelectorInterface;
   friend class SubframeSelectorExpressionsInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorMeasurementsInterface* TheSubframeSelectorMeasurementsInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorMeasurementsInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasurementsInterface.h - Released 2025-04-07T08:53:56Z
