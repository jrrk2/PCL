//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 1.0.6
// ----------------------------------------------------------------------------
// SplitCFAInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2021 Nikolay Volkov
// Copyright (c) 2003-2021 Pleiades Astrophoto S.L.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SplitCFAInterface_h
#define __SplitCFAInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "SplitCFAInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SplitCFAInterface : public ProcessInterface
{
public:

   SplitCFAInterface();
   virtual ~SplitCFAInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   SplitCFAInstance m_instance;

   struct GUIData
   {
      GUIData( SplitCFAInterface& );

      VerticalSizer Global_Sizer;

      SectionBar TargetImages_SectionBar;
      Control TargetImages_Control;
      HorizontalSizer TargetImages_Sizer;
      TreeBox Files_TreeBox;
      VerticalSizer TargetButtons_Sizer;
      PushButton AddFolder_PushButton;
      PushButton AddFiles_PushButton;
      PushButton SelectAll_PushButton;
      PushButton InvertSelection_PushButton;
      PushButton ToggleSelected_PushButton;
      PushButton RemoveSelected_PushButton;
      PushButton Clear_PushButton;
      CheckBox FullPaths_CheckBox;

      SectionBar Output_SectionBar;
      Control Output_Control;
      VerticalSizer Output_Sizer;
      HorizontalSizer OutputDir_Sizer;
      CheckBox OutputTree_CheckBox;
      Edit OutputDir_Edit;
      ToolButton OutputDir_SelectButton;
      ToolButton OutputDir_ClearButton;
      HorizontalSizer OutputChunks_Sizer;
      CheckBox OutputSubDirCFA_CheckBox;
      CheckBox Overwrite_CheckBox;
      Label Prefix_Label;
      Edit Prefix_Edit;
      Label Postfix_Label;
      Edit Postfix_Edit;
   };

   GUIData* GUI = nullptr;

   void SelectDir();                                        // Select output directory
   void SearchFile( const String&, const String& );         // Recursiv Search file in sub-folders
   int FileInList( const String& );                         // is a file in target frames or not? : -1 = not found; otherwise return pointer >=0
   void AddFile( const String&, const String& = String() ); // add one file to target list
   void AddFiles();                                         // add selected files
   void AddFolders();                                       // search all sub-folders and add all files
   void RemoveSelectedFiles();

   void UpdateControls();
   void UpdateTargetImagesList();
   void UpdateImageSelectionButtons();

   void __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __TargetImages_NodeSelectionUpdated( TreeBox& sender );
   void __TargetImages_BottonClick( Button& sender, bool checked );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers );
   void __EditCompleted( Edit& sender );
   void __Button_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SplitCFAInterface* TheSplitCFAInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __SplitCFAInterface_h

// ----------------------------------------------------------------------------
// EOF SplitCFAInterface.h - Released 2025-04-07T08:53:56Z
