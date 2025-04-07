//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Morphology Process Module Version 1.0.1
// ----------------------------------------------------------------------------
// StructureManagerDialog.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Morphology PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __StructureManagerDialog_h
#define __StructureManagerDialog_h

#include <pcl/Dialog.h>
#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/PushButton.h>

#include "MorphologicalTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class StructureManagerDialog : public Dialog
{
public:

   StructureManagerDialog( MorphologicalTransformationInterface& );

private:

   MorphologicalTransformationInterface& m_interface;

   VerticalSizer  Global_Sizer;

   GroupBox          Module_GroupBox;
   VerticalSizer     Module_Sizer;
      Label             ModulePath_Label;
      HorizontalSizer   ModuleButtons_Sizer;
         PushButton        LoadModule_PushButton;
         PushButton        SaveModule_PushButton;

   GroupBox          StructureList_GroupBox;
   VerticalSizer     StructureList_Sizer;
      ComboBox          StructureList_ComboBox;
      HorizontalSizer   StructureListButtons_Sizer;
         PushButton        PickStructure_PushButton;
         PushButton        DuplicateStructure_PushButton;
         PushButton        DeleteStructure_PushButton;
         PushButton        DeleteAll_PushButton;

   GroupBox          CurrentStructure_GroupBox;
   VerticalSizer     CurrentStructure_Sizer;
      Label             CurrentStructure_Label;
      HorizontalSizer   CurrentStructureButtons_Sizer;
         PushButton        SaveStructure_PushButton;

   HorizontalSizer   Buttons_Sizer;
      PushButton        Done_PushButton;

   void Update();

   void ModuleClick( Button& sender, bool checked );
   void StructureListClick( Button& sender, bool checked );
   void SaveStructureClick( Button& sender, bool checked );
   void DoneClick( Button& sender, bool checked );
   void ItemSelected( ComboBox& sender, int itemIndex );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StructureManagerDialog_h

// ----------------------------------------------------------------------------
// EOF StructureManagerDialog.h - Released 2025-04-07T08:53:56Z
