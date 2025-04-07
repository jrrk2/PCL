//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 1.7.2
// ----------------------------------------------------------------------------
// RescaleInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __RescaleInterface_h
#define __RescaleInterface_h

#include <pcl/GroupBox.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>

#include "RescaleInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class RescaleInterface : public ProcessInterface
{
public:

   RescaleInterface();
   virtual ~RescaleInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   RescaleInstance m_instance;

   struct GUIData
   {
      GUIData( RescaleInterface& );

      VerticalSizer    Global_Sizer;
         GroupBox         Rescale_GroupBox;
         VerticalSizer    Rescale_Sizer;
            RadioButton      RGBK_RadioButton;
            RadioButton      RGBK_Individual_RadioButton;
            RadioButton      CIEL_RadioButton;
            RadioButton      CIEY_RadioButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void RescaleButtonClick( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern RescaleInterface* TheRescaleInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RescaleInterface_h

// ----------------------------------------------------------------------------
// EOF RescaleInterface.h - Released 2025-04-07T08:53:56Z
