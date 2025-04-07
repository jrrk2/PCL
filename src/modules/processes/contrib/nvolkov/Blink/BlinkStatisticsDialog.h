//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 1.2.5
// ----------------------------------------------------------------------------
// BlinkStatisticsDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __BlinkStatisticsDialog_h
#define __BlinkStatisticsDialog_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Dialog.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/Rectangle.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/StringList.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class BlinkInterface;

class BlinkStatisticsDialog : public Dialog
{
public:

   BlinkStatisticsDialog( BlinkInterface* parent );

private:

   VerticalSizer     Global_Sizer;

   ComboBox          File_ComboBox;
   TreeBox           Keyword_TreeBox;

   HorizontalSizer   StatPreferencesLine1_Sizer;
      RadioButton       Range0_RadioButton;
      RadioButton       Range1_RadioButton;
      Label             Digits_Label;
      SpinBox           Digits0_SpinBox;
      SpinBox           Digits1_SpinBox;
      CheckBox          SortChannels_CheckBox;
      CheckBox          StatCropMode_CheckBox;
      CheckBox          WriteStatsFile_CheckBox;

   HorizontalSizer   StatPreferencesLine2_Sizer;
      Label             OutputDir_Label;
      Edit              OutputDir_Edit;
      ToolButton        OutputDir_SelectButton;

   HorizontalSizer   Buttons_Sizer;
      PushButton        OK_PushButton;
      PushButton        Cancel_PushButton;

   BlinkInterface*   m_parent = nullptr; // parent interface
   Rect              m_rect = 0;         // CurrentView Rectangle
   Array<StringList> m_stat;             // All image statistics incuding header in String format
   String            m_outputDir;        // folder for Statistics file

   void        Init();
   StringList  FindKeywords( int fileIndex );
   void        ConvertStatisticsToText();
   void        PrepareData();
   void        ExecuteStatistics();
   void        SelectOutputDir();

   void        __RangeMode_Click( Button& sender, bool checked );
   void        __TreeBox_ItemSelected( ComboBox& sender, int itemIndex );
   void        __Button_Click( Button& sender, bool checked );
   void        __Control_DoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void        __Edit_Completed( Edit& sender );
   void        __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BlinkStatisticsDialog_h

// ----------------------------------------------------------------------------
// EOF BlinkStatisticsDialog.h - Released 2025-04-07T08:53:56Z
