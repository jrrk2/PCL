//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SandboxInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SandboxInterface_h
#define __SandboxInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "SandboxInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SandboxInterface : public ProcessInterface
{
public:

   SandboxInterface();
   virtual ~SandboxInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   String IconImageSVGFile() const override;
   void ApplyInstance() const override;
   void ResetInstance() override;
   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ ) override;
   ProcessImplementation* NewProcess() const override;
   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;
   bool ImportProcess( const ProcessImplementation& ) override;

private:

   /*
    * The instance being defined
    */
   SandboxInstance m_instance;

   /*
    * Child controls
    */
   struct GUIData
   {
      GUIData( SandboxInterface& );

      VerticalSizer     Global_Sizer;
         NumericControl    ParameterOne_NumericControl;
         HorizontalSizer   ParameterTwo_Sizer;
            Label             ParameterTwo_Label;
            SpinBox           ParameterTwo_SpinBox;
         HorizontalSizer   ParameterThree_Sizer;
            CheckBox          ParameterThree_CheckBox;
         HorizontalSizer   ParameterFour_Sizer;
            Label             ParameterFour_Label;
            ComboBox          ParameterFour_ComboBox;
         HorizontalSizer   ParameterFive_Sizer;
            Label             ParameterFive_Label;
            Edit              ParameterFive_Edit;
   };

   GUIData* GUI = nullptr;

   /*
    * Control updates
    */
   void UpdateControls();

   /*
    * Event handlers
    */
   void e_RealValueUpdated( NumericEdit& sender, double value );
   void e_IntegerValueUpdated( SpinBox& sender, int value );
   void e_ItemClicked( Button& sender, bool checked );
   void e_ItemSelected( ComboBox& sender, int itemIndex );
   void e_EditGetFocus( Control& sender );
   void e_EditCompleted( Edit& sender );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SandboxInterface* TheSandboxInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SandboxInterface_h

// ----------------------------------------------------------------------------
// EOF SandboxInterface.h - Released 2025-04-07T08:53:56Z
