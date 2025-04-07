//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NetworkService Process Module Version 1.2.0
// ----------------------------------------------------------------------------
// NetworkServiceInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NetworkService PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __NetworkServiceInterface_h
#define __NetworkServiceInterface_h

#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>
#include <pcl/TextBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CLASS NetworkTransfer;

class NetworkServiceInterface : public ProcessInterface
{
public:

   NetworkServiceInterface();
   virtual ~NetworkServiceInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   InterfaceFeatures Features() const override;
   bool IsInstanceGenerator() const override;
   bool CanImportInstances() const override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;

private:

   String m_taskProviderURL;
   String m_workerName;
   String m_taskType;
   String m_userName;
   String m_userPassword;

   struct GUIData
   {
      GUIData( NetworkServiceInterface& );

      VerticalSizer     Global_Sizer;
         HorizontalSizer   TaskProvider_Sizer;
            Label             TaskProvider_Label;
            Edit              TaskProvider_Edit;
         HorizontalSizer   TaskType_Sizer;
            Label             TaskType_Label;
            Edit              TaskType_Edit;
         HorizontalSizer   WorkerName_Sizer;
            Label             WorkerName_Label;
            Edit              WorkerName_Edit;
         HorizontalSizer   UserName_Sizer;
            Label             UserName_Label;
            Edit              UserName_Edit;
         HorizontalSizer   UserPassword_Sizer;
            Label             UserPassword_Label;
            Edit              UserPassword_Edit;
         TextBox           Log_TextBox;
         Label             Status_Label;
         HorizontalSizer   Buttons_Sizer;
            PushButton        Start_PushButton;
            PushButton        Stop_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_EditGetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );
   void e_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
   friend class UILogger;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern NetworkServiceInterface* TheNetworkServiceInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __NetworkServiceInterface_h

// ----------------------------------------------------------------------------
// EOF NetworkServiceInterface.h - Released 2025-04-07T08:53:56Z
