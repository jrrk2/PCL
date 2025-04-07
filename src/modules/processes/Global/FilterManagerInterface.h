//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 1.6.4
// ----------------------------------------------------------------------------
// FilterManagerInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FilterManagerInterface_h
#define __FilterManagerInterface_h

#include "FilterManagerInstance.h"

#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/FilterManager.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/ToolButton.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class CurveExplorerDialog;

class FilterManagerInterface : public ProcessInterface
{
public:

   FilterManagerInterface();
   virtual ~FilterManagerInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   bool WantsGlobalNotifications() const override;
   void GlobalFiltersUpdated() override;

private:

   FilterManagerInstance m_instance;
   CurveExplorerDialog*  m_curveExplorer = nullptr;

   struct GUIData
   {
      GUIData( FilterManagerInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   CurveExplorer_Sizer;
            PushButton        CurveExplorer_Button;
         HorizontalSizer   Task_Sizer;
            Label             Task_Label;
            ComboBox          Task_ComboBox;
         HorizontalSizer   Path_Sizer;
            Label             Path_Label;
            Edit              Path_Edit;
            ToolButton        Path_ToolButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_EditCompleted( Edit& sender );
   void e_Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern FilterManagerInterface* TheFilterManagerInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FilterManagerInterface_h

// ----------------------------------------------------------------------------
// EOF FilterManagerInterface.h - Released 2025-04-07T08:53:56Z
