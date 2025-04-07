//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// INDIDeviceControllerInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __INDIDeviceControllerInterface_h
#define __INDIDeviceControllerInterface_h

#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Timer.h>
#include <pcl/TreeBox.h>

#include "INDIDeviceControllerInstance.h"
#include "ZeroconfServiceDetection.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIDeviceControllerInterface : public ProcessInterface
{
public:

   INDIDeviceControllerInterface();
   virtual ~INDIDeviceControllerInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;
   InterfaceFeatures Features() const override;

   void UpdateControls();

   struct GUIData
   {
      GUIData( INDIDeviceControllerInterface& );

      VerticalSizer Global_Sizer;
      SectionBar Server_SectionBar;
      Control Server_Control;
      HorizontalSizer Server_HSizer;
      VerticalSizer ServerName_VSizer;
      HorizontalSizer ServerName_HSizer;
      HorizontalSizer ServiceDetection_HSizer;
      Label HostName_Label;
      Edit HostName_Edit;
      Label Port_Label;
      SpinBox Port_SpinBox;
      CheckBox EnableServerDetection_CheckBox;
      VerticalSizer ServerAction_Sizer;
      PushButton Connect_PushButton;
      PushButton Disconnect_PushButton;
      SectionBar Devices_SectionBar;
      Control Devices_Control;
      HorizontalSizer Devices_Sizer;
      TreeBox Devices_TreeBox;
      VerticalSizer NodeAction_Sizer;
      PushButton NodeAction_PushButton;
      Label ServerMessage_Label;

      Timer SynchronizeWithServer_Timer;
#ifdef WITH_ZEROCONF
      ZeroConfServiceBrowser ZeroconfServiceBrowser_Thread;
      bool m_isIndigoServiceChanged = false;
#endif
   };

private:

   GUIData* GUI = nullptr;

   void UpdateDeviceLists();
   void UpdateNodeActionButtons( TreeBox::Node* );
   void AdjustTreeColumns();

   void e_Show( Control& );
   void e_Hide( Control& );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_Click( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node&, int col );
   void e_NodeDoubleClicked( TreeBox& sender, TreeBox::Node&, int col );
   void e_NodeExpanded( TreeBox& sender, TreeBox::Node& );
   void e_NodeCollapsed( TreeBox& sender, TreeBox::Node& );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Timer( Timer& sender );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIDeviceControllerInterface* TheINDIDeviceControllerInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __INDIDeviceControllerInterface_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInterface.h - Released 2025-04-07T08:53:56Z
