//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 1.4.2
// ----------------------------------------------------------------------------
// FastRotationInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __FastRotationInterface_h
#define __FastRotationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/RadioButton.h>

#include "FastRotationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FastRotationInterface : public ProcessInterface
{
public:

   FastRotationInterface();
   virtual ~FastRotationInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   FastRotationInstance m_instance;

   struct GUIData
   {
      GUIData( FastRotationInterface& );

      GroupBox       Transform_GroupBox;
      VerticalSizer  Transform_Sizer;
      RadioButton    Rotate180_RadioButton;
      RadioButton    Rotate90CW_RadioButton;
      RadioButton    Rotate90CCW_RadioButton;
      RadioButton    HorizontalMirror_RadioButton;
      RadioButton    VerticalMirror_RadioButton;

      VerticalSizer  Global_Sizer;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void e_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern FastRotationInterface* TheFastRotationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FastRotationInterface_h

// ----------------------------------------------------------------------------
// EOF FastRotationInterface.h - Released 2025-04-07T08:53:56Z
