//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 1.2.3
// ----------------------------------------------------------------------------
// DeviceConfigBase.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2025 Klaus Kretzschmar
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __DeviceConfigBase_h
#define __DeviceConfigBase_h

#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/Sizer.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ConfigDialogBase : public Dialog
{
public:

   ConfigDialogBase( const String& deviceName );

protected:

   bool m_firstTimeShown = true;
   String m_device;

   VerticalSizer Global_Sizer;
   HorizontalSizer ConfigButton_Sizer;
   PushButton SaveConfig_Button;
   PushButton Ok_Button;
   PushButton Cancel_Button;

   virtual void SendUpdatedProperties() = 0;

   void AddBaseControls();

   void e_Show( Control& sender );
   void e_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

} // namespace pcl

#endif // __DeviceConfigBase_h

// ----------------------------------------------------------------------------
// EOF DeviceConfigBase.h - Released 2025-04-07T08:53:56Z
