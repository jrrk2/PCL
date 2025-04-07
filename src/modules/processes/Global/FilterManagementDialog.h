//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// FilterManagementDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FilterManagementDialog_h
#define __FilterManagementDialog_h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FilterManagementDialog : public Dialog
{
public:

   enum working_mode { ImportXML, ExportXML, ImportCSV, ExportCSV, MergeWithDefault, Reset };

   FilterManagementDialog();

   working_mode Mode() const
   {
      return m_mode;
   }

   String Path() const
   {
      switch ( m_mode )
      {
      case ImportXML:
         return m_inputXMLFilePath;
      case ExportXML:
         return m_outputXMLFilePath;
      case ImportCSV:
         return m_inputCSVDirectory;
      case ExportCSV:
         return m_outputCSVDirectory;
      default: // ?!
         return String();
      }
   }

   bool Merge() const
   {
      return m_merge;
   }

private:

   working_mode m_mode = ImportCSV;
   String       m_outputXMLFilePath;
   String       m_outputCSVDirectory;
   String       m_inputXMLFilePath;
   String       m_inputCSVDirectory;
   bool         m_merge = true;

   VerticalSizer     Global_Sizer;
      HorizontalSizer   ImportXMLFiltersDatabase_Sizer;
         RadioButton       ImportXMLFiltersDatabase_RadioButton;
      HorizontalSizer   ImportCSVFilterDefinitions_Sizer;
         RadioButton       ImportCSVFilterDefinitions_RadioButton;
      HorizontalSizer   InputPath_Sizer;
         Label             InputPath_Label;
         Edit              InputPath_Edit;
         ToolButton        InputPath_ToolButton;
      HorizontalSizer   ExportXMLFiltersDatabase_Sizer;
         RadioButton       ExportXMLFiltersDatabase_RadioButton;
      HorizontalSizer   ExportCSVFilterDefinitions_Sizer;
         RadioButton       ExportCSVFilterDefinitions_RadioButton;
         CheckBox          Merge_CheckBox;
      HorizontalSizer   OutputPath_Sizer;
         Label             OutputPath_Label;
         Edit              OutputPath_Edit;
         ToolButton        OutputPath_ToolButton;
      HorizontalSizer   MergeWithDefaultFilters_Sizer;
         RadioButton       MergeWithDefaultFilters_RadioButton;
      HorizontalSizer   ResetFilters_Sizer;
         RadioButton       ResetFilters_RadioButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void UpdateControls();

   void e_Click( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FilterManagementDialog_h

// ----------------------------------------------------------------------------
// EOF FilterManagementDialog.h - Released 2025-04-07T08:53:56Z
