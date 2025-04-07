//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// SimplexNoiseInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

#ifndef __SimplexNoiseInterface_h
#define __SimplexNoiseInterface_h

#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/RadioButton.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "SimplexNoiseInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SimplexNoiseInterface : public ProcessInterface
{
public:

   SimplexNoiseInterface();
   virtual ~SimplexNoiseInterface();

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

   SimplexNoiseInstance instance;

   struct GUIData
   {
      GUIData( SimplexNoiseInterface& );

      VerticalSizer     Global_Sizer;

      NumericControl    Amount_NumericControl;
      Control           Scale_Control;
      HorizontalSizer   Scale_Sizer;
         Label             Scale_Label;
         SpinBox           Scale_SpinBox;
         RadioButton       Scale1_RadioButton;
         RadioButton       Scale10_RadioButton;
         RadioButton       Scale100_RadioButton;
      Control           OffsetX_Control;
      HorizontalSizer   OffsetX_Sizer;
         Label             OffsetX_Label;
         SpinBox           OffsetX_SpinBox;
         RadioButton       OffsetX1_RadioButton;
         RadioButton       OffsetX10_RadioButton;
         RadioButton       OffsetX100_RadioButton;
      Control           OffsetY_Control;
      HorizontalSizer   OffsetY_Sizer;
         Label             OffsetY_Label;
         SpinBox           OffsetY_SpinBox;
         RadioButton       OffsetY1_RadioButton;
         RadioButton       OffsetY10_RadioButton;
         RadioButton       OffsetY100_RadioButton;
      HorizontalSizer   Operator_Sizer;
         Label             Operator_Label;
         ComboBox          Operator_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ValueUpdated( NumericEdit& sender, double value );
   void __IntValueUpdated( SpinBox& sender, int value );
   void __Click( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SimplexNoiseInterface* TheSimplexNoiseInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SimplexNoiseInterface_h

// ----------------------------------------------------------------------------
// EOF SimplexNoiseInterface.h - Released 2025-04-07T08:53:56Z
