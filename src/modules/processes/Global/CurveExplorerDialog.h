//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// CurveExplorerDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __CurveExplorerDialog_h
#define __CurveExplorerDialog_h

#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/FilterManager.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>
#include <pcl/WebView.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CurveExplorerDialog : public Dialog
{
public:

   CurveExplorerDialog();

   bool MadeChanges() const
   {
      return m_madeChanges;
   }

   void ReloadData();

private:

   FilterManager           m_manager;
   SampledSpectrumDataList m_filters;
   SampledSpectrumDataList m_whiteReferences;
   bool                    m_workingWithFilters = true;
   bool                    m_modified = false;
   bool                    m_madeChanges = false;
   StringList              m_htmlFiles;
   bool                    m_splitterPressed = false;
   int                     m_splitterPos = 0;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   CurveType_Sizer;
         Label             CurveType_Label;
         ComboBox          CurveType_ComboBox;
      TreeBox           Curves_TreeBox;
      Control           VertSplitter_Control;
      WebView           Graph_WebView;
      HorizontalSizer   Buttons_Sizer;
         ToolButton        ExportPDF_ToolButton;
         ToolButton        CombineFilters_ToolButton;
         PushButton        OK_PushButton;

   void Update();
   void UpdateCurves();
   void UpdateGraph();
   void CombineFilters();
   void ExportPDF();

   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Click( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void e_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers );
   void e_LoadFinished( WebView& sender, bool state );
   void e_Close( Control& sender, bool& allowClose );
   void e_Show( Control& sender );
   void e_Hide( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CurveExplorerDialog_h

// ----------------------------------------------------------------------------
// EOF CurveExplorerDialog.h - Released 2025-04-07T08:53:56Z
