//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 2.9.4
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 1.0.2
// ----------------------------------------------------------------------------
// GREYCstorationInterface.h - Released 2025-04-07T08:53:56Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
//
// Copyright (c) 2003-2025 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Use of this source code is governed by the PixInsight Class Library License
// version 2.0, which can be found in the LICENSE file as well as at:
// https://pixinsight.com/license/PCL-License-2.0.html
// ----------------------------------------------------------------------------

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#ifndef __GREYCstorationInterface_h
#define __GREYCstorationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>

#include "GREYCstorationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class GREYCstorationInterface : public ProcessInterface
{
public:

   GREYCstorationInterface();
   virtual ~GREYCstorationInterface();

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

   GREYCstorationInstance instance;

   struct GUIData
   {
      GUIData( GREYCstorationInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   Iterations_Sizer;
         Label             Iterations_Label;
         SpinBox           Iterations_SpinBox;
      NumericControl    Amplitude_NumericControl;
      NumericControl    Sharpness_NumericControl;
      NumericControl    Anisotropy_NumericControl;
      NumericControl    Alpha_NumericControl;
      NumericControl    Sigma_NumericControl;
      NumericControl    SpatialStepSize_NumericControl;
      NumericControl    AngularStepSize_NumericControl;
      NumericControl    Precision_NumericControl;
      HorizontalSizer   Interpolation_Sizer;
         Label             Interpolation_Label;
         ComboBox          Interpolation_ComboBox;
      HorizontalSizer   FastApproximation_Sizer;
         CheckBox          FastApproximation_CheckBox;
         CheckBox          CoupledChannels_CheckBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __ValueUpdated( NumericEdit& sender, double value );
   void __IntValueUpdated( SpinBox& sender, int value );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __Click( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GREYCstorationInterface* TheGREYCstorationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GREYCstorationInterface_h

// ----------------------------------------------------------------------------
// EOF GREYCstorationInterface.h - Released 2025-04-07T08:53:56Z
